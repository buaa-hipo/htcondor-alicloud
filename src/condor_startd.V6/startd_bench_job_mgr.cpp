/***************************************************************
 *
 * Copyright (C) 1990-2010, Condor Team, Computer Sciences Department,
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
#include "startd.h"
#include "startd_cron_job_mgr.h"
#include "startd_bench_job_mgr.h"
#include "startd_bench_job_params.h"
#include "startd_bench_job.h"


/*
 * StartdBenchJobMgrParams class methods
 */
StartdBenchJobMgrParams::StartdBenchJobMgrParams(
	const char			&base,
	const CronJobMgr	&mgr )
		: CronJobMgrParams( base ),
		  m_mgr( static_cast<const StartdBenchJobMgr &>(mgr) )
{
}

char *
StartdBenchJobMgrParams::GetDefault( const char *item ) const
{
	if ( !strcasecmp( item, "JOBLIST" ) ) {
		return strdup( "MIPS KFLOPS" );
	}
	return NULL;
}

void
StartdBenchJobMgrParams::GetDefault( const char *item,
									 double &dv ) const
{
	if ( strcasecmp( item, "MAX_JOB_LOAD" ) ) {
		dv = 1.0;
	}
}


/*
 * StartdBenchJobMgr class methods
 */

// Basic constructor
StartdBenchJobMgr::StartdBenchJobMgr( void )
		: CronJobMgr( ),
		  m_shutting_down( false ),
		  m_is_running( false ),
		  m_rip( NULL )
{
}

// Basic destructor
StartdBenchJobMgr::~StartdBenchJobMgr( void )
{
	dprintf( D_FULLDEBUG, "StartdBenchJobMgr: Bye\n" );
}

int
StartdBenchJobMgr::Initialize( const char *name )
{
	SetName( name, name );
	return CronJobMgr::Initialize( name );
}

bool
StartdBenchJobMgr::StartBenchmarks( Resource *rip, int &count )
{
	ASSERT( rip );
	if ( GetNumActiveJobs() ) {
		count = 0;
		return true;
	}
	m_rip = rip;
	count = GetNumJobs( );
	dprintf( D_ALWAYS, "BenchMgr:StartBenchmarks()\n" );
	return StartOnDemandJobs( ) == 0;
}

int
StartdBenchJobMgr::Reconfig( void )
{
	StringList	before, after;

	m_job_list.GetStringList( before );
	int status = CronJobMgr::HandleReconfig();
	m_job_list.GetStringList( after );
	if ( status ) {
		return status;
	}
	if ( ! before.identical(after) ) {
		char	*before_str = before.print_to_string();
		char	*after_str  = after.print_to_string();
		dprintf( D_ALWAYS,
				 "WARNING: benchmark job list changed from '%s' to '%s'"
				 " -- If there are additions to the benchmark list, these"
				 " new benchmarks won't be run until the 'RunBenchmarks'"
				 " expresion becomes true and all benchmarks are run.\n",
				 before_str?before_str:"", after_str?after_str:"" );
		free( before_str );
		free( after_str );
	}
	return 1;
}

// Perform shutdown
int
StartdBenchJobMgr::Shutdown( bool force )
{
	dprintf( D_FULLDEBUG, "StartdBenchJobMgr: Shutting down\n" );
	return KillAll( force );
}

// Check shutdown
bool
StartdBenchJobMgr::ShutdownOk( void )
{
	bool	idle = IsAllIdle( );

	// dprintf( D_ALWAYS, "ShutdownOk: %s\n", idle ? "Idle" : "Busy" );
	return idle;
}


StartdBenchJobMgrParams *
StartdBenchJobMgr::CreateMgrParams( const char &base )
{
	return new StartdBenchJobMgrParams( base, *this );
}

StartdBenchJobParams *
StartdBenchJobMgr::CreateJobParams( const char *job_name )
{
	return new StartdBenchJobParams( job_name, *this );
}

StartdBenchJob *
StartdBenchJobMgr::CreateJob( CronJobParams *job_params )
{
	dprintf( D_FULLDEBUG,
			 "*** Creating Benchmark job '%s'***\n",
			 job_params->GetName() );
	StartdBenchJobParams *params =
		dynamic_cast<StartdBenchJobParams *>( job_params );
	ASSERT( params );
	return new StartdBenchJob( params, *this );
}

// Should we start a job?
bool
StartdBenchJobMgr::ShouldStartJob( const CronJob &job ) const
{
	if ( m_shutting_down ) {
		dprintf( D_ALWAYS, "ShouldStartJob: shutting down\n" );
		return false;
	}

	// If we're busy running cron jobs, wait
	if (  ( NULL != cron_job_mgr ) &&
		  ( !cron_job_mgr->ShouldStartBenchmarks() )  ) {
		dprintf( D_ALWAYS, "ShouldStartJob: running normal cron jobs\n" );
		return false;
	}

	// Finally, ask my parent for permission
	return CronJobMgr::ShouldStartJob( job );
}

// Job is started
bool
StartdBenchJobMgr::JobStarted( const CronJob &job )
{
	if ( ! m_is_running ) {
		m_rip->benchmarks_started( );
	}
	m_is_running = true;
	return CronJobMgr::JobStarted( job );
}

// Job exitted
bool
StartdBenchJobMgr::JobExited( const CronJob &job )
{
	bool status = CronJobMgr::JobExited( job );
	if ( 0 == GetNumActiveJobs() ) {
		BenchmarksFinished( );
	}
	return status;
}

bool
StartdBenchJobMgr::BenchmarksFinished( void )
{
	m_rip->benchmarks_finished( );
	resmgr->update_all();
	if ( m_shutting_down ) {
		startd_check_free();
	}
	else if ( NULL != cron_job_mgr ) {
		cron_job_mgr->ScheduleAllJobs();
	}
	return true;
}
