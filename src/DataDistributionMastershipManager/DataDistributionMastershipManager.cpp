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

#include "DataDistributionMastershipManager.h"

extern "C" DDM_EXPORT void* CreateObjectImplementation()
{
	return static_cast<void*> (new DataDistributionMastershipManager);
}

DataDistributionMastershipManager::DataDistributionMastershipManager() : DataDistributionMastershipCommon()
{
	m_csFlags = new DataDistributionLockWrapper();
}

DataDistributionMastershipManager::~DataDistributionMastershipManager()
{
	delete m_csFlags;
}

int64_t* DataDistributionMastershipManager::GetClusterIndexes(size_t* length)
{
	DataDistributionAutoLockWrapper lock(m_csFlags);

	ClusterHealthIterator it;

	*length = clusterState.size();
	int64_t* arraElements = (int64_t*)malloc(sizeof(int64_t) * (*length));
	size_t counter = 0;
	for (it = clusterState.begin(); it != clusterState.end(); ++it)
	{
		arraElements[counter] = it->first;
		counter++;
	}
	return arraElements;
}

DDM_INSTANCE_STATE DataDistributionMastershipManager::GetStateOf(int64_t serverId)
{
	DataDistributionAutoLockWrapper lock(m_csFlags);
	DDM_INSTANCE_STATE state = DDM_INSTANCE_STATE::UNKNOWN;
	ClusterHealthIterator it;
	auto elem = clusterState.at(serverId);
	if (elem != NULL)
	{
		state = elem->Status;
	}

	return state;
}

ClusterHealth DataDistributionMastershipManager::GetClusterHealth()
{
	return clusterState;
}
