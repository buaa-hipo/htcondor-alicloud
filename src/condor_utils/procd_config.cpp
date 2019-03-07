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

#include "condor_common.h"
#include "condor_debug.h"
#include "condor_config.h"
#include "directory.h"
#include "procd_config.h"

MyString
get_procd_address()
{
	MyString ret;

	char* procd_addr = param("PROCD_ADDRESS");
	if (procd_addr != NULL) {
		ret = procd_addr;
		free(procd_addr);
	}
	else {
		// setup a good default for PROCD_ADDRESS.
#ifdef WIN32
		// Win32
		//
		ret = "//./pipe/procd_pipe";
#else
		// Unix - default to $(LOCK)/procd_pipe
		//
		char *lockdir = param("LOCK");
		if (!lockdir) {
			lockdir = param("LOG");
		}
		if (!lockdir) {
			EXCEPT("PROCD_ADDRESS not defined in configuration");
		}
		char *temp = dircat(lockdir,"procd_pipe");
		ASSERT(temp);
		ret = temp;
		free(lockdir);
		delete [] temp;
#endif
	}

	return ret;
}
