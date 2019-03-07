/*
 * Copyright 2008 Red Hat, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "condor_common.h"

#include "ClassAdLogPlugin.h"

struct ExampleClassAdLogPlugin : public ClassAdLogPlugin
{
	void
	initialize()
	{
		printf("Init\n");
	}

	void
	shutdown()
	{
		printf("Shutdown\n");
	}

	void
	newClassAd(const char *key)
	{
		printf("newClassAd: %s\n", key);
	}

	void
	destroyClassAd(const char *key)
	{
		printf("destroyClassAd: %s\n", key);
	}

	void
	setAttribute(const char *key,
				 const char *name,
				 const char *value)
	{
		printf("setAttribute: %s[%s] = %s\n", key, name, value);
	}

	void
	deleteAttribute(const char *key,
					const char *name)
	{
		printf("deleteAttribute: %s[%s]\n", key, name);
	}
};

static ExampleClassAdLogPlugin instance;
