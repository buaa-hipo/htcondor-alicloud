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

#ifndef ALICLOUD_COMMANDS_H
#define ALICLOUD_COMMANDS_H

#include "condor_common.h"
#include "condor_string.h"
#include "MyString.h"
#include "string_list.h"

#include <map>

// EC2 Commands
#define ALICLOUD_COMMAND_VM_START             "EC2_VM_START"
#define ALICLOUD_COMMAND_VM_STOP              "EC2_VM_STOP"
#define ALICLOUD_COMMAND_VM_REBOOT            "EC2_VM_REBOOT"
#define ALICLOUD_COMMAND_VM_STATUS            "EC2_VM_STATUS"
#define ALICLOUD_COMMAND_VM_STATUS_ALL        "EC2_VM_STATUS_ALL"
#define ALICLOUD_COMMAND_VM_RUNNING_KEYPAIR   "EC2_VM_RUNNING_KEYPAIR"
#define ALICLOUD_COMMAND_VM_CREATE_GROUP      "EC2_VM_CREATE_GROUP"
#define ALICLOUD_COMMAND_VM_DELETE_GROUP      "EC2_VM_DELETE_GROUP"
#define ALICLOUD_COMMAND_VM_GROUP_NAMES       "EC2_VM_GROUP_NAMES"
#define ALICLOUD_COMMAND_VM_GROUP_RULES       "EC2_VM_GROUP_RULES"
#define ALICLOUD_COMMAND_VM_ADD_GROUP_RULE    "EC2_VM_ADD_GROUP_RULE"
#define ALICLOUD_COMMAND_VM_DEL_GROUP_RULE    "EC2_VM_DEL_GROUP_RULE"
#define ALICLOUD_COMMAND_VM_CREATE_KEYPAIR    "EC2_VM_CREATE_KEYPAIR"
#define ALICLOUD_COMMAND_VM_DESTROY_KEYPAIR   "EC2_VM_DESTROY_KEYPAIR"
#define ALICLOUD_COMMAND_VM_KEYPAIR_NAMES     "EC2_VM_KEYPAIR_NAMES"
#define ALICLOUD_COMMAND_VM_REGISTER_IMAGE    "EC2_VM_REGISTER_IMAGE"
#define ALICLOUD_COMMAND_VM_DEREGISTER_IMAGE  "EC2_VM_DEREGISTER_IMAGE"
#define ALICLOUD_COMMAND_VM_ASSOCIATE_ADDRESS "EC2_VM_ASSOCIATE_ADDRESS"
#define ALICLOUD_COMMAND_VM_ATTACH_VOLUME     "EC2_VM_ATTACH_VOLUME"
#define ALICLOUD_COMMAND_VM_CREATE_TAGS       "EC2_VM_CREATE_TAGS"
#define ALICLOUD_COMMAND_VM_SERVER_TYPE       "EC2_VM_SERVER_TYPE"

#define ALICLOUD_COMMAND_VM_START_SPOT        "EC2_VM_START_SPOT"
#define ALICLOUD_COMMAND_VM_STOP_SPOT         "EC2_VM_STOP_SPOT"
#define ALICLOUD_COMMAND_VM_STATUS_SPOT       "EC2_VM_STATUS_SPOT"
#define ALICLOUD_COMMAND_VM_STATUS_ALL_SPOT   "EC2_VM_STATUS_ALL_SPOT"

// For condor_annex.
#define ALICLOUD_COMMAND_BULK_START           "EC2_BULK_START"
#define ALICLOUD_COMMAND_PUT_RULE             "CWE_PUT_RULE"
#define ALICLOUD_COMMAND_PUT_TARGETS          "CWE_PUT_TARGETS"
#define ALICLOUD_COMMAND_BULK_STOP            "EC2_BULK_STOP"
#define ALICLOUD_COMMAND_DELETE_RULE          "CWE_DELETE_RULE"
#define ALICLOUD_COMMAND_REMOVE_TARGETS       "CWE_REMOVE_TARGETS"
#define ALICLOUD_COMMAND_GET_FUNCTION         "ALI_GET_FUNCTION"
#define ALICLOUD_COMMAND_S3_UPLOAD            "S3_UPLOAD"
#define ALICLOUD_COMMAND_CF_CREATE_STACK      "CF_CREATE_STACK"
#define ALICLOUD_COMMAND_CF_DESCRIBE_STACKS   "CF_DESCRIBE_STACKS"
#define ALICLOUD_COMMAND_CALL_FUNCTION        "ALI_CALL_FUNCTION"
#define ALICLOUD_COMMAND_BULK_QUERY           "EC2_BULK_QUERY"


#define GENERAL_GAHP_ERROR_CODE             "GAHPERROR"
#define GENERAL_GAHP_ERROR_MSG              "GAHP_ERROR"

class AlicloudRequest {
    public:
        AlicloudRequest( int i, const char * c, int sv = 4 ) :
            includeResponseHeader(false), requestID(i), requestCommand(c),
            signatureVersion(sv), httpVerb( "POST" ) { }
        virtual ~AlicloudRequest();

        virtual bool SendRequest();
        virtual bool SendURIRequest();
        virtual bool SendJSONRequest( const std::string & payload );
        virtual bool SendS3Request( const std::string & payload );

    protected:
        typedef std::map< std::string, std::string > AttributeValueMap;
        AttributeValueMap query_parameters;
        AttributeValueMap headers;

        std::string serviceURL;
        std::string accessKeyFile;
        std::string secretKeyFile;

        std::string errorMessage;
        std::string errorCode;

        std::string resultString;
        unsigned long responseCode;

        bool includeResponseHeader;

		// For tracing.
		int requestID;
		std::string requestCommand;
		struct timespec mutexReleased;
		struct timespec lockGained;
		struct timespec requestBegan;
		struct timespec requestEnded;
		struct timespec mutexGained;
		struct timespec sleepBegan;
		struct timespec liveLine;
		struct timespec sleepEnded;

		// So that we don't bother to send expired signatures.
		struct timespec signatureTime;

		int signatureVersion;

		// For signature v4.  Use if the URL is not of the form
		// '<service>.<region>.provider.tld'.  (Includes S3.)
		std::string region;
		std::string service;

		// Some odd services (Lambda) require the use of GET.
		// Some odd services (S3) requires the use of PUT.
		std::string httpVerb;

	private:
		bool sendV2Request();
		bool sendV4Request( const std::string & payload, bool sendContentSHA = false );

		std::string canonicalizeQueryString();
		bool createV4Signature( const std::string & payload, std::string & authorizationHeader, bool sendContentSHA = false );

		bool sendPreparedRequest(	const std::string & protocol,
									const std::string & uri,
									const std::string & payload );
};

// EC2 Commands

class AlicloudVMStart : public AlicloudRequest {
	public:
		AlicloudVMStart( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMStart();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
        std::string instanceID;
        std::vector< std::string > instanceIDs;
};

class AlicloudVMStartSpot : public AlicloudVMStart {
    public:
		AlicloudVMStartSpot( int i, const char * c ) : AlicloudVMStart( i, c ) { }
        virtual ~AlicloudVMStartSpot();

        virtual bool SendRequest();

        static bool ioCheck( char ** argv, int argc );
        static bool workerFunction( char ** argv, int argc, std::string & result_string );

    protected:
        std::string spotRequestID;
};

class AlicloudVMStop : public AlicloudRequest {
	public:
		AlicloudVMStop( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMStop();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudVMStopSpot : public AlicloudVMStop {
    public:
		AlicloudVMStopSpot( int i, const char * c ) : AlicloudVMStop( i, c ) { }
        virtual ~AlicloudVMStopSpot();

        // EC2_VM_STOP_SPOT uses the same argument structure as EC2_VM_STOP.
		// static bool ioCheck( char ** argv, int argc );
		static bool workerFunction( char ** argv, int argc, std::string & result_string );
};

#define ALICLOUD_STATUS_RUNNING "running"
#define ALICLOUD_STATUS_PENDING "pending"
#define ALICLOUD_STATUS_SHUTTING_DOWN "shutting-down"
#define ALICLOUD_STATUS_TERMINATED "terminated"

class AlicloudStatusResult {
	public:
		std::string instance_id;
		std::string status;
		std::string ami_id;
		std::string public_dns;
		std::string private_dns;
		std::string keyname;
		std::string instancetype;
        std::string stateReasonCode;
        std::string clientToken;
        std::string spotFleetRequestID;

        std::vector< std::string > securityGroups;
};

class AlicloudVMStatusAll : public AlicloudRequest {
	public:
		AlicloudVMStatusAll( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMStatusAll();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
        std::vector< AlicloudStatusResult > results;
};

class AlicloudVMStatus : public AlicloudVMStatusAll {
	public:
		AlicloudVMStatus( int i, const char * c ) : AlicloudVMStatusAll( i, c ) { }
		virtual ~AlicloudVMStatus();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudStatusSpotResult {
    public:
        std::string state;
        std::string launch_group;
        std::string request_id;
        std::string instance_id;
        std::string status_code;
};

class AlicloudVMStatusSpot : public AlicloudVMStatus {
    public:
		AlicloudVMStatusSpot( int i, const char * c ) : AlicloudVMStatus( i, c ) { }
        virtual ~AlicloudVMStatusSpot();

        virtual bool SendRequest();

        // EC2_VM_STATUS_SPOT uses the same argument structure as EC2_VM_STATUS_SPOT.
		// static bool ioCheck( char ** argv, int argc );
		static bool workerFunction( char ** argv, int argc, std::string & result_string );

    protected:
        std::vector< AlicloudStatusSpotResult > spotResults;
};

class AlicloudVMStatusAllSpot : public AlicloudVMStatusSpot {
    public:
		AlicloudVMStatusAllSpot( int i, const char * c ) : AlicloudVMStatusSpot( i, c ) { }
        virtual ~AlicloudVMStatusAllSpot();

		static bool ioCheck( char ** argv, int argc );
		static bool workerFunction( char ** argv, int argc, std::string & result_string );
};

class AlicloudVMCreateKeypair : public AlicloudRequest {
	public:
		AlicloudVMCreateKeypair( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMCreateKeypair();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
    	std::string privateKeyFileName;
};

class AlicloudVMDestroyKeypair : public AlicloudRequest {
	public:
		AlicloudVMDestroyKeypair( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMDestroyKeypair();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudAssociateAddress : public AlicloudRequest {
    public:
		AlicloudAssociateAddress( int i, const char * c ) : AlicloudRequest( i, c ) { }
        virtual ~AlicloudAssociateAddress();

        static bool ioCheck(char **argv, int argc);
        static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudCreateTags : public AlicloudRequest {
    public:
		AlicloudCreateTags( int i, const char * c ) : AlicloudRequest( i, c ) { }
        virtual ~AlicloudCreateTags();

        static bool ioCheck(char **argv, int argc);
        static bool workerFunction(char **argv, int argc, std::string &result_string);
};

/**
 * AlicloudAttachVolume - Will attempt to attach a running instance to an EBS volume
 * @see http://docs.alicloudwebservices.com/ALIEC2/latest/APIReference/index.html?ApiReference-query-AttachVolume.html
 */
class AlicloudAttachVolume : public AlicloudRequest {
    public:
        AlicloudAttachVolume( int i, const char * c ) : AlicloudRequest( i, c ) { }
        virtual ~AlicloudAttachVolume();

        static bool ioCheck(char **argv, int argc);
        static bool workerFunction(char **argv, int argc, std::string &result_string);
};


class AlicloudVMServerType : public AlicloudRequest {
	public:
        AlicloudVMServerType( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudVMServerType();

		virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		std::string serverType;
};

// Spot Fleet commands
class AlicloudBulkStart : public AlicloudRequest {
	public:
		AlicloudBulkStart( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudBulkStart();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
    	void setLaunchSpecificationAttribute( int, std::map< std::string, std::string > &, const char *, const char * = NULL );

		std::string bulkRequestID;
};

class AlicloudBulkStop : public AlicloudRequest {
	public:
		AlicloudBulkStop( int i, const char * c ) : AlicloudRequest( i, c ), success( true ) { }
		virtual ~AlicloudBulkStop();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		bool success;
};

class AlicloudPutRule : public AlicloudRequest {
	public:
		AlicloudPutRule( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudPutRule();

		virtual bool SendJSONRequest( const std::string & payload );

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
		std::string ruleARN;
};

class AlicloudDeleteRule : public AlicloudRequest {
	public:
		AlicloudDeleteRule( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudDeleteRule();

		virtual bool SendJSONRequest( const std::string & payload );

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudPutTargets : public AlicloudRequest {
	public:
		AlicloudPutTargets( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudPutTargets();

		virtual bool SendJSONRequest( const std::string & payload );

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudRemoveTargets : public AlicloudRequest {
	public:
		AlicloudRemoveTargets( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudRemoveTargets();

		virtual bool SendJSONRequest( const std::string & payload );

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);
};

class AlicloudGetFunction : public AlicloudRequest {
	public:
		AlicloudGetFunction( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudGetFunction();

		virtual bool SendURIRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
		std::string functionHash;
};

class AlicloudS3Upload : public AlicloudRequest {
	public:
		AlicloudS3Upload( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudS3Upload();

		virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		std::string path;
};

class AlicloudCreateStack : public AlicloudRequest {
	public:
		AlicloudCreateStack( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudCreateStack();

		virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		std::string stackID;
};

class AlicloudDescribeStacks : public AlicloudRequest {
	public:
		AlicloudDescribeStacks( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudDescribeStacks();

		virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		std::string stackStatus;
		std::vector< std::string > outputs;
};

class AlicloudCallFunction : public AlicloudRequest {
	public:
		AlicloudCallFunction( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudCallFunction();

		virtual bool SendJSONRequest( const std::string & payload );

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

    protected:
    	std::string success;
		std::string instanceID;
};

class AlicloudBulkQuery : public AlicloudRequest {
	public:
		AlicloudBulkQuery( int i, const char * c ) : AlicloudRequest( i, c ) { }
		virtual ~AlicloudBulkQuery();

        virtual bool SendRequest();

		static bool ioCheck(char **argv, int argc);
		static bool workerFunction(char **argv, int argc, std::string &result_string);

	protected:
		StringList resultList;
};

#endif

