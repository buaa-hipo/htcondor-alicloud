/***************************************************************
 *
 * Copyright (C) 1990-2011, Condor Team, Computer Sciences Department,
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

#if defined(AIX32)
#	define _BSD
#endif

#ifndef WIN32
#include <sys/wait.h>
#endif
#include "proc.h"
#include "condor_debug.h"

#if defined(AIX31) || defined(AIX32)
#include <time.h>
#endif

#include "util_lib_proto.h"

int		DontDisplayTime;

// note format_time.cpp has a version of this function that takes a time_t
// does anyone actually link to this one?
#define SECOND	1
#define MINUTE	(60 * SECOND)
#define HOUR	(60 * MINUTE)
#define DAY		(24 * HOUR)

char	*
format_time( float fp_secs )
{
	int		days;
	int		hours;
	int		min;
	int		secs;
	int		tot_secs = (int)fp_secs;
	static char	answer[25];

	days = tot_secs / DAY;
	tot_secs %= DAY;
	hours = tot_secs / HOUR;
	tot_secs %= HOUR;
	min = tot_secs / MINUTE;
	secs = tot_secs % MINUTE;

	(void)sprintf( answer, "%3d+%02d:%02d:%02d", days, hours, min, secs );
	return answer;
}


static const char* JobStatusNames[] = {
    NULL,
    "IDLE",
    "RUNNING",
    "REMOVED",
    "COMPLETED",
	"HELD",
	"TRANSFERRING_OUTPUT",
	"SUSPENDED",
};


const char*
getJobStatusString( int status )
{
	if( status < JOB_STATUS_MIN || status > JOB_STATUS_MAX ) {
		return "UNKNOWN";
	}
	return JobStatusNames[status];
}


int
getJobStatusNum( const char* name )
{
	int i;
	if( ! name ) {
		return -1;
	}
	for ( i = JOB_STATUS_MIN; i <= JOB_STATUS_MAX; i++ ) {
		if ( strcasecmp( name, JobStatusNames[i] ) == MATCH ) {
			return i;
		}
	}
	return -1;
}


