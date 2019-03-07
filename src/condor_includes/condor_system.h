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

#ifndef CONDOR_SYSTEM_H
#define CONDOR_SYSTEM_H


/******************************
** Windows specifics
******************************/
#if defined(WIN32)
# include "condor_sys_nt.h"
#endif

/******************************
** Unix specifics
******************************/
#if !defined( WIN32 )

/* Things we want defined on all Unix systems */

#ifndef _POSIX_SOURCE
# define _POSIX_SOURCE
#endif

#ifndef UNIX
# define UNIX
#endif


/**********************************************************************
** These system-specific files will "fix" anything that needs fixing,
** define platform-specific functionality flags we want to use, etc. 
**********************************************************************/	

#if defined(LINUX)
#	include "condor_sys_linux.h"
#elif defined(HPUX)
#	include "condor_sys_hpux.h"
#elif defined(Solaris)
#	include "condor_sys_solaris.h"
#elif defined(Darwin)
#	include "condor_sys_bsd.h"
#elif defined(AIX)
#	include "condor_sys_aix.h"
#elif defined(CONDOR_FREEBSD)
#	include "condor_sys_bsd.h"
#else
#   error "condor_system.h: Don't know what Unix this is!"
#endif


/********************************************************************
** Declare base system types (currently the int64_t types, but will
** probably grow beyond this charter).
*********************************************************************/
#include "condor_sys_types.h"


/**********************************************************************
** Files that need to be fixed on all (or nearly all) platforms 
**********************************************************************/
#include "condor_fix_access.h"
#include "condor_fix_assert.h"

#if defined(WIN32)
#include <iomanip.h>
#endif


/**********************************************************************
** Clean-up, default definitions, etc.
**********************************************************************/

#if !defined(HAS_U_TYPES)
    typedef unsigned int	u_int;
    typedef unsigned char	u_char;
    typedef unsigned short	u_short;
    typedef unsigned long	u_long;
#endif

#if !defined(NO_VOID_SIGNAL_RETURN)
#	define VOID_SIGNAL_RETURN	1
#endif

#if !defined(SIGISMEMBER_IS_BROKEN)
#	define SIGISMEMBER_IS_BROKEN 0
#endif

#if !defined(HAS_F_DUP2FD)
#	define HAS_F_DUP2FD 0
#endif

#if !defined(NBBY)
#	define NBBY 8
#endif

#if !defined(NFDS)
#	define NFDS(x) (x)
#endif


/* If WCOREFLAG is defined but WCOREFLG is not, define WCOREFLG since
   that's what we use in our code. */
#if defined(WCOREFLAG) && !defined(WCOREFLG)
#	define WCOREFLG WCOREFLAG
#endif

#ifndef _POSIX_PATH_MAX
#	define _POSIX_PATH_MAX 255
#endif

#ifndef _POSIX_ARG_MAX
#	define _POSIX_ARG_MAX 4096
#endif


/**********************************************************************
** We'll also include everything we use after this so we make sure
** we've got what we need on all platforms.  Headers that have already
** been fixed will simply be ignored since they define their own flags
** to prevent multiple inclusions. 
**********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <limits.h>
#include <ctype.h>
#include <fcntl.h>	
#include <errno.h>
#include <pwd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <float.h>
#include <math.h>
#include <utime.h>

/* select() on all our platforms takes an fd_set pointer, so we can
   just define this here for everyone.  We don't really need it
   anymore, but we might hit a platform that has a different select,
   so the level of indirection is worth keeping around. */
typedef fd_set *SELECT_FDSET_PTR;

#include "condor_unsetenv.h"

#endif /* UNIX */


/* This stuff here is shared by all archs, *INCLUDING* NiceTry */


#include "condor_snutils.h"


/* Pull in some required types */
#if defined( HAVE_SYS_TYPES_H )
#  include <sys/types.h>
#endif
#if defined( HAVE_STDINT_H )
#  include <stdint.h>
#endif

/* Define _O_BINARY, etc. if they're not defined as zero */
#ifndef _O_BINARY
#  define _O_BINARY		0x0
#endif
#ifndef _O_SEQUENTIAL
#  define _O_SEQUENTIAL	0x0
#endif
#ifndef O_LARGEFILE
#  define O_LARGEFILE	0x0
#endif
#ifndef _O_NOINHERIT
#  define _O_NOINHERIT	0x0 /* child process doesn't inherit file */
#endif

#include "condor_sys_formats.h"

#ifndef WIN32
#	include <dirent.h>

/* Some convenient definitions to make it easier to use readdir64()
 * when available.  This is important so that 32-bit builds of condor
 * can see directory entries that have 64-bit inode values.  Using the
 * 32-bit readdir() has caused procapi to not be able to see entries
 * in /proc, which can lead to improper cleanup, procd exiting because
 * it thinks its parent is gone, and general badness.
 */

  #if defined(AIX) && HAVE_READDIR64

    typedef DIR64 condor_DIR;
    #define condor_opendir opendir64
    #define condor_closedir closedir64
    #define condor_rewinddir rewinddir64

  #else

    typedef DIR condor_DIR;
    #define condor_opendir opendir
    #define condor_closedir closedir
    #define condor_rewinddir rewinddir

  #endif

  #if HAVE_READDIR64
    typedef struct dirent64 condor_dirent;
    #define condor_readdir readdir64
  #else
    typedef struct dirent condor_dirent;
    #define condor_readdir readdir
  #endif

#endif // !WIN32

/* Declaration for Sleep(int milliseconds) in our util lib.  Note
 * we don't do this on Win32, since Sleep() is native on Win32.
 */
#ifndef WIN32
	unsigned int Sleep(unsigned int milliseconds);
#endif

/* This defines macros that can disable certain gcc warnings */
/* If not using gcc, macros are null defines */
#include "gcc_diag.h"

#endif /* CONDOR_SYSTEM_H */
