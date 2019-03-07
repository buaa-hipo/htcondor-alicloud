#!/bin/bash

PORT=$1; shift;
ECS_URL=http://localhost:${PORT}

# CreateKeyPair.
curl -i ${ECS_URL}'/?Action=CreateKeyPair&ALIAccessKeyId=1&KeyName=kn-1'; echo

# DescribeKeyPairs.
curl -i ${ECS_URL}'/?Action=DescribeKeyPairs&ALIAccessKeyId=1'; echo

# Minimal RunInstances command.
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1'; echo

# Individual options.
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1&KeyName=kn-1'; echo
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1&InstanceType=mx.example'; echo
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1&SecurityGroup.1=sg-name-1'; echo
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1&SecurityGroup.1=sg-name-2&SecurityGroup.2=sg-name-3'; echo
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1&UserData=somethingbase64encoded'; echo

# All options.
curl -i ${ECS_URL}'/?Action=RunInstances&ALIAccessKeyId=1&MaxCount=1&MinCount=1&ImageId=1'; echo

# DescribeInstances.
curl -i ${ECS_URL}'/?Action=DescribeInstances&ALIAccessKeyId=1'; echo

# TerminateInstances.
curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000000'; echo

curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000001'; echo
curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000002'; echo
curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000003'; echo
curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000004'; echo
curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000005'; echo

curl -i ${ECS_URL}'/?Action=TerminateInstances&ALIAccessKeyId=1&InstanceId.1=i-00000006'; echo

# DeleteKeyPair
curl -i ${ECS_URL}'/?Action=DeleteKeyPair&ALIAccessKeyId=1&KeyName=kn-1'; echo
