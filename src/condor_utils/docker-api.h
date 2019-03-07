#ifndef _CONDOR_DOCKER_API_H
#define _CONDOR_DOCKER_API_H

#include <string>
#include "condor_arglist.h"


class DockerAPI {
	public:
		static int default_timeout;
		static const int docker_hung = -9; // this error code is returned when we timed out a command to docker.

		/**
		 * Runs command in the Docker identified by imageID.  The container
		 * will be named name.  The command will run in the given
		 * environment with the given arguments.  If directory is non-empty,
		 * it will be mapped into the container and the command run there
		 * [TODO].
		 *
		 * If run() succeeds, the pid will be that of a process which will
		 * terminate when the instance does.  The error will be unchanged.
		 *
		 * If run() fails, it will return a negative number [TODO: and set
		 * error to ....]
		 *
		 * @param name 			If empty, Docker will generate a random name.  [FIXME]
		 * @param imageID		For now, must be the GUID.
		 * @param command		...
		 * @param arguments		...
		 * @param environment	...
		 * @param directory		...
		 * @param pid			On success, will be set to the PID of a process which will terminate when the container does.  Otherwise, unchanged.
		 * @param childFDs		The redirected std[in|out|err] FDs.
		 * @param error			On success, unchanged.  Otherwise, [TODO].
		 * @return 				0 on success, negative otherwise.
		 */
		static int run(			ClassAd &machineAd,
						ClassAd &jobAd,
						const std::string & name,
						const std::string & imageID,
						const std::string & command,
						const ArgList & arguments,
						const Env & environment,
						const std::string & directory,
						const std::list<std::string> extraVolumes,
						int & pid,
						int * childFDs,
						CondorError & error );

		/**
		 * Releases the disk space (but not the image) associated with
		 * the given container.
		 *
		 * @param container		The Docker GUID, or the name passed to run().
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int rm( const std::string & container, CondorError & err );

		/**
		 * Releases the named image
		 *
		 * @param image		The Docker image name or GUUID.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int rmi( const std::string & image, CondorError & err );

		/**
		 * Sends a signal to the first process in the named container
		 *
		 * @param image		The Docker image name or GUUID.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */

		static int kill( const std::string & image, CondorError & err );

		/**
		 * Sends the given signal to the specified container's primary process.
		 *
		 * @param container		The Docker GUID, or the name passed to run().
		 * @param signal		The signal to send.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int kill( const std::string & container, int signal, CondorError & err );

		// Only available in Docker 1.1 or later.
		static int pause( const std::string & container, CondorError & err );

		// Only available in Docker 1.1 or later.
		static int unpause( const std::string & container, CondorError & err );

		static int stats(const std::string &container, uint64_t &memUsage, uint64_t &netIn, uint64_t &netOut, uint64_t &userCpu, uint64_t &sysCpu);

		/**
		 * Obtains the docker-inspect values State.Running and State.ExitCode.
		 *
		 * @param container		The Docker GUID, or the name passed to run().
		 * @param isRunning		On success, will be set to State.Running.  Otherwise, unchanged.
		 * @param exitCode		On success, will be set to State.ExitCode.  Otherwise, unchanged.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int getStatus( const std::string & container, bool isRunning, int & result, CondorError & err );

		/**
		 * Attempts to detect the presence of a working Docker installation.
		 *
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int detect( CondorError & err );

		/**
		 * Obtains the configured DOCKER's version string.
		 *
		 * @param version		On success, will be set to the version string.  Otherwise, unchanged.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int version( std::string & version, CondorError & err );
		static int majorVersion;
		static int minorVersion;

		/**
		 * Returns a ClassAd corresponding to a subset of the output of
		 * 'docker inspect'.
		 *
		 * @param container		The Docker GUID, or the name passed to run().
		 * @param inspectionAd	Populated on success, unchanged otherwise.
		 * @param error			....
		 * @return				0 on success, negative otherwise.
		 */
		static int inspect( const std::string & container, ClassAd * inspectionAd, CondorError & err );
};

#endif /* _CONDOR_DOCKER_API_H */
