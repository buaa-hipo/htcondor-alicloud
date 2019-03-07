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
#include "condor_debug.h"
#include "condor_config.h"
#include "condor_string.h"
#include "string_list.h"
#include "condor_arglist.h"
#include "util_lib_proto.h"
#include "internet.h"
#include "basename.h"
#include "alicloudgahp_common.h"
#include "alicloudCommands.h"

// Expecting:
// EC2_VM_START <req-id> <service-url> <accesskeyfile> <secretkeyfile>
//              <ami-id> <keypair> <userdata> <userdatafile> <instance-type>
//				<availability-zone> 
//				<Amount>
//              <VSwitchId>
//              <security-group-id>
bool AlicloudVMStart::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 21) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]) &&
		verify_string_name(argv[6]) &&
		verify_string_name(argv[7]) &&
		verify_string_name(argv[8]) &&
		verify_string_name(argv[9]) &&
		verify_string_name(argv[10]) &&
		verify_string_name(argv[11]) &&
		verify_string_name(argv[12]) &&
		verify_string_name(argv[13]) &&
		verify_string_name(argv[14]) &&
		verify_string_name(argv[15]) &&
		verify_string_name(argv[16]) &&
		verify_number(argv[17]) &&
		verify_string_name(argv[18]) &&
		verify_string_name(argv[19]) &&
		verify_string_name(argv[20]);
}

// Expecting:EC2_VM_START_SPOT <req_id>
// <serviceurl> <accesskeyfile> <secretkeyfile>
// <ami-id> <spot-price> <keypair> <userdata> <userdatafile>
//          <instancetype> <availability_zone> <vpc_subnet> <vpc_ip>
//          <client-token> <iam-profile-arn> <iam-profile-name>
//			<groupname>* <NULLSTRING> <groupid>* <NULLSTRING>
bool AlicloudVMStartSpot::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 19) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]) &&
		verify_string_name(argv[6]) &&
		verify_string_name(argv[7]) &&
		verify_string_name(argv[8]) &&
		verify_string_name(argv[9]) &&
		verify_string_name(argv[10]) &&
		verify_string_name(argv[11]) &&
		verify_string_name(argv[12]) &&
		verify_string_name(argv[13]) &&
		verify_string_name(argv[14]) &&
		verify_string_name(argv[15]) &&
		verify_string_name(argv[16]) &&
		verify_string_name(argv[17]) &&
		verify_string_name(argv[18]);
}

// Expecting:EC2_VM_STOP <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <instance-id>
bool AlicloudVMStop::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 6) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]);
}

// Expecting:EC2_VM_STATUS <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <instance-id>
bool AlicloudVMStatus::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 6) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]);
}

bool AlicloudVMStatusAllSpot::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 5) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]);
}

// Expecting:EC2_VM_ASSOCIATE_ADDRESS  <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <instance-id> <elastic-ip> 
bool AlicloudAssociateAddress::ioCheck(char **argv, int argc)
{
    return verify_number_args(argc, 7) &&
        verify_request_id(argv[1]) &&
        verify_string_name(argv[2]) &&
        verify_string_name(argv[3]) &&
        verify_string_name(argv[4]) &&
        verify_string_name(argv[5]) && 
        verify_string_name(argv[6]);
}

// Expecting:EC2_VM_ATTACH_VOLUME <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <volume-id> <instance-id> <device-id>
bool AlicloudAttachVolume::ioCheck(char **argv, int argc)
{
    return verify_number_args(argc, 8) &&
        verify_request_id(argv[1]) &&
        verify_string_name(argv[2]) &&
        verify_string_name(argv[3]) &&
        verify_string_name(argv[4]) &&
        verify_string_name(argv[5]) && 
        verify_string_name(argv[6]) && 
        verify_string_name(argv[7]);
}

// Expecting:EC2_VM_STATUS_ALL <req_id> <serviceurl> <accesskeyfile> <secretkeyfile>
bool AlicloudVMStatusAll::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 5) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]);
}

// Expecting:EC2_VM_CREATE_KEYPAIR <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <keyname> <outputfile>
bool AlicloudVMCreateKeypair::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 7) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]) &&
		verify_string_name(argv[6]);
}

// Expecting:EC2_VM_DESTROY_KEYPAIR <req_id> <serviceurl> <accesskeyfile> <secretkeyfile> <keyname>
bool AlicloudVMDestroyKeypair::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 6) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]);
}

// Expecting:EC2_VM_SERVER_TYPE <req_id> <serviceurl> <accesskeyfile> <secretkeyfile>
bool AlicloudVMServerType::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 5) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]);
}

// Expecting:	EC2_BULK_START <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<client-token> <spot-price> <target-capacity>
//				<iam-fleet-role> <allocation-strategy> <valid-until>
//				<launch-configuration-json-blob>+ <NULLSTRING>
bool AlicloudBulkStart::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 13 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] ) &&
		verify_string_name( argv[6] ) &&
		verify_string_name( argv[7] ) &&
		verify_string_name( argv[8] ) &&
		verify_string_name( argv[9] ) &&
		verify_string_name( argv[10] ) &&
		verify_string_name( argv[11] ) &&
		verify_string_name( argv[12] );
}

// Expecting:	CWE_PUT_RULE <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<rule-name> <schedule-expression> <desired-state>
bool AlicloudPutRule::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 8 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] ) &&
		verify_string_name( argv[6] ) &&
		verify_string_name( argv[7] );
}

// Expecting:	CWE_PUT_TARGETS <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<rule-name> <target-id> <target-arn> <target-input>
bool AlicloudPutTargets::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 9 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] ) &&
		verify_string_name( argv[6] ) &&
		verify_string_name( argv[7] ) &&
		verify_string_name( argv[8] );
}

// Expecting:	EC2_BULK_STOP <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<bulk-request-id>
bool AlicloudBulkStop::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 6 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] );
}

// Expecting:	CWE_DELETE_RULE <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<rule-name>
bool AlicloudDeleteRule::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 6 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] );
}

// Expecting:	CWE_REMOVE_TARGETS <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<rule-name> <target-id>
bool AlicloudRemoveTargets::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 7 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] ) &&
		verify_string_name( argv[6] );
}

// Expecting:	ALI_GET_FUNCTION <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<function-name-or-arn>
bool AlicloudGetFunction::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 6 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] );
}

// Expecting:	S3_UPLOAD <req_id>
//				<serviceurl> <accesskeyfile> <secretkeyfile>
//				<bucketName> <fileName> <path>
bool AlicloudS3Upload::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 8) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]) &&
		verify_string_name(argv[6]) &&
		verify_string_name(argv[7]);
}

// Expecting:	CF_CREATE_STACK <req_id>
//				<serviceurl> <accesskeyfile> <secretkeyfile>
//				<stackName> <templateURL> <capability>
//				(<parameters-name> <parameter-value>)* <NULLSTRING>
bool AlicloudCreateStack::ioCheck(char **argv, int argc)
{
	return verify_min_number_args(argc, 9) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]) &&
		verify_string_name(argv[6]) &&
		verify_string_name(argv[7]) &&
		verify_string_name(argv[8]);
}

// Expecting:	CF_DESCRIBE_STACKS <req_id>
//				<serviceurl> <accesskeyfile> <secretkeyfile>
//				<stackName>
bool AlicloudDescribeStacks::ioCheck(char **argv, int argc)
{
	return verify_number_args(argc, 6) &&
		verify_request_id(argv[1]) &&
		verify_string_name(argv[2]) &&
		verify_string_name(argv[3]) &&
		verify_string_name(argv[4]) &&
		verify_string_name(argv[5]);
}

// Expecting:	ALI_CALL_FUNCTION <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
//				<function-name-or-arn> <function-argument-blob>
bool AlicloudCallFunction::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 7 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] ) &&
		verify_string_name( argv[5] ) &&
		verify_string_name( argv[6] );
}

// Expecting:	EC2_BULK_QUERY <req_id>
//				<service_url> <accesskeyfile> <secretkeyfile>
bool AlicloudBulkQuery::ioCheck(char **argv, int argc) {
	return verify_min_number_args( argc, 5 ) &&
		verify_request_id( argv[1] ) &&
		verify_string_name( argv[2] ) &&
		verify_string_name( argv[3] ) &&
		verify_string_name( argv[4] );
}
