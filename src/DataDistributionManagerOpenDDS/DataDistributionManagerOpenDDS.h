/*
*  Copyright 2021 MASES s.r.l.
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  Refer to LICENSE for more information.
*/

#ifndef DATADISTRIBUTIONMANAGEROPENDDS_H
#define DATADISTRIBUTIONMANAGEROPENDDS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataDistributionManagerCommon.h"
#include "DataDistributionSchemaTypeSupportImpl.h"
#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/LocalObject.h>
#include <orbsvcs/Time_Utilities.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/SubscriberImpl.h>
#include <ace/Log_Msg_Callback.h>
#include <ace/Log_Record.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class ChannelConfigurationOpenDDS;

class DDM_EXPORT DataDistributionManagerOpenDDS : public DataDistributionCommon, public ACE_Log_Msg_Callback
{
public:
	DataDistributionManagerOpenDDS();
	virtual ~DataDistributionManagerOpenDDS(void);
	HRESULT Initialize();
	HANDLE CreateChannel(const char* channelName, IDataDistributionChannelCallback* dataCb, DDM_CHANNEL_DIRECTION direction = DDM_CHANNEL_DIRECTION::ALL, const char* arrayParams[] = NULL, int len = NULL);
	HRESULT StartChannel(HANDLE channelHandle, DWORD dwMilliseconds);
	HRESULT StopChannel(HANDLE channelHandle, DWORD dwMilliseconds);
	void SetParameter(HANDLE channelHandle, const char* paramName, const char* paramValue);
	const char* GetParameter(HANDLE channelHandle, const char* paramName);
	HRESULT Lock(HANDLE channelHandle, DWORD timeout);
	HRESULT Unlock(HANDLE channelHandle);
	HRESULT SeekChannel(HANDLE channelHandle, size_t position);
	HRESULT DeleteChannel(HANDLE channelHandle);
	HRESULT WriteOnChannel(HANDLE channelHandle, const char* key, size_t keyLen, void *param, size_t dataLen, const BOOL waitAll = FALSE, const int64_t timestamp = DDM_NO_TIMESTAMP);
	HRESULT ReadFromChannel(HANDLE channelHandle, int64_t offset, size_t *dataLen, void **param);
	HRESULT ChangeChannelDirection(HANDLE channelHandle, DDM_CHANNEL_DIRECTION direction);

	HRESULT Stop(DWORD milliseconds);

	int GetServerLostTimeout() { return m_ServerLostTimeout; };
	static DDM_UNDERLYING_ERROR_CONDITION OpenDDSErrorMapper(CORBA::Long code);
private:
	// ACE_Log_Msg_Callback api
	void log(ACE_Log_Record &log_record);
	// internal api
	HRESULT shutdown();
	TimeBase::TimeT get_timestamp();
	static DDS::Duration_t DurationFromMs(int ms);
	HRESULT conf_init(ChannelConfigurationOpenDDS* configuration, const char* arrayParams[], int len);
	HRESULT read_config_file(ChannelConfigurationOpenDDS* configuration, const char* arrayParams[], int len);
	void SetCmdLine(std::string cmdLine);
	HRESULT StartConsumerAndWait(ChannelConfigurationOpenDDS* pChannelConfiguration, DWORD dwMilliseconds);
	void StopConsumer(ChannelConfigurationOpenDDS* pChannelConfiguration);
	static DWORD __stdcall consumerHandler(void * argh);
	static DWORD __stdcall readDataFromInfoRepo(void * argh);
	HRESULT InitializeInfoRepo();
	void SetDomainParticipantQos(DDS::DomainParticipantQos* qos, const char* arrayParams[], int len);
	void SetTopicQos(DDS::TopicQos* qos, const char* arrayParams[], int len);
	void SetPublisherQos(DDS::PublisherQos* qos, const char* arrayParams[], int len);
	void SetSubscriberQos(DDS::SubscriberQos* qos, const char* arrayParams[], int len);
	void SetDataWriterQos(DDS::DataWriterQos* qos, const char* arrayParams[], int len);
	void SetDataReaderQos(DDS::DataReaderQos* qos, const char* arrayParams[], int len);
private:
	::DDS::DomainId_t m_domainId;
	DataDistributionSchema::OpenDDSMsgTypeSupport_var openddsmsg_servant;
	DDS::DomainParticipantFactory_var m_dpf;
	DDS::DomainParticipant_var m_participant;
	std::string	m_cmdLine;
	ACE_ARGV_T<char>* m_myArgs;
	int m_argc;
	char** m_argv;

	::DDS::DomainParticipantQos m_domain_partecipant_qos;

	::CORBA::Boolean m_bStartDCPSInfoRepo;
	::CORBA::Boolean m_bDCPSInfoRepoLogOnApplication;
	std::string	m_DCPSInfoRepoCmdLine;
#define BUFSIZE 4096 

	HANDLE m_hChildStd_OUT_Rd;
	HANDLE m_hChildStd_OUT_Wr;
	HANDLE m_hreadDataFromInfoRepo;

	int  m_ServerLostTimeout;

	std::vector<ChannelConfigurationOpenDDS*> channelVector;
};

class ChannelConfigurationOpenDDS : public ChannelConfiguration
{
public:
	ChannelConfigurationOpenDDS(const char* channelName, DDM_CHANNEL_DIRECTION direction, DataDistributionManagerOpenDDS* mainManager, IDataDistributionChannelCallback* Cb)
		: ChannelConfiguration(channelName, direction, mainManager, Cb)
	{
		h_evtConsumer = CreateEvent(0, true, false, NULL);
	}

	DDS::Topic_var channel_channel;
	DDS::Publisher_var publisher;

	DDS::DataWriterQos m_dw_qos;
	DDS::TopicQos m_channel_qos;
	DDS::PublisherQos m_publisher_qos;
	DDS::SubscriberQos m_subscriber_qos;

	DDS::DataWriter_var channel_base_dw;
	DataDistributionSchema::OpenDDSMsgDataWriter_var channel_dw;

	DDS::Subscriber_var subscriber;
	DDS::DataReaderQos m_dr_qos;

	DDS::DataReaderListener_var channel_listener;
	DDS::DataReader_var channel_base_dr;
	DataDistributionSchema::OpenDDSMsgDataReader_var channel_dr;

	HANDLE  h_evtConsumer;
	BOOL	bConsumerRun;
	DWORD	dwConsumerThrId;
	HANDLE	hConsumerThread;
};

#endif /* DATADISTRIBUTIONMANAGEROPENDDS_H  */
