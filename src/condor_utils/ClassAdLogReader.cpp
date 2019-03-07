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

#ifdef _NO_CONDOR_
#include <assert.h> // for assert
#include <errno.h> // for errno
#include <syslog.h> // for syslog, LOG_ERR
#else
#include "condor_common.h"
#include "condor_debug.h"
#endif

#include "ClassAdLogReader.h"


class FileSentry
{
public:
	FileSentry(FILE *fp) : m_fp(fp) {}
	~FileSentry() {if (m_fp) fclose(m_fp);}
private:
	FILE *m_fp;
};


ClassAdLogIterator::ClassAdLogIterator(const std::string &fname) :
	m_parser(new ClassAdLogParser()),
	m_prober(new ClassAdLogProber()),
	m_fname(fname),
	m_eof(true)
{
	m_parser->setJobQueueName(fname.c_str());
	Next();
}


ClassAdLogIterator
ClassAdLogIterator::operator++()
{
	Next();
	return *this;
}


ClassAdLogIterator
ClassAdLogIterator::operator++(int)
{
	ClassAdLogIterator result = *this;
	Next();
	return result;
}


bool
ClassAdLogIterator::operator==(const ClassAdLogIterator &rhs)
{
	if (m_current.get() == rhs.m_current.get())
	{
		return true;
	}
	if (!m_current.get() || !rhs.m_current.get())
	{
		return false;
	}
	if (m_current->isDone() && rhs.m_current->isDone())
	{
		return true;
	}
	if (m_fname != rhs.m_fname)
	{
		return false;
	}
	if (m_prober->getCurProbedSequenceNumber() != rhs.m_prober->getCurProbedSequenceNumber())
	{
		return false;
	}
	if (m_prober->getCurProbedCreationTime() != rhs.m_prober->getCurProbedCreationTime())
	{
		return false;
	}
	return true;
}


void
ClassAdLogIterator::Next()
{
	//printf("Calling next\n");
	ProbeResultType probe_st;
	FileOpErrCode fst;

	if (!m_eof || (m_current.get() && m_current->getEntryType() == ClassAdLogIterEntry::ET_INIT))
	{
		Load();
		if (m_eof)
		{
			//printf("Update probe info.\n");
			m_prober->incrementProbeInfo();
		}
		return;
	}
	m_eof = true;

	if (m_parser->getFilePointer() == NULL)
	{
		//printf("Re-opening file with parser %p.\n", m_parser.get());
		fst = m_parser->openFile();
		if(fst == FILE_OPEN_ERROR) {
			dprintf(D_ALWAYS, "Failed to open %s: errno=%d\n", m_parser->getJobQueueName(), errno);
			m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_ERR));
			return;
		}
		//m_sentry.reset(new FileSentry(m_parser->getFilePointer()));
	}

	//printf("Calling probe with file pointer %p.\n", m_parser->getFilePointer());
	probe_st = m_prober->probe(m_parser->getLastCALogEntry(), m_parser->getFilePointer());

	bool success = true;
	switch (probe_st)
	{
		case INIT_QUILL:
			//printf("Other.\n");
			m_parser->setNextOffset(0);
			m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_INIT));
			return;
		case COMPRESSED:
		case PROBE_ERROR:
			m_parser->setNextOffset(0);
			m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_RESET));
			//printf("Error.\n");
			return;
		case ADDITION:
			//printf("Addition.\n");
			success = Load();
			return;
		case NO_CHANGE:
			//printf("No change.\n");
			m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_NOCHANGE));
			break;
		case PROBE_FATAL_ERROR:
			//printf("Fatal error.\n");
			m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_ERR));
			return;
	}
	m_parser->closeFile();

	if (success)
	{
		// update prober to most recent observations about the job log file
		//printf("Update probe info.\n");
		m_prober->incrementProbeInfo();
		//printf("Update probe info done.\n");
	}

	return;
}


bool
ClassAdLogIterator::Load()
{
	FileOpErrCode err;
	m_eof = false;
	do {
		int op_type = 999;

		err = m_parser->readLogEntry(op_type);
		if (err == FILE_READ_SUCCESS)
		{
			if (Process(*m_parser->getCurCALogEntry())) {return true;}
		}
	} while (err == FILE_READ_SUCCESS);
	if (err != FILE_READ_EOF)
	{
		dprintf(D_ALWAYS, "error reading from %s: %d, %d\n", m_fname.c_str(), err, errno);
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_ERR));
	}
	else
	{
		//printf("Hit EOF.\n");
		m_parser->closeFile();
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_NOCHANGE));
		m_eof = true;
	}
	return true;
}


bool
ClassAdLogIterator::Process(const ClassAdLogEntry &log_entry)
{
	switch(log_entry.op_type) {
	case CondorLogOp_NewClassAd:
		//printf("New classad\n");
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::NEW_CLASSAD));
		if (log_entry.key) {m_current->setKey(log_entry.key);}
		if (log_entry.mytype) {m_current->setAdType(log_entry.mytype);}
		if (log_entry.targettype) {m_current->setAdTarget(log_entry.targettype);}
		break;
	case CondorLogOp_DestroyClassAd:
		//printf("Destroy classad\n");
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::DESTROY_CLASSAD));
		if (log_entry.key) {m_current->setKey(log_entry.key);}
                break;
	case CondorLogOp_SetAttribute:
		//printf("Set attribute\n");
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::SET_ATTRIBUTE));
		if (log_entry.key) {m_current->setKey(log_entry.key);}
		if (log_entry.name) {m_current->setName(log_entry.name);}
		if (log_entry.value) {m_current->setValue(log_entry.value);}
		break;
	case CondorLogOp_DeleteAttribute:
		//printf("Delete attribute\n");
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::DELETE_ATTRIBUTE));
		if (log_entry.key) {m_current->setKey(log_entry.key);}
		if (log_entry.name) {m_current->setName(log_entry.name);}
		break;
	case CondorLogOp_BeginTransaction:
	case CondorLogOp_EndTransaction:
	case CondorLogOp_LogHistoricalSequenceNumber:
		return false;
	default:
		dprintf(D_ALWAYS, "error reading %s: Unsupported Job Queue Command\n", m_fname.c_str());
		m_current.reset(new ClassAdLogIterEntry(ClassAdLogIterEntry::ET_ERR));
	}
	return true;
}

ClassAdLogReader::ClassAdLogReader(ClassAdLogConsumer *consumer):
	m_consumer(consumer)
{
	m_consumer->SetClassAdLogReader(this);
}

ClassAdLogReader::~ClassAdLogReader()
{
	if (m_consumer) {
		delete m_consumer;
		m_consumer = NULL;
	}
}

void
ClassAdLogReader::SetClassAdLogFileName(char const *fname)
{
	parser.setJobQueueName(fname);
}


char const *
ClassAdLogReader::GetClassAdLogFileName()
{
	return parser.getJobQueueName();
}

PollResultType
ClassAdLogReader::Poll() {
	ProbeResultType probe_st;
	FileOpErrCode fst;

	fst = parser.openFile();
	if(fst == FILE_OPEN_ERROR) {
#ifdef _NO_CONDOR_
		syslog(LOG_ERR,
			   "Failed to open %s: errno=%d (%m)",
			   parser.getJobQueueName(), errno);
#else
		dprintf(D_ALWAYS,"Failed to open %s: errno=%d\n",parser.getJobQueueName(),errno);
#endif
		return POLL_FAIL;
	}

	probe_st = prober.probe(parser.getLastCALogEntry(),parser.getFilePointer());

	bool success = true;
	switch(probe_st) {
	case INIT_QUILL:
	case COMPRESSED:
	case PROBE_ERROR:
		success = BulkLoad();
		break;
	case ADDITION:
		success = IncrementalLoad();
		break;
	case NO_CHANGE:
		break;
	case PROBE_FATAL_ERROR:
		return POLL_ERROR;
	}

	parser.closeFile();

	if(success) {
		// update prober to most recent observations about the job log file
		prober.incrementProbeInfo();
	}

	return POLL_SUCCESS;
}


bool
ClassAdLogReader::BulkLoad()
{
	parser.setNextOffset(0);
	m_consumer->Reset();
	return IncrementalLoad();
}


bool
ClassAdLogReader::IncrementalLoad()
{
	FileOpErrCode err;
	do {
		int op_type = -1;

		err = parser.readLogEntry(op_type);
		assert(err != FILE_FATAL_ERROR); // XXX
		if (err == FILE_READ_SUCCESS) {
			bool processed = ProcessLogEntry(parser.getCurCALogEntry(), &parser);
			if(!processed) {
#ifdef _NO_CONDOR_
				syslog(LOG_ERR,
					   "error reading %s: Failed to process log entry.",
					   GetClassAdLogFileName());
#else
				dprintf(D_ALWAYS, "error reading %s: Failed to process log entry.\n",GetClassAdLogFileName());
#endif
				return false;
			}
		}
	}while(err == FILE_READ_SUCCESS);
	if (err != FILE_READ_EOF) {
#ifdef _NO_CONDOR_
		syslog(LOG_ERR,
			   "error reading from %s: %d, errno=%d",
			   GetClassAdLogFileName(), err, errno);
#else
		dprintf(D_ALWAYS, "error reading from %s: %d, %d\n",GetClassAdLogFileName(),err,errno);
#endif
		return false;
	}
	return true;
}


/*! read the body of a log Entry.
 */
bool
ClassAdLogReader::ProcessLogEntry(ClassAdLogEntry *log_entry,
							  ClassAdLogParser * /*caLogParser*/)
{

	switch(log_entry->op_type) {
	case CondorLogOp_NewClassAd:
		return m_consumer->NewClassAd(log_entry->key,
									  log_entry->mytype,
									  log_entry->targettype);
	case CondorLogOp_DestroyClassAd:
		return m_consumer->DestroyClassAd(log_entry->key);
	case CondorLogOp_SetAttribute:
		return m_consumer->SetAttribute(log_entry->key,
										log_entry->name,
										log_entry->value);
	case CondorLogOp_DeleteAttribute:
		return m_consumer->DeleteAttribute(log_entry->key,
										   log_entry->name);
 	case CondorLogOp_BeginTransaction:
		// Transactions may be ignored, because the transaction will either
		// be completed eventually, or the log writer will backtrack
		// and wipe out the transaction, which will cause us to do a
		// bulk reload.
		break;
	case CondorLogOp_EndTransaction:
		break;
	case CondorLogOp_LogHistoricalSequenceNumber:
		break;
	default:
#ifdef _NO_CONDOR_
		syslog(LOG_ERR,
			   "error reading %s: Unsupported Job Queue Command",
			   GetClassAdLogFileName());
#else
		dprintf(D_ALWAYS, "error reading %s: Unsupported Job Queue Command\n",GetClassAdLogFileName());
#endif
		return false;
	}
	return true;
}
