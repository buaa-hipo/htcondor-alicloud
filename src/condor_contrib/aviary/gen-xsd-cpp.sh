#!/bin/sh

#
# Copyright 2009-2011 Red Hat, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# codegen to provide WSDL/XSD CPP headers and source

WSFCPP_HOME=/usr
if [ -z "$1" ]; then
    echo No arg - using default
else
    WSFCPP_HOME=$1
fi
echo WSFCPP_HOME=$WSFCPP_HOME

# generate our cpp types from WSDL
WSDL2CPP.sh -uri etc/aviary-job.wsdl -or -d adb -ss -g -ns2p http://common.aviary.grid.redhat.com=AviaryCommon,http://job.aviary.grid.redhat.com=AviaryJob -o codegen/job
WSDL2CPP.sh -uri etc/aviary-query.wsdl -or -d adb -ss -g -ns2p http://common.aviary.grid.redhat.com=AviaryCommon,http://query.aviary.grid.redhat.com=AviaryQuery -o codegen/query
WSDL2CPP.sh -uri etc/aviary-locator.wsdl -or -d adb -ss -g -ns2p http://common.aviary.grid.redhat.com=AviaryCommon,http://locator.aviary.grid.redhat.com=AviaryLocator -o codegen/locator
WSDL2CPP.sh -uri etc/aviary-collector.wsdl -or -d adb -ss -g -ns2p http://common.aviary.grid.redhat.com=AviaryCommon,http://collector.aviary.grid.redhat.com=AviaryCollector -o codegen/collector
WSDL2CPP.sh -uri etc/aviary-hadoop.wsdl -or -d adb -ss -g -ns2p http://common.aviary.grid.redhat.com=AviaryCommon,http://hadoop.aviary.grid.redhat.com=AviaryHadoop -o codegen/hadoop


# get rid of the extraneous stuff that WSDL2CPP won't let us turn off
rm -f codegen/job/*AviaryJob*Service*.{h,cpp,vcproj}
rm -f codegen/query/*AviaryQuery*Service*.{h,cpp,vcproj}
rm -f codegen/locator/*AviaryLocator*Service*.{h,cpp,vcproj}
rm -f codegen/collector/*AviaryCollector*Service*.{h,cpp,vcproj}
rm -f codegen/locator/*AviaryHadoop*Service*.{h,cpp,vcproj}


# setup our include dir
if ! test -d include; then
    mkdir include;
fi

# stow the headers for others steps in the build 
mv codegen/job/src/*.h include;
mv codegen/query/src/*.h include;
mv codegen/locator/src/*.h include;
mv codegen/collector/src/*.h include;
mv codegen/hadoop/src/*.h include;

# WSDLCPP should do this for us but break out common
if ! test -d codegen/common/src; then
    mkdir -p codegen/common/src;
fi
mv codegen/query/src/AviaryCommon*.cpp codegen/common/src
rm -f codegen/job/src/AviaryCommon*.cpp
rm -f codegen/locator/src/AviaryCommon*.cpp
rm -f codegen/collector/src/AviaryCommon*.cpp
rm -f codegen/hadoop/src/AviaryCommon*.cpp
