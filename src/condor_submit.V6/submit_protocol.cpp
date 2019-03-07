/***************************************************************
 *
 * Copyright (C) 1990-2015, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/

#include "condor_common.h"
#include "condor_config.h"
#include "condor_debug.h"
#include "condor_network.h"
#include "condor_string.h"
#include "spooled_job_files.h"
#include "subsystem_info.h"
#include "env.h"
#include "basename.h"
#include "condor_getcwd.h"
#include <time.h>
#include "write_user_log.h"
#include "condor_classad.h"
#include "condor_attributes.h"
#include "condor_adtypes.h"
#include "condor_io.h"
#include "condor_distribution.h"
#include "condor_ver_info.h"
#if !defined(WIN32)
#include <pwd.h>
#include <sys/stat.h>
#else
// WINDOWS only
#include "store_cred.h"
#endif
#include "internet.h"
#include "my_hostname.h"
#include "domain_tools.h"
#include "get_daemon_name.h"
#include "condor_qmgr.h"
#include "sig_install.h"
#include "access.h"
#include "daemon.h"
#include "match_prefix.h"

#include "extArray.h"
#include "HashTable.h"
#include "MyString.h"
#include "string_list.h"
#include "which.h"
#include "sig_name.h"
#include "print_wrapped_text.h"
#include "dc_schedd.h"
#include "dc_collector.h"
#include "my_username.h"
#include "globus_utils.h"
#include "enum_utils.h"
#include "setenv.h"
#include "classad_hashtable.h"
#include "directory.h"
#include "filename_tools.h"
#include "fs_util.h"
#include "dc_transferd.h"
#include "condor_ftp.h"
#include "condor_crontab.h"
#include <scheduler.h>
#include "condor_holdcodes.h"
#include "condor_url.h"
#include "condor_version.h"

#include "list.h"
#include "condor_vm_universe_types.h"
#include "vm_univ_utils.h"
#include "condor_md.h"
#include "submit_internal.h"

#include <algorithm>
#include <string>
#include <set>


ActualScheddQ::~ActualScheddQ()
{
	// The queue management protocol is finicky, for now we do NOT disconnect in the destructor.
	qmgr = NULL;
}

bool ActualScheddQ::Connect(DCSchedd & MySchedd, CondorError & errstack) {
	if (qmgr) return true;
	qmgr = ConnectQ(MySchedd.addr(), 0 /* default */, false /* default */, &errstack, NULL, MySchedd.version());
	allows_late = has_late = false;
	if (qmgr) {
		CondorVersionInfo cvi(MySchedd.version());
		if (cvi.built_since_version(8,7,1)) {
			has_late = true;
			allows_late = param_boolean("SCHEDD_ALLOW_LATE_MATERIALIZE",has_late);
		}
	}
	return qmgr != NULL;
}

bool ActualScheddQ::disconnect(bool commit_transaction, CondorError & errstack) {
	bool rval = false;
	if (qmgr) {
		rval = DisconnectQ(qmgr, commit_transaction, &errstack);
	}
	qmgr = NULL;
	return rval;
}

int ActualScheddQ::get_NewCluster() { return NewCluster(); }
int ActualScheddQ::get_NewProc(int cluster_id) { return NewProc(cluster_id); }
int ActualScheddQ::destroy_Cluster(int cluster_id, const char *reason) { return DestroyCluster(cluster_id, reason); }

int ActualScheddQ::init_capabilities() {
	int rval = 0;
	if ( ! tried_to_get_capabilities) {
		rval = GetScheddCapabilites(0, capabilities);
		tried_to_get_capabilities = true;

		// fetch late materialize caps from the capabilities ad.
		allows_late = has_late = false;
		if ( ! capabilities.LookupBool("LateMaterialize", allows_late)) {
			allows_late = has_late = false;
		} else {
			has_late = true; // schedd knows about late materialize
		}
	}
	return rval;
}
bool ActualScheddQ::has_late_materialize() {
	init_capabilities();
	return has_late;
}
bool ActualScheddQ::allows_late_materialize() {
	init_capabilities();
	return allows_late;
}
int ActualScheddQ::get_Capabilities(ClassAd & caps) {
	int rval = init_capabilities();
	if (rval == 0) {
		caps.Update(capabilities);
	}
	return rval;
}

int ActualScheddQ::set_Attribute(int cluster, int proc, const char *attr, const char *value, SetAttributeFlags_t flags) {
	return SetAttribute(cluster, proc, attr, value, flags);
}

int ActualScheddQ::set_AttributeInt(int cluster, int proc, const char *attr, int value, SetAttributeFlags_t flags) {
	return SetAttributeInt(cluster, proc, attr, value, flags);
}

int ActualScheddQ::set_Factory(int cluster, int qnum, const char * filename, const char * text) {
	return SetJobFactory(cluster, qnum, filename, text);
}

int ActualScheddQ::set_Foreach(int cluster, int itemnum, const char * filename, const char * text) {
	return SetMaterializeData(cluster, itemnum, filename, text);
}

int ActualScheddQ::send_SpoolFileIfNeeded(ClassAd& ad) { return SendSpoolFileIfNeeded(ad); }
int ActualScheddQ::send_SpoolFile(char const *filename) { return SendSpoolFile(filename); }
int ActualScheddQ::send_SpoolFileBytes(char const *filename) { return SendSpoolFileBytes(filename); }

//====================================================================================
// functions for a simulate schedd q
//====================================================================================

SimScheddQ::SimScheddQ(int starting_cluster)
	: cluster(starting_cluster)
	, proc(-1)
	, close_file_on_disconnect(false)
	, log_all_communication(false)
	, fp(NULL)
{
}

SimScheddQ::~SimScheddQ()
{
	if (fp && close_file_on_disconnect) {
		fclose(fp);
	}
	fp = NULL;
}

bool SimScheddQ::Connect(FILE* _fp, bool close_on_disconnect, bool log_all) {
	ASSERT( ! fp);
	fp = _fp;
	close_file_on_disconnect = close_on_disconnect;
	log_all_communication = log_all;
	return fp != NULL;
}

bool SimScheddQ::disconnect(bool /*commit_transaction*/, CondorError & /*errstack*/)
{
	if (fp && close_file_on_disconnect) {
		fclose(fp);
	}
	fp = NULL;
	return true;
}

int SimScheddQ::get_NewCluster() {
	proc = -1;
	if (log_all_communication) fprintf(fp, "::get_newCluster\n");
	return ++cluster;
}

int SimScheddQ::get_NewProc(int cluster_id) {
	ASSERT(cluster == cluster_id);
	if (fp) { 
		if (log_all_communication) fprintf(fp, "::get_newProc\n");
		fprintf (fp, "\n");
	}
	return ++proc;
}

int SimScheddQ::destroy_Cluster(int cluster_id, const char * /*reason*/) {
	ASSERT(cluster_id == cluster);
	return 0;
}

int SimScheddQ::get_Capabilities(ClassAd & caps) {
	caps.Assign( "LateMaterialize", true );
	return GetScheddCapabilites(0, caps);
}


int SimScheddQ::set_Attribute(int cluster_id, int proc_id, const char *attr, const char *value, SetAttributeFlags_t /*flags*/) {
	ASSERT(cluster_id == cluster);
	ASSERT(proc_id == proc || proc_id == -1);
	if (fp) {
		if (log_all_communication) fprintf(fp, "::set(%d,%d) ", cluster_id, proc_id);
		fprintf(fp, "%s=%s\n", attr, value);
	}
	return 0;
}
int SimScheddQ::set_AttributeInt(int cluster_id, int proc_id, const char *attr, int value, SetAttributeFlags_t /*flags*/) {
	ASSERT(cluster_id == cluster);
	ASSERT(proc_id == proc || proc_id == -1);
	if (fp) {
		if (log_all_communication) fprintf(fp, "::int(%d,%d) ", cluster_id, proc_id);
		fprintf(fp, "%s=%d\n", attr, value);
	}
	return 0;
}

int SimScheddQ::set_Factory(int cluster_id, int qnum, const char * filename, const char * text) {
	ASSERT(cluster_id == cluster);
	if (fp) {
		if (log_all_communication) fprintf(fp, "::setFactory(%d,%d,%s,%s) ", cluster_id, qnum, filename?filename:"NULL", text?"<text>":"NULL");
		//PRAGMA_REMIND("print the submit digest")
		//fprintf(fp, "%s=%d\n", attr, value);
	}
	return 0;
}

int SimScheddQ::set_Foreach(int cluster_id, int itemnum, const char * filename, const char * text) {
	ASSERT(cluster_id == cluster);
	if (fp) {
		if (log_all_communication) fprintf(fp, "::setForeach(%d,%d,%s,%s) ", cluster_id, itemnum, filename?filename:"NULL", text?"<items>":"NULL");
		//PRAGMA_REMIND("print the foreach data")
		//fprintf(fp, "%s=%d\n", attr, value);
	}
	return 0;
}

int SimScheddQ::send_SpoolFileIfNeeded(ClassAd& ad) {
	if (fp) {
		fprintf(fp, "::send_SpoolFileIfNeeded\n");
		fPrintAd(fp, ad);
	}
	return 0;
}
int SimScheddQ::send_SpoolFile(char const * filename) {
	if (fp) { fprintf(fp, "::send_SpoolFile: %s\n", filename); }
	return 0;
}
int SimScheddQ::send_SpoolFileBytes(char const * filename) {
	if (fp) { fprintf(fp, "::send_SpoolFileBytes: %s\n", filename); }
	return 0;
}

