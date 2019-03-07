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


#ifndef _CONDOR_HOOK_UTILS_H_
#define _CONDOR_HOOK_UTILS_H_

/**
  Lookup the given hook config parameter and make sure it is
  defined, pointing to a valid executable, and that we have
  some reason to believe that executable is trust-worthy.
  @param hook_param The name of the hook parameter
  @param hpath Returns with path to hook. NULL if not defined, or path error
  @return true if successful, false if there was a path error
*/
bool validateHookPath( const char* hook_param, char*& hpath );


#endif /* _CONDOR_HOOK_UTILS_H_ */
