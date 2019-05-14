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


#ifndef ECSRESOURCE_H
#define ECSRESOURCE_H

#include "condor_common.h"
#include "condor_daemon_core.h"

#include "ecsjob.h"
#include "baseresource.h"
#include "gahp-client.h"

#define ECS_RESOURCE_NAME "ecs"

class ECSJob;
class ECSResource;

class ECSResource : public BaseResource
{
public:
	void Reconfig();

	static const char *HashName( const char * resource_name,
								 const char * public_key_file,
								 const char * private_key_file );

	static ECSResource* FindOrCreateResource( const char * resource_name,
											  const char * public_key_file,
											  const char * private_key_file );

	ECSGahpClient *gahp;
	ECSGahpClient *status_gahp;

	ECSResource(const char * resource_name,
				const char * public_key_file,
				const char * private_key_file );

	~ECSResource();

	static HashTable <HashKey, ECSResource *> ResourcesByName;

	const char *ResourceType();

	const char *GetHashName();

	void PublishResourceAd( ClassAd *resource_ad );

    bool hadAuthFailure() { return m_hadAuthFailure; }

	bool ServerTypeQueried( ECSJob *job = NULL );
	bool ClientTokenWorks( ECSJob *job = NULL );
	bool ShuttingDownTrusted( ECSJob *job = NULL );

    std::string authFailureMessage;

	std::string m_serverType;

    BatchStatusResult StartBatchStatus();
    BatchStatusResult FinishBatchStatus();
    ECSGahpClient * BatchGahp() { return status_gahp; }

    HashTable< HashKey, ECSJob * > jobsByInstanceID;
    HashTable< HashKey, ECSJob * > spotJobsByRequestID;

private:
	void DoPing(unsigned & ping_delay,
				bool & ping_complete,
				bool & ping_succeeded  );

	char* m_public_key_file;
	char* m_private_key_file;

	bool m_hadAuthFailure;
	bool m_checkSpotNext;

public:
	class GahpStatistics {
		public:
			GahpStatistics();

			void Init();
			void Clear();
			void Advance();
			void Publish( ClassAd & ad ) const;
			void Unpublish( ClassAd & ad ) const;

			stats_entry_recent<int> NumRequests;
			stats_entry_recent<int> NumDistinctRequests;
			stats_entry_recent<int> NumRequestsExceedingLimit;
			stats_entry_recent<int> NumExpiredSignatures;

			StatisticsPool pool;
	};

private:
	GahpStatistics gs;
};
#endif
