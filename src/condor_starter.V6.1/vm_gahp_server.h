/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
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


#ifndef CONDOR_VM_GAHP_SERVER_H
#define CONDOR_VM_GAHP_SERVER_H

#include "condor_common.h"
#include "condor_classad.h"
#include "condor_daemon_core.h"
#include "condor_distribution.h"
#include "gahp_common.h"
#include "HashTable.h"
#include "MyString.h"
#include "condor_string.h"
#include "condor_arglist.h"
#include "vm_gahp_request.h"

class VMGahpRequest;
class VMGahpServer : public Service {
	public:
		VMGahpServer(const char *vmgahpserver,
		             const char *vmtype,
		             ClassAd* job_ad);
		virtual ~VMGahpServer();

		bool startUp(Env *job_env, const char* job_iwd, int nice_inc, 
				FamilyInfo *family_info);
		bool cleanup(void);

		void setPollInterval(unsigned int interval);
		unsigned int getPollInterval(void);

		void cancelPendingRequest(int req_id);
		bool isPendingRequest(int req_id);
		VMGahpRequest *findRequestbyReqId(int req_id);

		bool nowPending(const char *command, const char *args, 
				VMGahpRequest *req);

		int numOfPendingRequests(void);

		// Result will be stored in m_pending_result of VMGahpRequest
		void getPendingResult(int req_id, bool is_blocking);

		// Return the pid of vmgahp
		int getVMGahpServerPid(void) {return m_vmgahp_pid;}

		// Return VM type 
		const char* getVMType(void) {return m_vm_type.Value();}

		bool isSupportedCommand(const char *command);
		bool isSupportedVMType(const char *vmtype);

		bool publishVMClassAd(const char *workingdir);

		void setVMid(int vm_id);

		// Print system error message to dprintf
		void printSystemErrorMsg(void);

		// Error message during startUp
		MyString start_err_msg;

	protected:
		bool read_argv(Gahp_Args &g_args);
		bool read_argv(Gahp_Args *g_args) { return read_argv(*g_args);}
		bool write_line(const char *command);
		bool write_line(const char *command, int req, const char *args);
		int pipe_ready(int pipe_end);
		int err_pipe_ready(int pipe_end);
		int poll();
		int poll_now();
		void poll_real_soon();

		int new_reqid(void);

		// Methods for VMGahp commands
		bool command_version(void);
		bool command_commands(void);
		bool command_support_vms(void);
		bool command_async_mode_on(void);
		bool command_quit(void);

	private:
		int m_is_initialized;
		bool m_is_cleanuped;
		bool m_is_async_mode;
		bool m_send_all_classad;

		// Does Starter log include log from vmgahp?
		bool m_include_gahp_log;

		MyString m_vm_type;
		MyString m_vmgahp_server;
		MyString m_vmgahp_config;
		ClassAd *m_job_ad;
		MyString m_workingdir;

		int m_vmgahp_pid;
		int m_vm_id;
		int m_vmgahp_readfd;
		int m_vmgahp_writefd;
		int m_vmgahp_errorfd;

		HashTable<int,VMGahpRequest*> m_request_table;

		unsigned int m_pollInterval;
		int m_poll_tid;
		int m_poll_real_soon_tid;
		int m_stderr_tid;

		int m_next_reqid;
		bool m_rotated_reqids;

		MyString m_vmgahp_version;
		MyString m_vmgahp_error_buffer;
		StringList m_commands_supported;
		StringList m_vms_supported;

		void killVM(void);
};

#endif //CONDOR_VM_GAHP_SERVER_H
