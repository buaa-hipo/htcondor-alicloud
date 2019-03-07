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


/* prototype in condor_includes/get_port_range.h */

#include "condor_common.h"
#include "condor_debug.h"
#include "condor_config.h"
#include "get_port_range.h"

int get_port_range(int is_outgoing, int *low_port, int *high_port)
{
	int  t_low = 0, t_high = 0;

	// (is_outgoing != 0) means client mode, outgoing connection
	// (is_outgoing == 0) means server mode, incoming connnection

	if (is_outgoing) {
		// first check for the existence of OUT_LOWPORT and OUT_HIGHPORT
		if (param_integer("OUT_LOWPORT", t_low, false, 0)) {
			if (param_integer("OUT_HIGHPORT", t_high, false, 0)) {
				dprintf (D_NETWORK, "get_port_range - (OUT_LOWPORT,OUT_HIGHPORT) is (%i,%i).\n", t_low, t_high);
			} else {
				dprintf (D_ALWAYS, "get_port_range - ERROR: OUT_LOWPORT defined but no OUT_HIGHPORT.\n");
				return FALSE;
			}
		}
	} else {
		// first check for the existence of IN_LOWPORT and IN_HIGHPORT
		if (param_integer("IN_LOWPORT", t_low, false, 0)) {
			if (param_integer("IN_HIGHPORT", t_high, false, 0)) {
				dprintf (D_NETWORK, "get_port_range - (IN_LOWPORT,IN_HIGHPORT) is (%i,%i).\n", t_low, t_high);
			} else {
				dprintf (D_ALWAYS, "get_port_range - ERROR: IN_LOWPORT defined but no IN_HIGHPORT.\n");
				return FALSE;
			}
		}
	}

	// check if the above settings existed
	if ((t_low == 0) && (t_high == 0)) {
#if defined( WIN32 )
		// See ticket #4711.  If a socket isn't bound to a specific interface,
		// (and it won't be, with BIND_ALL_INTERFACES defaulting to true)
		// Windows will delay port-in-use errors from calls to bind() until
		// the connect() call forces it to decide which interface to use.
		// This gives our networking logic fits.  The simplest work-around
		// is to stop trying to bind to a specific port for outbound
		// connections unless the administrator /really/ wants it -- that is,
		// has set OUT_{LOW,HIGH}PORT.
		if( is_outgoing ) {
			dprintf( D_NETWORK, "get_port_range - not checking LOWPORT, HIGHPORT for outgoing connection on Windows.\n" );
			return FALSE;
		}
#endif
		// fallback to the old LOWPORT and HIGHPORT
		if (param_integer("LOWPORT", t_low, false, 0)) {
			if (param_integer("HIGHPORT", t_high, false, 0)) {
				dprintf (D_NETWORK, "get_port_range - (LOWPORT,HIGHPORT) is (%i,%i).\n", t_low, t_high);
			} else {
				dprintf (D_ALWAYS, "get_port_range - ERROR: LOWPORT defined but no HIGHPORT.\n");
				return FALSE;
			}
		}
	}

	// modify the command line arguments now, since that's how it was and i
	// want to avoid breaking any old code i haven't seen that relies on this
	// side effect (though i haven't seen any) -ZKM
	*low_port = t_low;
	*high_port = t_high;

	// check bounds
    if((*low_port < 0) || (*high_port < 0) || (*low_port > *high_port)) {
        dprintf(D_ALWAYS, "get_port_range - ERROR: invalid port range (%d,%d)\n ",
                           *low_port, *high_port);
        return FALSE;
    }

// windows doesn't have the concept of privileged ports
// on unix, we do print a warning if the range crosses 1024, but it
// is not a fatal error.
#ifndef WIN32
	// check privilege mix (non-fatal)
	if(*low_port < 1024 && *high_port >= 1024) {
		dprintf(D_ALWAYS, "get_port_range - WARNING: port range (%d,%d) "
				"is mix of privileged and non-privileged ports!\n",
				*low_port, *high_port );
	}
#endif

	// return TRUE if we have a valid LOWPORT and HIGHPORT.  if both are zero
	// it means bind to any port, i.e. no port range and we therefore return
	// FALSE.
	if (*low_port || *high_port) {
    	return TRUE;
	} else {
		return FALSE;
	}

}


int
_condor_bind_all_interfaces( void )
{
	return param_boolean_crufty("BIND_ALL_INTERFACES", true) ? TRUE : FALSE;
}




