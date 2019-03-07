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


#ifndef CONDOR_GAHP_CLIENT_H
#define CONDOR_GAHP_CLIENT_H

#include "condor_common.h"
#include "condor_daemon_core.h"
#include "gahp_common.h"

#include "classad_hashtable.h"
#include "globus_utils.h"
#include "proxymanager.h"
#include "condor_arglist.h"
#include <map>
#include <deque>
#include <list>
#include <vector>
#include <string>
#include <utility>

#define NULLSTRING "NULL"

struct GahpProxyInfo
{
	Proxy *proxy;
	int cached_expiration;
	int num_references;
};

typedef void (* unicore_gahp_callback_func_t)(const char *update_ad_string);

class BoincJob;
class BoincResource;

#define GAHPCLIENT_DEFAULT_SERVER_ID "DEFAULT"
#define GAHPCLIENT_DEFAULT_SERVER_PATH "DEFAULT"

// Additional error values that GAHP calls can return
///
static const int GAHPCLIENT_COMMAND_PENDING = -100;
///
static const int GAHPCLIENT_COMMAND_NOT_SUPPORTED = -101;
///
static const int GAHPCLIENT_COMMAND_NOT_SUBMITTED = -102;
///
static const int GAHPCLIENT_COMMAND_TIMED_OUT = -103;

void GahpReconfig();

class GenericGahpClient;

class GahpServer : public Service {
 public:
	static GahpServer *FindOrCreateGahpServer(const char *id,
											  const char *path,
											  const ArgList *args = NULL);
	static HashTable <HashKey, GahpServer *> GahpServersById;

	GahpServer(const char *id, const char *path, const ArgList *args = NULL);
	~GahpServer();

	bool Startup();
	bool Initialize(Proxy * proxy);
	bool CreateSecuritySession();

	void DeleteMe();

	static const int m_buffer_size;
	char *m_buffer;
	int m_buffer_pos;
	int m_buffer_end;
	int buffered_read( int fd, void *buf, int count );
	int buffered_peek();

	int m_deleteMeTid;

	bool m_in_results;

	static int m_reaperid;

	static void Reaper(Service *,int pid,int status);

	class GahpStatistics {
	public:
		GahpStatistics();

		void Publish( ClassAd & ad ) const;
		void Unpublish( ClassAd & ad ) const;

		static void Tick();

		static int RecentWindowMax;
		static int RecentWindowQuantum;
		static int Tick_tid;

		stats_entry_recent<int> CommandsIssued;
		stats_entry_recent<int> CommandsTimedOut;
		stats_entry_abs<int> CommandsInFlight;
		stats_entry_abs<int> CommandsQueued;
		stats_entry_recent<Probe> CommandRuntime;

		StatisticsPool Pool;
	};

	GahpStatistics m_stats;

	void read_argv(Gahp_Args &g_args);
	void read_argv(Gahp_Args *g_args) { read_argv(*g_args); }
	void write_line(const char *command, const char *debug_cmd = NULL);
	void write_line(const char *command,int req,const char *args);
	int pipe_ready(int pipe_end);
	int err_pipe_ready(int pipe_end);

	void AddGahpClient();
	void RemoveGahpClient();

	int ProxyCallback();
	void doProxyCheck();
	GahpProxyInfo *RegisterProxy( Proxy *proxy );
	void UnregisterProxy( Proxy *proxy );

	/** Set interval to automatically poll the Gahp Server for results.
		If the Gahp server supports async result notification, then
		the poll interval defaults to zero (disabled).  Otherwise,
		it will default to 5 seconds.  
	 	@param interval Polling interval in seconds, or zero
		to disable polling all together.  
		@return true on success, false otherwise.
		@see getPollInterval
	*/
	void setPollInterval(unsigned int interval);

	/** Retrieve the interval used to auto poll the Gahp Server 
		for results.  Also used to determine if async notification
		is in effect.
	 	@return Polling interval in seconds, or a zero
		to represent auto polling is disabled (likely if
		the Gahp server supports async notification).
		@see setPollInterval
	*/
	unsigned int getPollInterval();

	/** Immediately poll the Gahp Server for results.  Normally,
		this method is invoked automatically either by a timer set
		via setPollInterval or by a Gahp Server async result
		notification.
		@see setPollInterval
	*/
	void poll();

	void poll_real_soon();

	bool useBoincResource( BoincResource *resource );
	bool command_boinc_select_project( const char *url, const char *auth_file );

	bool cacheProxyFromFile( GahpProxyInfo *new_proxy );
	bool uncacheProxy( GahpProxyInfo *gahp_proxy );
	bool useCachedProxy( GahpProxyInfo *new_proxy, bool force = false );

	bool command_cache_proxy_from_file( GahpProxyInfo *new_proxy );
	bool command_use_cached_proxy( GahpProxyInfo *new_proxy );

		// Methods for private GAHP commands
	bool command_version();
	bool command_initialize_from_file(const char *proxy_path,
									  const char *command=NULL);
	bool command_commands();
	bool command_async_mode_on();
	bool command_response_prefix(const char *prefix);
	bool command_condor_version();

	int new_reqid();

	int next_reqid;
	bool rotated_reqids;

	unsigned int m_reference_count;
	HashTable<int,GenericGahpClient*> *requestTable;
	std::deque<int> waitingHighPrio;
	std::deque<int> waitingMediumPrio;
	std::deque<int> waitingLowPrio;

	int m_gahp_pid;
	int m_gahp_readfd;
	int m_gahp_writefd;
	int m_gahp_errorfd;
	int m_gahp_real_readfd;
	int m_gahp_real_errorfd;
	std::string m_gahp_error_buffer;
	std::list<std::string> m_gahp_error_list;
	bool m_gahp_startup_failed;
	char m_gahp_version[150];
	std::string m_gahp_condor_version;
	StringList * m_commands_supported;
	bool use_prefix;
	unsigned int m_pollInterval;
	int poll_tid;
	bool poll_pending;

	int max_pending_requests;
	int num_pending_requests;
	GahpProxyInfo *current_proxy;
	bool skip_next_r;
	char *binary_path;
	ArgList binary_args;
	char *my_id;
	int m_ssh_forward_port;

	std::string m_sec_session_id;

	char *globus_gass_server_url;
	char *globus_gt2_gram_callback_contact;
	void *globus_gt2_gram_user_callback_arg;
	globus_gram_client_callback_func_t globus_gt2_gram_callback_func;
	int globus_gt2_gram_callback_reqid;

	unicore_gahp_callback_func_t unicore_gahp_callback_func;
	int unicore_gahp_callback_reqid;

	BoincResource *m_currentBoincResource;

	GahpProxyInfo *master_proxy;
	int proxy_check_tid;
	bool is_initialized;
	bool can_cache_proxies;
	HashTable<HashKey,GahpProxyInfo*> *ProxiesByFilename;
}; // end of class GahpServer

class GenericGahpClient : public Service {
	friend class GahpServer;

	public:
		GenericGahpClient(	const char * id = GAHPCLIENT_DEFAULT_SERVER_ID,
							const char * path = GAHPCLIENT_DEFAULT_SERVER_PATH,
							const ArgList * args = NULL );
		virtual ~GenericGahpClient();

		bool Startup();
		bool Initialize( Proxy * proxy );
		bool CreateSecuritySession();

		void purgePendingRequests() { clear_pending(); }
		bool pendingRequestIssued() { return pending_submitted_to_gahp || pending_result; }

		enum mode {
				/** */ normal,
				/** */ results_only,
				/** */ blocking
		};

		void setMode( mode m ) { m_mode = m; }
		mode getMode() { return m_mode; }

		void setTimeout( int t ) { m_timeout = t; }
		unsigned int getTimeout() { return m_timeout; }

		void setNotificationTimerId( int tid ) { user_timerid = tid; }
		int getNotificationTimerId() { return user_timerid; }

		void PublishStats( ClassAd * ad );

		void setNormalProxy( Proxy * proxy );
		void setDelegProxy( Proxy * proxy );
		Proxy * getMasterProxy();

		bool isStarted() { return server->m_gahp_pid != -1 && !server->m_gahp_startup_failed; }
		bool isInitialized() { return server->is_initialized; }

		StringList *getCommands() { return server->m_commands_supported; }

	    void setErrorString( const std::string & newErrorString );
		const char * getErrorString();

		const char * getGahpStderr();
		const char * getVersion();
		const char * getCondorVersion();

		enum PrioLevel {
			low_prio,
			medium_prio,
			high_prio
		};

		// The caller must delete 'result' if it isn't NULL.
		int callGahpFunction(
			const char * command,
			const std::vector< YourString > & arguments,
			Gahp_Args * & result,
			PrioLevel priority = medium_prio );

	protected:
		void clear_pending();
		bool is_pending( const char *command, const char *buf );
		void now_pending (const char *command,const char *buf,
						 GahpProxyInfo *proxy = NULL,
						 PrioLevel prio_level = medium_prio );
		Gahp_Args * get_pending_result( const char *, const char * );
		bool check_pending_timeout( const char *, const char * );
		int reset_user_timer( int tid );
		void reset_user_timer_alarm();

		unsigned int m_timeout;
		mode m_mode;
		char * pending_command;
		char * pending_args;
		int pending_reqid;
		Gahp_Args * pending_result;
		time_t pending_timeout;
		int pending_timeout_tid;
		time_t pending_submitted_to_gahp;
		int user_timerid;
		GahpProxyInfo * normal_proxy;
		GahpProxyInfo * deleg_proxy;
		GahpProxyInfo * pending_proxy;
		std::string error_string;

		// Used in now_pending(), not worth rewriting to get rid of for now.
		BoincResource *m_boincResource;

		GahpServer * server;
};


class GahpClient : public GenericGahpClient {
	// Hopefully not necessary.
	// friend class GahpServer;

	public:
		
		/** @name Instantiation. 
		 */
		//@{
	
			/// Constructor
		GahpClient(const char *id=GAHPCLIENT_DEFAULT_SERVER_ID,
				   const char *path=GAHPCLIENT_DEFAULT_SERVER_PATH,
				   const ArgList *args=NULL);
			/// Destructor
		~GahpClient();
		
		//@}

		void setBoincResource( BoincResource *server );

		int getSshForwardPort() { return server->m_ssh_forward_port; }

		//-----------------------------------------------------------
		
		/**@name Globus Methods
		 * These methods have the exact same API as their native Globus
		 * Toolkit counterparts.  
		 */
		//@{

		const char *
			getGlobusGassServerUrl() { return server->globus_gass_server_url; }

		const char *getGt2CallbackContact()
			{ return server->globus_gt2_gram_callback_contact; }

		/// cache it from the gahp
		const char *
		globus_gram_client_error_string(int error_code);

		///
		int 
		globus_gram_client_callback_allow(
			globus_gram_client_callback_func_t callback_func,
			void * user_callback_arg,
			char ** callback_contact);

		///
		int 
		globus_gram_client_job_request(const char * resource_manager_contact,
			const char * description,
			const int limited_deleg,
			const char * callback_contact,
			std::string & job_contact,
			bool is_restart);

		///
		int 
		globus_gram_client_job_cancel(const char * job_contact);

		///
		int
		globus_gram_client_job_status(const char * job_contact,
			int * job_status,
			int * failure_code);

		///
		int
		globus_gram_client_job_signal(const char * job_contact,
			globus_gram_protocol_job_signal_t signal,
			const char * signal_arg,
			int * job_status,
			int * failure_code);

		///
		int
		globus_gram_client_job_callback_register(const char * job_contact,
			const int job_state_mask,
			const char * callback_contact,
			int * job_status,
			int * failure_code);

		///
		int 
		globus_gram_client_ping(const char * resource_manager_contact);

		///
		int
		globus_gram_client_job_refresh_credentials(const char *job_contact,
												   int limited_deleg);

		///
		int
		globus_gass_server_superez_init( char **gass_url, int port );

		///
		int 
		globus_gram_client_get_jobmanager_version(const char * resource_manager_contact);


		int
		condor_job_submit(const char *schedd_name, ClassAd *job_ad,
						  char **job_id);

		int
		condor_job_update_constrained(const char *schedd_name,
									  const char *constraint,
									  ClassAd *update_ad);

		int
		condor_job_status_constrained(const char *schedd_name,
									  const char *constraint,
									  int *num_ads, ClassAd ***ads);

		int
		condor_job_remove(const char *schedd_name, PROC_ID job_id,
						  const char *reason);

		int
		condor_job_update(const char *schedd_name, PROC_ID job_id,
						  ClassAd *update_ad);

		int
		condor_job_hold(const char *schedd_name, PROC_ID job_id,
						const char *reason);

		int
		condor_job_release(const char *schedd_name, PROC_ID job_id,
						   const char *reason);

		int
		condor_job_stage_in(const char *schedd_name, ClassAd *job_ad);

		int
		condor_job_stage_out(const char *schedd_name, PROC_ID job_id);

		int
		condor_job_refresh_proxy(const char *schedd_name, PROC_ID job_id,
								 const char *proxy_file);

		int
		condor_job_update_lease(const char *schedd_name,
								const SimpleList<PROC_ID> &jobs,
								const SimpleList<int> &expirations,
								SimpleList<PROC_ID> &updated );

		int
		blah_job_submit(ClassAd *job_ad, char **job_id);

		int
		blah_job_status(const char *job_id, ClassAd **status_ad);

		int
		blah_job_cancel(const char *job_id);

		int
		blah_job_refresh_proxy(const char *job_id, const char *proxy_file);

		int
		blah_download_sandbox(const char *sandbox_id, const ClassAd *job_ad,
							  std::string &sandbox_path);

		int
		blah_upload_sandbox(const char *sandbox_id, const ClassAd *job_ad);

		int
		blah_download_proxy(const char *sandbox_id, const ClassAd *job_ad);

		int
		blah_destroy_sandbox(const char *sandbox_id, const ClassAd *job_ad);

		bool
		blah_get_sandbox_path(const char *sandbox_id, std::string &sandbox_path);

		int
		nordugrid_submit(const char *hostname, const char *rsl, char *&job_id);

		int
		nordugrid_status(const char *hostname, const char *job_id,
						 char *&status);

		int
		nordugrid_ldap_query(const char *hostname, const char *ldap_base,
							 const char *ldap_filter, const char *ldap_attrs,
							 StringList &results);

		int
		nordugrid_cancel(const char *hostname, const char *job_id);

		int
		nordugrid_stage_in(const char *hostname, const char *job_id,
						   StringList &files);

		int
		nordugrid_stage_out(const char *hostname, const char *job_id,
							StringList &files);

		int
		nordugrid_stage_out2(const char *hostname, const char *job_id,
							 StringList &src_files, StringList &dest_files);

		int
		nordugrid_exit_info(const char *hostname, const char *job_id,
							bool &normal_exit, int &exit_code,
							float &wallclock, float &sys_cpu,
							float &user_cpu );

		int
		nordugrid_ping(const char *hostname);

		int
		gridftp_transfer(const char *src_url, const char *dst_url);

		///
		int 
		unicore_job_create(const char * description,
						   char ** job_contact);

		///
		int
		unicore_job_start(const char *job_contact);

		///
		int 
		unicore_job_destroy(const char * job_contact);

		///
		int
		unicore_job_status(const char * job_contact,
						   char **job_status);

		///
		int
		unicore_job_recover(const char * description);

		int
		unicore_job_callback(unicore_gahp_callback_func_t callback_func);

		int cream_delegate(const char *delg_service, const char *delg_id);
		
		int cream_job_register(const char *service, const char *delg_id, 
							   const char *jdl, const char *lease_id, char **job_id, char **upload_url, char **download_url);
		
		int cream_job_start(const char *service, const char *job_id);
		
		int cream_job_purge(const char *service, const char *job_id);

		int cream_job_cancel(const char *service, const char *job_id);

		int cream_job_suspend(const char *service, const char *job_id);

		int cream_job_resume(const char *service, const char *job_id);

		int cream_job_status(const char *service, const char *job_id, 
							 char **job_status, int *exit_code, char **failure_reason);

		struct CreamJobStatus {
			std::string job_id;
			std::string job_status;
			int exit_code;
			std::string failure_reason;
		};
		typedef std::map<std::string, CreamJobStatus> CreamJobStatusMap;
		int cream_job_status_all(const char *service, CreamJobStatusMap & job_ids);
		
		int cream_proxy_renew(const char *delg_service, const char *delg_id);
		
		int cream_ping(const char * service);
		
		int cream_set_lease(const char *service, const char *lease_id, time_t &lease_expiry);

		int gce_ping( const std::string &service_url,
					  const std::string &auth_file,
					  const std::string &project,
					  const std::string &zone );

		int gce_instance_insert( const std::string &service_url,
								 const std::string &auth_file,
								 const std::string &project,
								 const std::string &zone,
								 const std::string &instance_name,
								 const std::string &machine_type,
								 const std::string &image,
								 const std::string &metadata,
								 const std::string &metadata_file,
								 bool preemptible,
								 const std::string &json_file,
								 std::string &instance_id );

		int gce_instance_delete( std::string service_url,
								 const std::string &auth_file,
								 const std::string &project,
								 const std::string &zone,
								 const std::string &instance_name );

		int gce_instance_list( const std::string &service_url,
							   const std::string &auth_file,
							   const std::string &project,
							   const std::string &zone,
							   StringList &instance_ids,
							   StringList &instance_names,
							   StringList &statuses,
							   StringList &status_msgs );

		int boinc_ping();

		int boinc_submit( const char *batch_name,
						  const std::set<BoincJob *> &jobs );

		typedef std::vector< std::pair< std::string, std::string > > BoincBatchResults;
		typedef std::vector< BoincBatchResults > BoincQueryResults;
//		typedef std::vector< std::vector< std::pair< std::string, std::string > > > BoincQueryResults;
		int boinc_query_batches( StringList &batch_names,
								 const std::string& last_query_time,
								 std::string &new_query_time,
								 BoincQueryResults &results );

		typedef std::vector< std::pair< std::string, std::string> > BoincOutputFiles;
		int boinc_fetch_output( const char *job_name,
								const char *iwd,
								const char *std_err,
								bool transfer_all,
								const BoincOutputFiles &output_files,
								int &exit_status,
								double &cpu_time,
								double &wallclock_time );

		int boinc_abort_jobs( StringList &job_names );

		int boinc_retire_batch( const char *batch_name );

		int boinc_set_lease( const char *batch_name,
							 time_t new_lease_time );

#ifdef CONDOR_GLOBUS_HELPER_WANT_DUROC
	// Not yet ready for prime time...
	globus_duroc_control_barrier_release();
	globus_duroc_control_init();
	globus_duroc_control_job_cancel();
	globus_duroc_control_job_request();
	globus_duroc_control_subjob_states();
	globus_duroc_error_get_gram_client_error();
	globus_duroc_error_string();
#endif /* ifdef CONDOR_GLOBUS_HELPER_WANT_DUROC */

		//@}

	private:

};	// end of class GahpClient

class EC2GahpClient : public GahpClient {
	public:

		EC2GahpClient(	const char * id, const char * path, const ArgList * args );
		~EC2GahpClient();

		int describe_stacks(	const std::string & service_url,
								const std::string & publickeyfile,
								const std::string & privatekeyfile,

								const std::string & stackName,

								std::string & stackStatus,
								std::map< std::string, std::string > & outputs,
								std::string & errorCode );

		int create_stack(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & stackName,
							const std::string & templateURL,
							const std::string & capability,
							const std::map< std::string, std::string > & parameters,

							std::string & stackID,
							std::string & errorCode );

		int get_function(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & functionARN,

							std::string & functionHash,
							std::string & errorCode );

		int call_function(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & functionARN,
							const std::string & argumentBlob,

							std::string & returnBlob,
							std::string & errorCode );

		int put_targets(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & ruleName,
							const std::string & id,
							const std::string & arn,
							const std::string & input,

							std::string & errorCode );

		int remove_targets(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & ruleName,
							const std::string & id,

							std::string & errorCode );

		int put_rule(		const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & ruleName,
							const std::string & scheduleExpression,
							const std::string & state,

							std::string & ruleARN,
							std::string & error_code );

		int delete_rule(	const std::string & service_url,
							const std::string & publickeyfile,
							const std::string & privatekeyfile,

							const std::string & ruleName,

							std::string & error_code );

		struct LaunchConfiguration {
			YourString ami_id;
			YourString spot_price;
			YourString keypair;
			YourString user_data;
			YourString instance_type;
			YourString availability_zone;
			YourString vpc_subnet;
			YourString block_device_mapping;
			YourString iam_profile_arn;
			YourString iam_profile_name;
			StringList * groupnames;
			StringList * groupids;

			YourString weighted_capacity;

			LaunchConfiguration() : groupnames( NULL ), groupids( NULL ) { }
			LaunchConfiguration(	YourString a, YourString b, YourString c,
									YourString d, YourString f,
									YourString g, YourString h,
									YourString j, YourString k,
									YourString l,
									StringList * m, StringList * n,
									YourString o ) :
					ami_id( a ), spot_price( b ), keypair( c ),
					user_data( d ), instance_type( f ),
					availability_zone( g ), vpc_subnet( h ),
					block_device_mapping( j ), iam_profile_arn( k ),
					iam_profile_name( l ),
					groupnames( m ), groupids( n ),
					weighted_capacity( o ) { }

			void convertToJSON( std::string & s ) const;
		};

		int s3_upload(	const std::string & service_url,
						const std::string & publickeyfile,
						const std::string & privatekeyfile,

						const std::string & bucketName,
						const std::string & fileName,
						const std::string & path,

						std::string & error_code );

		int bulk_start(	const std::string & service_url,
						const std::string & publickeyfile,
						const std::string & privatekeyfile,

						const std::string & client_token,
						const std::string & spot_price,
						const std::string & target_capacity,
						const std::string & iam_fleet_role,
						const std::string & allocation_strategy,
						const std::string & valid_until,

						const std::vector< LaunchConfiguration > & launch_configurations,

						std::string & bulkRequestID,
						std::string & error_code );

		int bulk_start(	const std::string & service_url,
						const std::string & publickeyfile,
						const std::string & privatekeyfile,

						const std::string & client_token,
						const std::string & spot_price,
						const std::string & target_capacity,
						const std::string & iam_fleet_role,
						const std::string & allocation_strategy,
						const std::string & valid_until,

						const std::vector< std::string > & launch_configurations,

						std::string & bulkRequestID,
						std::string & error_code );

		int bulk_stop(	const std::string & service_url,
						const std::string & publickeyfile,
						const std::string & privatekeyfile,

						const std::string & bulkRequestID,

						std::string & error_code );

		int bulk_query(	const std::string & service_url,
						const std::string & publickeyfile,
						const std::string & privatekeyfile,

						StringList & returnStatus,
						std::string & error_code );

		int ec2_vm_start( const std::string & service_url,
						  const std::string & publickeyfile,
						  const std::string & privatekeyfile,
						  const std::string & ami_id,
						  const std::string & keypair,
						  const std::string & user_data,
						  const std::string & user_data_file,
						  const std::string & instance_type,
						  const std::string & availability_zone,
						  const std::string & vpc_subnet,
						  const std::string & vpc_ip,
						  const std::string & client_token,
						  const std::string & block_device_mapping,
						  const std::string & iam_profile_arn,
						  const std::string & iam_profile_name,
						  unsigned int maxCount,
						  StringList & groupnames,
						  StringList & groupids,
						  StringList & parametersAndValues,
						  std::vector< std::string > & instance_ids,
						  std::string & error_code );

		int ec2_vm_stop( const std::string & service_url,
						 const std::string & publickeyfile,
						 const std::string & privatekeyfile,
						 const std::string & instance_id,
						 std::string & error_code );

		int ec2_vm_stop( const std::string & service_url,
						 const std::string & publickeyfile,
						 const std::string & privatekeyfile,
						 const std::vector< std::string > & instance_ids,
						 std::string & error_code );

		int ec2_vm_status_all( const std::string & service_url,
							   const std::string & publickeyfile,
							   const std::string & privatekeyfile,
							   StringList & returnStatus,
							   std::string & error_code );

		int ec2_gahp_statistics( StringList & returnStatistics );

		int ec2_ping( const std::string & service_url,
					  const std::string & publickeyfile,
					  const std::string & privatekeyfile,
					  std::string & error_code );

		int ec2_vm_server_type( const std::string & service_url,
								const std::string & publickeyfile,
								const std::string & privatekeyfile,
								std::string & server_type,
								std::string & error_code );

		int ec2_vm_create_keypair( const std::string & service_url,
								   const std::string & publickeyfile,
								   const std::string & privatekeyfile,
								   const std::string & keyname,
								   const std::string & outputfile,
								   std::string & error_code );

		int ec2_vm_destroy_keypair( const std::string & service_url,
									const std::string & publickeyfile,
									const std::string & privatekeyfile,
									const std::string & keyname,
									std::string & error_code );

        /**
         * Used to associate an elastic ip with a running instance
         */
        int ec2_associate_address( const std::string & service_url,
                                   const std::string & publickeyfile,
                                   const std::string & privatekeyfile,
                                   const std::string & instance_id,
                                   const std::string & elastic_ip,
                                   StringList & returnStatus,
                                   std::string & error_code );

		// Used to associate a tag with an resource, like a running instance
        int ec2_create_tags( const std::string & service_url,
							 const std::string & publickeyfile,
							 const std::string & privatekeyfile,
							 const std::string & instance_id,
							 StringList & tags,
							 StringList & returnStatus,
							 std::string & error_code );

		/**
		 * Used to attach to an EBS volume(s).
		 */
		int ec2_attach_volume( const std::string & service_url,
                               const std::string & publickeyfile,
                               const std::string & privatekeyfile,
                               const std::string & volume_id,
							   const std::string & instance_id,
                               const std::string & device_id,
                               StringList & returnStatus,
                               std::string & error_code );

        // Is there a particular reason these aren't const references?
        int ec2_spot_start( const std::string & service_url,
                            const std::string & publickeyfile,
                            const std::string & privatekeyfile,
                            const std::string & ami_id,
                            const std::string & spot_price,
                            const std::string & keypair,
                            const std::string & user_data,
                            const std::string & user_data_file,
                            const std::string & instance_type,
                            const std::string & availability_zone,
                            const std::string & vpc_subnet,
                            const std::string & vpc_ip,
                            const std::string & client_token,
                            const std::string & iam_profile_arn,
                            const std::string & iam_profile_name,
                            StringList & groupnames,
                            StringList & groupids,
                            std::string & request_id,
                            std::string & error_code );

        int ec2_spot_stop( const std::string & service_url,
                           const std::string & publickeyfile,
                           const std::string & privatekeyfile,
                           const std::string & request_id,
                           std::string & error_code );

        int ec2_spot_status_all( const std::string & service_url,
                                 const std::string & publickeyfile,
                                 const std::string & privatekeyfile,
                                 StringList & returnStatus,
                                 std::string & error_code );
};

// Utility functions used all over the GAHP client code.
const char * escapeGahpString( const char * input );
const char * escapeGahpString( const std::string & input );

#endif /* ifndef CONDOR_GAHP_CLIENT_H */
