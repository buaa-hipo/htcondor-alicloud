/***************************************************************
 *
 * Copyright (C) 1990-2009, Condor Team, Computer Sciences Department,
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

/***************************************************************
 * Headers
 ***************************************************************/

#include "condor_common.h"
#include "condor_debug.h"
#include "hibernator.h"
#include "string_list.h"


/***************************************************************
 * Base Hibernator class
 ***************************************************************/

HibernatorBase::HibernatorBase () throw ()
		: m_states ( NONE ),
		  m_initialized( false )
{
}


HibernatorBase::~HibernatorBase () throw ()
{
}

bool
HibernatorBase::isStateValid ( SLEEP_STATE state )
{
	switch ( state ) {
	case NONE:	// Do nothing
		return true;
	case S1:
		return true;
	case S2:
		return true;
	case S3:
		return true;
	case S4:
		return true;
	case S5:
		return true;
	default:
		return false;
	}
}

bool
HibernatorBase::isStateSupported ( SLEEP_STATE state ) const
{
	if ( NONE == state ) {
		return true;
	}
	return (m_states & state) ? true : false;
}


bool
HibernatorBase::switchToState ( SLEEP_STATE level,
								SLEEP_STATE &new_level,
								bool force ) const
{
	if ( ! isStateValid( level ) ) {
		dprintf ( D_ALWAYS, "Hibernator: Invalid power state 0x%02x\n",
				  level );
		return false;
	}
	if ( ! isStateSupported( level )  ) {
		dprintf ( D_ALWAYS, "Hibernator: This machine does not "
			"support low power state: %s\n", sleepStateToString(level) );
		return false;
	}
	dprintf ( D_FULLDEBUG, "Hibernator: Entering sleep "
			  "state '%s'.\n", sleepStateToString ( level ) );

	new_level = NONE;
	switch ( level ) {
		/* S[1-3] will all be treated as "suspend to RAM" */
	case S1:
		new_level = enterStateStandBy( force );
		break;

	case S2:
	case S3:
		new_level = enterStateSuspend( force );
		break;

		/* S4 will all be treated as hibernate */
	case S4:
		new_level = enterStateHibernate( force );
		break;

		/* S5 will be treated as shutdown (soft-off) */
	case S5:
		new_level = enterStatePowerOff( force );
		break;

	default:
		/* should never happen */
		return false;
	}

	return true;
}

unsigned short
HibernatorBase::getStates () const
{
	return m_states;
}

void
HibernatorBase::setStates ( unsigned short states )
{
	m_states = states;
}

void
HibernatorBase::addState ( SLEEP_STATE state )
{
	m_states |= state;
}

void
HibernatorBase::addState ( const char *state )
{
	m_states |= stringToSleepState ( state );
}

const char*
HibernatorBase::getMethod () const
{
	return "default";
}

/***************************************************************
 * Hibernator static members
 ***************************************************************/

/* factory method */

/* conversion methods */
struct HibernatorBase::StateLookup
{
	int							number;
	HibernatorBase::SLEEP_STATE	state;
	const char					**strings;
};
static const char *s0names[] = { "NONE", "0", NULL };
static const char *s1names[] = { "S1",   "1", "standby", "sleep", NULL };
static const char *s2names[] = { "S2",   "2", NULL};
static const char *s3names[] = { "S3",   "3", "ram", "mem", "suspend", NULL };
static const char *s4names[] = { "S4",   "4", "disk", "hibernate", NULL };
static const char *s5names[] = { "S5",   "5", "shutdown", "off", NULL };
static const char *sxnames[] = { NULL };
static const HibernatorBase::StateLookup states[] =
{
	{ 0,  HibernatorBase::NONE, s0names, },
	{ 1,  HibernatorBase::S1,   s1names, },
	{ 2,  HibernatorBase::S2,   s2names, },
	{ 3,  HibernatorBase::S3,   s3names, },
	{ 4,  HibernatorBase::S4,   s4names, },
	{ 5,  HibernatorBase::S5,   s5names, },
	{ -1, HibernatorBase::NONE, sxnames, },
};

HibernatorBase::SLEEP_STATE
HibernatorBase::intToSleepState ( int n )
{
	return Lookup(n).state;
}

int
HibernatorBase::sleepStateToInt ( HibernatorBase::SLEEP_STATE state )
{
	return Lookup(state).number;
}

char const*
HibernatorBase::sleepStateToString ( HibernatorBase::SLEEP_STATE state )
{
	int index = sleepStateToInt ( state );
	return states[index].strings[0];
}

HibernatorBase::SLEEP_STATE
HibernatorBase::stringToSleepState ( char const *name )
{
	return Lookup(name).state;
}

bool
HibernatorBase::maskToStates(
	unsigned			   mask,
	ExtArray<SLEEP_STATE> &_states )
{
	_states.truncate(-1);
	unsigned bit;
	for ( bit = (unsigned)S1;
		  bit <= (unsigned)S5;
		  bit <<= 1 ) {
		if ( bit & mask ) {
			_states.add( (SLEEP_STATE)bit );
		}
	}
	return true;
}

bool
HibernatorBase::statesToString( const ExtArray<SLEEP_STATE> &_states,
								MyString &str )
{
	str = "";
	for( int i = 0;  i <= _states.getlast();  i++ ) {
		if ( i ) {
			str += ",";
		}
		str += sleepStateToString( _states[i] );
	}
	return true;
}

bool
HibernatorBase::maskToString( unsigned mask, MyString &str )
{
	ExtArray<SLEEP_STATE>	_states;
	if( !maskToStates( mask, _states ) ) {
		return false;
	}
	return statesToString( _states, str );
}

bool
HibernatorBase::stringToStates( const char *str,
								ExtArray<SLEEP_STATE> &_states )
{
	_states.truncate(-1);
	StringList	strlist( str );
	strlist.rewind();
	const char	*name;
	int			n = 0;
	while( (name = strlist.next()) != NULL ) {
		SLEEP_STATE state = stringToSleepState( name );
		_states.add( state );
		n++;
	}
	return (n >= 1);
}

bool
HibernatorBase::statesToMask( const ExtArray<SLEEP_STATE> &_states,
							  unsigned &mask )
{
	mask = 0x0;
	for( int i = 0;  i <= _states.getlast();  i++ ) {
		mask |= _states[i];
	}
	return true;
}

bool
HibernatorBase::stringToMask( const char *str,
							  unsigned &mask )
{
	mask = 0x0;

	ExtArray<SLEEP_STATE> _states;
	if( !stringToStates( str, _states ) ) {
		return false;
	}
	return statesToMask( _states, mask );
}


const HibernatorBase::StateLookup&
HibernatorBase::Lookup ( int n )
{
	if ( (n > 0)  &&  (n <= 5) ) {
		return states[n];
	}
	return states[0];
}

const HibernatorBase::StateLookup&
HibernatorBase::Lookup ( SLEEP_STATE state )
{
	for( int i = 0;  states[i].number >= 0;  i++ ) {
		if ( states[i].state == state ) {
			return states[i];
		}
	}
	return states[0];
}

const HibernatorBase::StateLookup&
HibernatorBase::Lookup ( const char *name )
{
	for( int i = 0;  states[i].number >= 0;  i++ ) {
		const HibernatorBase::StateLookup	&state = states[i];

		for( int j = 0;  state.strings[j];  j++ ) {
			if ( strcasecmp(state.strings[j], name ) == 0 ) {
				return state;
			}
		}
	}
	return states[0];
}
