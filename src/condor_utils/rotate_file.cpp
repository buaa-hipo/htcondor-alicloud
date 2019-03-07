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
#include "util_lib_proto.h"

int
rotate_file(const char *old_filename, const char *new_filename)
{
	return rotate_file_dprintf(old_filename, new_filename, 0);
}

int
rotate_file_dprintf(const char *old_filename, const char *new_filename, int calledByDprintf)
{
#if defined(WIN32)
	/* We must use MoveFileEx on NT because rename can not overwrite
	   an existing file. */
	DWORD err;
	
	if (MoveFileEx(old_filename, new_filename,
				   MOVEFILE_COPY_ALLOWED |
                   MOVEFILE_REPLACE_EXISTING |
                   MOVEFILE_WRITE_THROUGH) == 0) {
		
		if (calledByDprintf == 0) {
			err = GetLastError();
			dprintf(D_ALWAYS, "MoveFileEx(%s,%s) failed with error %d\n",
					old_filename, new_filename, err);
		} else {
			// we are called by dprintf, therefore retry.
			Sleep(500);
			if (MoveFileEx(old_filename, new_filename,
				   MOVEFILE_COPY_ALLOWED |
                   MOVEFILE_REPLACE_EXISTING |
				   MOVEFILE_WRITE_THROUGH) == 0) {
					   if (CopyFile(old_filename, new_filename, 0) == 0) {
							return (int)GetLastError();
					   } else {
							return -2; // copy succeeded but old file still there.
					   }
			   // we succeeded on second try.
			} else {
				return 0;
			}
			
		}
			
		return -1;
	}
#else
	if (rename(old_filename, new_filename) < 0) {
		if (calledByDprintf == 0) {
			dprintf(D_ALWAYS, "rename(%s, %s) failed with errno %d\n",
					old_filename, new_filename, errno);
		} else {
			return errno; // return errno if called by dprintf
		}
		return -1;
	}
#endif

	return 0;
}
