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

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "condor_debug.h"
#include "string_list.h"
#include "HashTable.h"
#include "PipeBuffer.h"
#include "my_getopt.h"
#include "io_loop_pthread.h"
#include "alicloudgahp_common.h"
#include "alicloudCommands.h"
#include "subsystem_info.h"

// Globals from alicloudCommand.cpp's statistitcs.
extern int NumRequests;
extern int NumDistinctRequests;
extern int NumRequestsExceedingLimit;
extern int NumExpiredSignatures;

#define MIN_WORKER_NUM 1
#define ALICLOUD_GAHP_VERSION	"1.0"

int RESULT_OUTBOX = 1;	// stdout
int REQUEST_INBOX = 0; // stdin

const char * version = "$GahpVersion " ALICLOUD_GAHP_VERSION " Feb 15 2008 Condor\\ ALICLOUDGAHP $";

static IOProcess *ioprocess = NULL;

// forwarding declaration
static void gahp_output_return_error();
static void gahp_output_return_success();
static void gahp_output_return (const char ** results, const int count);
static int verify_gahp_command(char ** argv, int argc);
static void *worker_function( void *ptr );

/* We use a big mutex to make certain only one thread is running at a time,
 * except when that thread would be blocked on I/O or a signal.  We do
 * this becase many data structures and methods used from utility libraries
 * are not inheriently thread safe.  So we error on the side of correctness,
 * and this isn't a big deal since we are only really concerned with the gahp
 * blocking when we do network communication and SOAP/WS_SECURITY processing.
 */
pthread_mutex_t global_big_mutex = PTHREAD_MUTEX_INITIALIZER;
#include "thread_control.h"

static void io_process_exit(int exit_num)
{
	exit( exit_num );
}

void
usage()
{
	dprintf( D_ALWAYS, "Usage: alicloud_gahp -d debuglevel -w min_worker_nums -m max_worker_nums\n");
	exit(1);
}

static bool
registerAllAlicloudCommands(void)
{
	if( numofAlicloudCommands() > 0 ) {
		dprintf(D_ALWAYS, "There are already registered commands\n");
		return false;
	}

	// EC2 Commands
	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_START,
			AlicloudVMStart::ioCheck, AlicloudVMStart::workerFunction);

    registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_START_SPOT,
            AlicloudVMStartSpot::ioCheck, AlicloudVMStartSpot::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STOP,
			AlicloudVMStop::ioCheck, AlicloudVMStop::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STOP_SPOT,
			AlicloudVMStopSpot::ioCheck, AlicloudVMStopSpot::workerFunction);

    registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STATUS_SPOT,
            AlicloudVMStatusSpot::ioCheck, AlicloudVMStatusSpot::workerFunction);

    registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STATUS_ALL_SPOT,
            AlicloudVMStatusAllSpot::ioCheck, AlicloudVMStatusAllSpot::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STATUS,
			AlicloudVMStatus::ioCheck, AlicloudVMStatus::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_STATUS_ALL,
			AlicloudVMStatusAll::ioCheck, AlicloudVMStatusAll::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_CREATE_KEYPAIR,
			AlicloudVMCreateKeypair::ioCheck, AlicloudVMCreateKeypair::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_DESTROY_KEYPAIR,
			AlicloudVMDestroyKeypair::ioCheck, AlicloudVMDestroyKeypair::workerFunction);

    registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_ASSOCIATE_ADDRESS,
            AlicloudAssociateAddress::ioCheck, AlicloudAssociateAddress::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_ATTACH_VOLUME,
            AlicloudAttachVolume::ioCheck, AlicloudAttachVolume::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_CREATE_TAGS,
            AlicloudCreateTags::ioCheck, AlicloudCreateTags::workerFunction);

	registerAlicloudGahpCommand(ALICLOUD_COMMAND_VM_SERVER_TYPE,
            AlicloudVMServerType::ioCheck, AlicloudVMServerType::workerFunction);

	// Annex commands
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_BULK_START,
			AlicloudBulkStart::ioCheck, AlicloudBulkStart::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_PUT_RULE,
			AlicloudPutRule::ioCheck, AlicloudPutRule::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_PUT_TARGETS,
			AlicloudPutTargets::ioCheck, AlicloudPutTargets::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_BULK_STOP,
			AlicloudBulkStop::ioCheck, AlicloudBulkStop::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_DELETE_RULE,
			AlicloudDeleteRule::ioCheck, AlicloudDeleteRule::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_REMOVE_TARGETS,
			AlicloudRemoveTargets::ioCheck, AlicloudRemoveTargets::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_GET_FUNCTION,
			AlicloudGetFunction::ioCheck, AlicloudGetFunction::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_S3_UPLOAD,
			AlicloudS3Upload::ioCheck, AlicloudS3Upload::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_CF_CREATE_STACK,
			AlicloudCreateStack::ioCheck, AlicloudCreateStack::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_CF_DESCRIBE_STACKS,
			AlicloudDescribeStacks::ioCheck, AlicloudDescribeStacks::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_CALL_FUNCTION,
			AlicloudCallFunction::ioCheck, AlicloudCallFunction::workerFunction );
	registerAlicloudGahpCommand( ALICLOUD_COMMAND_BULK_QUERY,
			AlicloudBulkQuery::ioCheck, AlicloudBulkQuery::workerFunction );

	return true;
}

void
quit_on_signal(int sig)
{
	/* Posix says exit() is not signal safe, so call _exit() */
	_exit(sig);
}

int
main( int argc, char ** const argv )
{
#ifndef WIN32
	/* Add the signals we want unblocked into sigSet */
	sigset_t sigSet;
	struct sigaction act;

	sigemptyset( &sigSet );
	sigaddset(&sigSet,SIGTERM);
	sigaddset(&sigSet,SIGQUIT);
	sigaddset(&sigSet,SIGPIPE);

	/* Set signal handlers */
	sigemptyset(&act.sa_mask);  /* do not block anything in handler */
	act.sa_flags = 0;

	/* Signals which should cause us to exit with status = signum */
	act.sa_handler = quit_on_signal;
	sigaction(SIGTERM,&act,0);
	sigaction(SIGQUIT,&act,0);
	sigaction(SIGPIPE,&act,0);

	/* Unblock signals in our set */
	sigprocmask( SIG_UNBLOCK, &sigSet, NULL );
#endif

	int min_workers = MIN_NUMBER_WORKERS;
	int max_workers = -1;
	const char * subSystemName = "EC2_GAHP";
	const char * logDirectory = NULL;

	int c = 0;
	while ( (c = my_getopt(argc, argv, "l:s:f:d:w:m:" )) != -1 ) {
		switch(c) {
			case 'l':
				if( my_optarg && *my_optarg ) {
					logDirectory = my_optarg;
				}
				break;
			case 's':
				if( my_optarg && *my_optarg ) {
					subSystemName = my_optarg;
				}
				break;
			case 'f':
				break;
			case 'd':
				// Debug Level
				if( my_optarg && *my_optarg ) {
					set_debug_flags(my_optarg, 0);
				}
				break;
			case 'w':
				// Minimum number of worker pools
				min_workers = atoi(my_optarg);
				if( min_workers < MIN_NUMBER_WORKERS ) {
					min_workers = MIN_NUMBER_WORKERS;
				}
				break;
			case 'm':
				// Maximum number of worker pools
				max_workers = atoi(my_optarg);
				if( max_workers <= 0 ) {
					max_workers = -1;
				}
				break;
			default:
				usage();
		}
	}

	// This is horrible, but I can't find the "right" way to do it.
    if( logDirectory != NULL ) {
    	setenv( "_CONDOR_LOG", logDirectory, 1 );
    }

    set_mySubSystem( subSystemName, SUBSYSTEM_TYPE_GAHP );
    config();
    dprintf_config( subSystemName );
    const char * debug_string = getenv( "DebugLevel" );
    if( debug_string && * debug_string ) {
        set_debug_flags( debug_string, 0 );
    }

	dprintf(D_FULLDEBUG, "Welcome to the EC2 GAHP\n");

	const char *buff;

	//Try to read env for alicloud_http_proxy
	buff = getenv(ALICLOUD_HTTP_PROXY);
	if( buff && *buff ) {
		set_alicloud_proxy_server(buff);
		dprintf(D_ALWAYS, "Using http proxy = %s\n", buff);
	}

	// Register all alicloud commands
	if( registerAllAlicloudCommands() == false ) {
		dprintf(D_ALWAYS, "Can't register Alicloud Commands\n");
		exit(1);
	}

	// Create IOProcess class
	ioprocess = new IOProcess;
	ASSERT(ioprocess);

	if( ioprocess->startUp(REQUEST_INBOX, min_workers, max_workers) == false ) {
		dprintf(D_ALWAYS, "Failed to start IO Process\n");
		delete ioprocess;
		exit(1);
	}

	// Print out the GAHP version to the screen
	// now we're ready to roll
	printf ("%s\n", version);
	fflush(stdout);

	dprintf( D_ALWAYS, "EC2 GAHP initialized\n" );

		/* Our main thread should grab the mutex first.  We will then
		 * release it and let other threads run when we would otherwise
		 * block.
		 */
	alicloud_gahp_grab_big_mutex();

	for(;;) {
		ioprocess->stdinPipeHandler();
	}

	return 0;
}

// Check the parameters to make sure the command
// is syntactically correct
static int
verify_gahp_command(char ** argv, int argc) {
	// Special Commands First
	if (strcasecmp (argv[0], GAHP_COMMAND_RESULTS) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_STATISTICS) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_VERSION) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_COMMANDS) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_QUIT) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_ASYNC_MODE_ON) == 0 ||
			strcasecmp (argv[0], GAHP_COMMAND_ASYNC_MODE_OFF) == 0) {
		// These are no-arg commands
		return verify_number_args (argc, 1);
	}

	return executeIOCheckFunc(argv[0], argv, argc);
}

void
gahp_output_return (const char ** results, const int count) {
	int i=0;
	for (i=0; i<count; i++) {
		printf ("%s", results[i]);
		if (i < (count - 1 )) {
			printf (" ");
		}
	}

	printf ("\n");
	fflush(stdout);
}

static void
gahp_output_return_success() {
	const char* result[] = {GAHP_RESULT_SUCCESS};
	gahp_output_return (result, 1);
}

static void
gahp_output_return_error() {
	const char* result[] = {GAHP_RESULT_ERROR};
	gahp_output_return (result, 1);
}

Worker::Worker(int worker_id)
{
	m_id = worker_id;
	m_can_use = false;
	m_is_doing = false;
	m_is_waiting = false;
	m_must_be_alive = false;

	pthread_cond_init(&m_cond, NULL);
}

Worker::~Worker()
{
	Request *request = NULL;

	m_request_list.Rewind();
	while( m_request_list.Next(request) ) {
		m_request_list.DeleteCurrent();
		delete request;
	}

	pthread_cond_destroy(&m_cond);
}

bool
Worker::removeRequest(int req_id)
{
	Request *request = NULL;

	m_request_list.Rewind();
	while( m_request_list.Next(request) ) {

		if( request->m_reqid == req_id ) {
			// remove this request from worker request queue
			m_request_list.DeleteCurrent();
			delete request;
			return true;
		}
	}

	return false;
}


// Functions for IOProcess class
IOProcess::IOProcess()
	: m_workers_list(20, &hashFuncInt)
{
	m_async_mode = false;
	m_new_results_signaled = false;

	m_min_workers = MIN_NUMBER_WORKERS;
	m_max_workers = -1;

	m_next_worker_id = 0;
	m_rotated_worker_ids = false;

	m_avail_workers_num = 0;
}

IOProcess::~IOProcess()
{
}

bool
IOProcess::startUp(int stdin_pipe, int min_workers, int max_workers)
{
	m_min_workers = min_workers;
	m_max_workers = max_workers;
	m_stdin_buffer.setPipeEnd(stdin_pipe);

	// Create initial worker processes
	int i = 0;
	for( i = 0; i < m_min_workers; i++ ) {
		if( createNewWorker() == NULL ) {
			dprintf(D_ALWAYS, "Failed to create initial workers\n");
			dprintf(D_ALWAYS, "Exiting....\n");
			io_process_exit(1);
		}
	}

	return true;
}

int
IOProcess::stdinPipeHandler()
{
	char command[131072];
	while( m_stdin_buffer.GetNextLine( command, sizeof( command ) ) ) {
		dprintf (D_FULLDEBUG, "got stdin: '%s'\n", command);

		Gahp_Args args;

		if (parse_gahp_command (command, &args) &&
			verify_gahp_command (args.argv, args.argc)) {

				// Catch "special commands first
			if (strcasecmp (args.argv[0], GAHP_COMMAND_RESULTS) == 0) {
				// Print each result line

				// Print number of results
				printf("%s %d\n", GAHP_RESULT_SUCCESS, numOfResult());
				fflush(stdout);

				startResultIteration();

				char* next = NULL;
				while ((next = NextResult()) != NULL) {
					printf ("%s", next);
					fflush(stdout);
					deleteCurrentResult();
				}
				m_new_results_signaled = false;
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_VERSION) == 0) {
				printf ("S %s\n", version);
				fflush (stdout);
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_QUIT) == 0) {
				gahp_output_return_success();
				io_process_exit(0);
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_ASYNC_MODE_ON) == 0) {
				m_async_mode = true;
				m_new_results_signaled = false;
				gahp_output_return_success();
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_ASYNC_MODE_OFF) == 0) {
				m_async_mode = false;
				gahp_output_return_success();
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_STATISTICS) == 0) {
				printf( "%s %d %d %d %d\n", GAHP_RESULT_SUCCESS,
					NumRequests, NumDistinctRequests,
					NumRequestsExceedingLimit, NumExpiredSignatures
				);
				fflush( stdout );
			} else if (strcasecmp (args.argv[0], GAHP_COMMAND_COMMANDS) == 0) {
				StringList alicloud_commands;
				int num_commands = 0;

				num_commands = allAlicloudCommands(alicloud_commands);
				num_commands += 7;

				const char *commands[num_commands];
				int i = 0;
				commands[i++] = GAHP_RESULT_SUCCESS;
				commands[i++] = GAHP_COMMAND_ASYNC_MODE_ON;
				commands[i++] = GAHP_COMMAND_ASYNC_MODE_OFF;
				commands[i++] = GAHP_COMMAND_RESULTS;
				commands[i++] = GAHP_COMMAND_QUIT;
				commands[i++] = GAHP_COMMAND_VERSION;
				commands[i++] = GAHP_COMMAND_COMMANDS;

				alicloud_commands.rewind();
				char *one_alicloud_command = NULL;

				while( (one_alicloud_command = alicloud_commands.next() ) != NULL ) {
					commands[i++] = one_alicloud_command;
				}

				gahp_output_return (commands, i);
			} else {
				// got new command
				if( !addNewRequest(command) ) {
					gahp_output_return_error();
				} else {
					gahp_output_return_success();
				}
			}
		} else {
			gahp_output_return_error();
		}
	}

	// check if GetNextLine() returned NULL because of an error or EOF
	if (m_stdin_buffer.IsError() || m_stdin_buffer.IsEOF()) {
		dprintf (D_ALWAYS, "stdin buffer closed, exiting\n");
		io_process_exit(1);
	}

	return TRUE;
}

Worker*
IOProcess::createNewWorker(void)
{
	Worker *new_worker = NULL;
	new_worker = new Worker(newWorkerId());

	dprintf (D_FULLDEBUG, "About to start a new thread\n");

	// Set this worker is available
	new_worker->m_can_use = true;

	// Create pthread
	pthread_t thread;
	if( pthread_create(&thread, NULL,
				worker_function, (void *)new_worker) !=  0 ) {
		dprintf(D_ALWAYS, "Failed to create a new thread\n");

		delete new_worker;
		return NULL;
	}

	// Deatch this thread
	pthread_detach(thread);

	// Insert a new worker to HashTable
	m_workers_list.insert(new_worker->m_id, new_worker);
	m_avail_workers_num++;

	dprintf(D_FULLDEBUG, "New Worker[id=%d] is created!\n", new_worker->m_id);
	return new_worker;
}

Worker*
IOProcess::findFreeWorker(void)
{
	int currentkey = 0;
	Worker *worker = NULL;

	m_workers_list.startIterations();
	while( m_workers_list.iterate(currentkey, worker) != 0 ) {
		if( !worker->m_is_doing && worker->m_can_use ) {
			worker->m_must_be_alive = true;
			return worker;
		}
	}
	return NULL;
}

Worker*
IOProcess::findFirstAvailWorker(void)
{
	int currentkey = 0;
	Worker *worker = NULL;

	m_workers_list.startIterations();
	while( m_workers_list.iterate(currentkey, worker) != 0 ) {

		if( worker->m_can_use ) {
			worker->m_must_be_alive = true;
			return worker;
		}
	}

	return NULL;
}


Worker*
IOProcess::findWorker(int id)
{
	Worker *worker = NULL;

	m_workers_list.lookup(id, worker);
	return worker;
}

bool
IOProcess::removeWorkerFromWorkerList(int id)
{
	Worker* worker = findWorker(id);
	if( worker ) {
		m_workers_list.remove(id);

		delete worker;
		return true;
	}

	return false;
}

void
IOProcess::LockWorkerList(void)
{
}

void
IOProcess::UnlockWorkerList(void)
{
}

Request*
IOProcess::addNewRequest(const char *cmd)
{
	if( !cmd ) {
		return NULL;
	}

	Request* new_req = new Request(cmd);
	ASSERT(new_req);
	dprintf( D_PERF_TRACE, "request #%d (%s): dispatch\n",
		new_req->m_reqid, new_req->m_args.argv[0] );

	// check if there is available worker process
	Worker *worker = findFreeWorker();

	if( !worker ) {
		// There is no available worker

		if( m_max_workers == -1 || m_avail_workers_num < m_max_workers ) {
			worker = createNewWorker();
		}
	}

	addRequestToWorker(new_req, worker);
	return new_req;
}

void
IOProcess::addResult(const char *result)
{
	if( !result ) {
		return;
	}

	// Put this result into result buffer
	m_result_list.append(result);

	if (m_async_mode) {
		if (!m_new_results_signaled) {
			printf ("R\n");
			fflush (stdout);
		}
		m_new_results_signaled = true;	// So that we only do it once
	}
}

int
IOProcess::newWorkerId(void)
{
	Worker* unused = NULL;
	int starting_worker_id = m_next_worker_id++;

	while( starting_worker_id != m_next_worker_id ) {
		if( m_next_worker_id > 990000000 ) {
			m_next_worker_id = 1;
			m_rotated_worker_ids = true;
		}

		if( !m_rotated_worker_ids ) {
			return m_next_worker_id;
		}

		// Make certain this worker_id is not already in use
		if( m_workers_list.lookup(m_next_worker_id, unused) == -1 ) {
			// not in use, we are done
			return m_next_worker_id;
		}

		m_next_worker_id++;
	}

	// If we reached here, we are out of worker ids
	dprintf(D_ALWAYS, "Gahp Server Error - out of worker ids !!!?!?!?\n");
	return -1;
}

void
IOProcess::addRequestToWorker(Request* request, Worker* worker)
{
	if( !request ) {
		return;
	}

	if( worker ) {
		dprintf (D_FULLDEBUG, "Sending %s to worker %d\n",
				request->m_raw_cmd.c_str(), worker->m_id);

		request->m_worker = worker;
		worker->m_request_list.Append(request);
		worker->m_is_doing = true;

		if( worker->m_is_waiting ) {
			pthread_cond_signal(&worker->m_cond);
		}
	} else {
		// There is no available worker.
		// So we will insert this request to global pending request list
		dprintf (D_FULLDEBUG, "Appending %s to global pending request list\n",
				request->m_raw_cmd.c_str());

		m_pending_req_list.Append(request);
	}
}

int
IOProcess::numOfPendingRequest(void)
{
	int num = 0;
	num = m_pending_req_list.Number();
	return num;
}

Request*
IOProcess::popPendingRequest(void)
{
	Request *new_request = NULL;

	m_pending_req_list.Rewind();
	m_pending_req_list.Next(new_request);
	if( new_request ) {
		m_pending_req_list.DeleteCurrent();
	}

	return new_request;
}

Request* popRequest(Worker* worker)
{
	Request *new_request = NULL;
	if( !worker ) {
		return NULL;
	}

	worker->m_request_list.Rewind();
	worker->m_request_list.Next(new_request);

	if( new_request ) {
		// Remove this request from worker request queue
		worker->m_request_list.DeleteCurrent();
	} else {
		if( ioprocess ) {
			new_request = ioprocess->popPendingRequest();

			if( new_request ) {
				new_request->m_worker = worker;

				dprintf (D_FULLDEBUG, "Assigning %s to worker %d\n",
						new_request->m_raw_cmd.c_str(), worker->m_id);
			}
		}
	}

	return new_request;
}

static void
enqueue_result(Request* request)
{
	if( !request || request->m_result.empty() || !ioprocess ) {
		return;
	}

	ioprocess->addResult(request->m_result.c_str());
}

static bool
handle_gahp_command(Request* request)
{
	if( !request ) {
		return false;
	}
	char ** argv = request->m_args.argv;
	int argc = request->m_args.argc;

	// Assume it's been verified
	if( argc < 2 ) {
		dprintf (D_ALWAYS, "Invalid request\n");
		return false;
	}

	if( !verify_request_id(argv[1]) ) {
		dprintf (D_ALWAYS, "Invalid request ID\n");
		return false;
	}

	bool result = executeWorkerFunc(argv[0], argv, argc, request->m_result);

	if( request->m_result.empty() == false ) {
		enqueue_result(request);
	}

	return result;
}

static void worker_exit(Worker *worker, bool force)
{
	if( !worker ) {
		return;
	}

	int worker_id = worker->m_id;

	bool need_remove = force;
	if( ioprocess ) {
		ioprocess->LockWorkerList();

		if( need_remove == false ) {
			if( ioprocess->m_avail_workers_num > ioprocess->m_min_workers ) {
				need_remove = true;
			}
		}

		if( need_remove ) {
			// worker will be deleted inside removeWorkerFromWorkerList
			ioprocess->removeWorkerFromWorkerList(worker->m_id);
			worker = NULL;
			ioprocess->m_avail_workers_num--;

		}
		ioprocess->UnlockWorkerList();
	}

	if( need_remove ) {
		dprintf(D_FULLDEBUG, "Thread(%d) is exiting...\n", worker_id);

		int retval = 0;
		alicloud_gahp_release_big_mutex();
		pthread_exit(&retval);
	} else {
		// We need to keep this thread running
		worker->m_can_use = true;

		worker->m_is_doing = false;
		worker->m_is_waiting = false;
		worker->m_must_be_alive = false;
	}
}

static void *worker_function( void *ptr )
{
	Worker *worker = (Worker *)ptr;

		/* Our new thread should grab the big mutex before starting.
		 * We will then
		 * release it and let other threads run when we would otherwise
		 * block.
		 */
	alicloud_gahp_grab_big_mutex();

	if( !worker ) {
		dprintf (D_ALWAYS, "Ooops!! No input Data in worker thread\n");
		alicloud_gahp_release_big_mutex();
		return NULL;
	}

	// Pop Request
	Request *new_request = NULL;
	struct timespec ts;
	struct timeval tp;

	while(1) {

		worker->m_is_doing = false;
		worker->m_is_waiting = false;

		if( worker->m_can_use == false ) {
			// Need to die
			worker_exit(worker, true);
		}

		while( (new_request = popRequest(worker)) == NULL ) {

			worker->m_is_waiting = true;

			// Get Current Time
			gettimeofday(&tp, NULL);

			/* Convert from timeval to timespec */
			ts.tv_sec = tp.tv_sec;
			ts.tv_nsec = tp.tv_usec * 1000;
			ts.tv_sec += WORKER_MANAGER_TIMER_INTERVAL;

			if( ioprocess ) {
				if( ioprocess->numOfPendingRequest() > 0 ) {
					continue;
				}
			}

			int retval = pthread_cond_timedwait(&worker->m_cond,
					&global_big_mutex, &ts);

			if( worker->m_can_use == false ) {
				// Need to die
				worker->m_is_waiting = false;
				worker_exit(worker, true);
			} else {
				// If timeout happends, need to check m_must_be_alive
				if( retval == ETIMEDOUT ) {
					if( ioprocess ) {
						if( ioprocess->numOfPendingRequest() > 0 ) {
							continue;
						}
					}

					if( !worker->m_must_be_alive ) {
						// Need to die according to the min number of workers

						worker->m_is_waiting = false;
						worker->m_can_use = false;

						worker_exit(worker, false);
					} else {
						dprintf(D_FULLDEBUG, "Thread(%d) must be alive for "
								"another request\n", worker->m_id);
					}
				}
			}
		}

		worker->m_is_doing = true;
		worker->m_is_waiting = false;
		worker->m_must_be_alive = false;

		if(!handle_gahp_command(new_request) ) {
			dprintf(D_ALWAYS, "ERROR (io_loop) processing %s\n",
					new_request->m_raw_cmd.c_str());
		} else {
			dprintf(D_FULLDEBUG, "CMD(\"%s\") is done with result %s",
					new_request->m_raw_cmd.c_str(),
					new_request->m_result.c_str());
		}

		// Now we processed one request
		delete new_request;
		new_request = NULL;

	}

	alicloud_gahp_release_big_mutex();
	return NULL;
}

//
// This pool allocator isn't thread-safe, but it avoids calling glibc's
// allocator, which is also not thread-safe ... and called from two
// threads simultaneously (if a request arrives while curl is working).
//

const unsigned int requestPoolSize = 4096;
bool initializedRequestMap = false;
unsigned char requestMap[ requestPoolSize ];
unsigned char requests[ requestPoolSize * sizeof( Request ) ];

void * Request::operator new( size_t i ) throw() {
	static unsigned int index = 0;
	if( ! initializedRequestMap ) {
		memset( requestMap, 0, requestPoolSize );
		initializedRequestMap = true;
	}

	if( i != sizeof( Request ) ) {
		fprintf( stderr, "Request::operator new() called with wrong size request.\n" );
		return NULL;
	}

	unsigned int count = 0;
	for( ; count < requestPoolSize; ++index, ++count ) {
		if( index >= requestPoolSize ) {
			index = 0;
		}

		if( requestMap[ index ] == 0 ) {
			requestMap[ index ] = 1;
			break;
		}
	}

	if( count == requestPoolSize ) {
		fprintf( stderr, "Failed to find space for request in pool.\n" );
		return NULL;
	}

	void * address = &(((Request *)requests)[ index ]);
	++index;

	// fprintf( stderr, "Allocating %u = %p.\n", index, address );
	return address;
}

void Request::operator delete( void * v ) throw() {
	if( requests <= v && v < requests + sizeof( requests ) ) {
		unsigned long offset = ((unsigned long)v) - ((unsigned long)requests);
		offset = offset / sizeof( Request );
		// fprintf( stderr, "Deleting %p = %lu.\n", v, offset );
		requestMap[ offset ] = 0;
	} else {
		fprintf( stderr, "Reqeust::operator delete( %p ) invalid: not in pool (%p -- %p).\n", v, requests, requests + sizeof( requests ) );
	}
}
