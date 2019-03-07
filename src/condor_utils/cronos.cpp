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


 

/*****************************************************************
**
** Maintain a list of events which are scheduled to occur on a periodic
** basis.  Events are represented by a function which will be executed at
** the time(s) when the event is scheduled to occur.  The specification of
** when the event is to occur is a 5-tuple, (month,day,hour,mintue,second)
** which is represented as an array of integers.  Zero or more elements in
** the tuple may be specified as STAR which serves as a wildcard in the
** manner of cron(1).  Star is represented by -1 in the integer array.
** The application should call event_mgr() regularly at times when it
** would be convenient to service an event.  At each call to event_mgr()
** any events which were scheduled to occur between the last call to
** event_mgr() and the current time will be executed.  This package does
** not use any timers or signals, and does not mess with SIGALRM.
** Exports:
** 	schedule_event( month, day, hour, minute, second, func )
** 	int		month, day, hour, minute, second;
** 	int 	(*func)();
** 
** 	event_mgr()
** 
** Example:
** 
** 	Assume two functions chime() which plays a simple tune and ring()
** 	which rings the bell a requisite number of times for the current hour
** 	of the day.  Then a clock which chimes on the quarter hour and rings
** 	on the hour can be specified as follows.  Note: this is not the way
** 	to implement this program in real life.
** 
** 	schedule_event( STAR, STAR, STAR, 0, 0, chime );
** 	schedule_event( STAR, STAR, STAR, 15, 0, chime );
** 	schedule_event( STAR, STAR, STAR, 30, 0, chime );
** 	schedule_event( STAR, STAR, STAR, 45, 0, chime );
** 	schedule_event( STAR, STAR, STAR, 0, 0, ring );
** 	schedule_event( 12, 31, 23, 59, 45, ring );		-- Special celebration
** 	schedule_event( 12, 31, 23, 59, 50, ring );		-- for New Year's Eve
** 	schedule_event( 12, 31, 23, 59, 55, ring );
** 
** 	for(;;) {
** 		event_mgr();
** 		sleep( 1 );
** 	}
**
*****************************************************************/

#include "condor_common.h"

/*
** Represent a moment in time by a 5 element tuple
** (month,day,hour,minute,second)
*/
#define CMONTH	0
#define CDAY	1
#define CHOUR	2
#define CMINUTE	3
#define CSECOND	4
#define N_ELEM	5

typedef void (*FUNC_P)(void);

/*
** Events are to occur at times which match a pattern.  Patterns are
** represented by the same tuples, but may contain "*" as one or more
** elements.  E.g. (*,*,*,0,0) meaning every hour on the hour.
*/
#define STAR -1
typedef struct {
	int		pattern[N_ELEM];
	FUNC_P	func;
} EVENT;

static EVENT	Schedule[128];
static int	N_Events = 0;
static int	did_startup = 0;
static int	prev_time[N_ELEM];
static int	now_time[N_ELEM];

extern "C"
{
void schedule_event( int , int , int , int , int , FUNC_P );
void event_mgr( void );
static int event_due( int pattern[], int prev[], int now[] );
static int before( int t1[], int t2[] );
static int next_rightmost( int pattern[], int i );
static void get_moment( int cur[] );
static void check_schedule( int prev[], int now[] );


/* Exported function */
void
schedule_event( int month, int day, int hour, int minute, int second,
															FUNC_P func )
{
	Schedule[N_Events].pattern[CMONTH] = month;
	Schedule[N_Events].pattern[CDAY] = day;
	Schedule[N_Events].pattern[CHOUR] = hour;
	Schedule[N_Events].pattern[CMINUTE] = minute;
	Schedule[N_Events].pattern[CSECOND] = second;
	Schedule[N_Events].func = func;
	N_Events += 1;
}

/* Exported function */
void
event_mgr( void )
{

	if( !did_startup ) {
		get_moment( prev_time );
		did_startup = 1;
		return;
	}
	get_moment( now_time );
	check_schedule( prev_time, now_time );
	memcpy( prev_time,now_time, sizeof(now_time) ); /* ..dhaval 9/25 */
}


static void
check_schedule( int prev[], int now[] )
{
	int		i;

	for( i=0; i<N_Events; i++ ) {
		if( event_due(Schedule[i].pattern,prev,now) ) {
			(*Schedule[i].func)();
		}
	}
}

	
/*
** Given a pattern which specifies the time(s) an event is to occur and
** tuples representing the previous time we checked and the current time,
** return true if the event should be triggered.
**
** The idea is that a pattern represents a (potentially large) set of moments
** in time.  We generate the earliest member of this set which comes after
** prev, and call it alpha.  We then compare alpha with the current time to
** see if the event is due, (or overdue), now.
*/
static
int
event_due( int pattern[], int prev[], int now[] )
{
	int		alpha[N_ELEM];
	int		i;

		/* First approximation to alpha */
	for( i=0; i<N_ELEM; i++ ) {
		if( pattern[i] == STAR ) {
			alpha[i] = prev[i];
		} else {
			alpha[i] = pattern[i];
		}
	}

		/* Find earliest time matching pattern which is after prev */
	if( before(alpha,prev) ) {
		i = next_rightmost( pattern, N_ELEM );
		if( i < 0 ) {
			return FALSE;
		}
		for(;;) {
			alpha[i] += 1;
			if( before(prev,alpha) ) {
				break;
			} else {
				alpha[i] = 0;
				i = next_rightmost( pattern, i );
				if( i < 0 ) {
					return FALSE;
				}
			}
		}
	}

	return before( alpha, now );
}

/*
** Return true if t1 is before t2, and false otherwise.
*/
static
int
before( int t1[], int t2[] )
{
	int		i;

	for( i=0; i<N_ELEM; i++ ) {
		if( t1[i] < t2[i] ) {
			return TRUE;
		}
		if( t1[i] > t2[i] ) {
			return FALSE;
		}
	}
	return FALSE;
}

/*
** Given a pattern and a subscript and a pattern, return the subscript of
** the next rightmost STAR in the pattern.  If there is none, return -1.
*/
static
int
next_rightmost( int pattern[], int i )
{
	for( i--; i >= 0; i-- ) {
		if( pattern[i] == STAR ) {
			return i;
		}
	}
	return -1;
}

/*
** Fill in a tuple with the current time.
*/
static void
get_moment( int cur[] )
{
	struct tm	*tm;
	time_t	curr_time;

	(void)time( &curr_time );
	tm = localtime( &curr_time );
	cur[CMONTH] = tm->tm_mon + 1;
	cur[CDAY] = tm->tm_mday;
	cur[CHOUR] = tm->tm_hour;
	cur[CMINUTE] = tm->tm_min;
	cur[CSECOND] = tm->tm_sec;
}

}
