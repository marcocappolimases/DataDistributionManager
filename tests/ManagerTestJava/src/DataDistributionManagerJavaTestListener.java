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

import org.mases.datadistributionmanager.*;
import org.mases.datadistributionmanager.configuration.OpenDDSConfiguration.DCPSInfoRepoConfiguration;
import org.mases.datadistributionmanager.configuration.OpenDDSConfiguration.OpenDDSArgsConfiguration;
import org.mases.datadistributionmanager.configuration.*;

public class DataDistributionManagerJavaTestListener {
	public static void main(String args[]) {
		final int THRESHOLD = 10;

		DDM_CHANNEL_DIRECTION direction = DDM_CHANNEL_DIRECTION.RECEIVER;
		SmartDataDistribution dataDistribution = new SmartDataDistribution();
		dataDistribution.addListener(new LoggingListener() {
			public void OnLogging(DDM_LOG_LEVEL level, String source, String function, String logStr) {
				System.out.println(logStr);
			}
		});
		HRESULT hRes = HRESULT.S_OK;
		OpenDDSConfiguration conf = new OpenDDSConfiguration();
		if (args.length == 0) {
			OpenDDSArgsConfiguration argsConf = conf.new OpenDDSArgsConfiguration();
			// set the full command line
			// argsConf.setCommandLine("-DCPSConfigFile dds_tcp_conf.ini
			// -DCPSTransportDebugLevel 10");
			argsConf.setDCPSConfigFile("dds_tcp_conf.ini");
			argsConf.setDCPSTransportDebugLevel(10);
			conf.setOpenDDSArgs(argsConf);
			DCPSInfoRepoConfiguration infoRepo = conf.new DCPSInfoRepoConfiguration(true,
					"-ORBEndpoint iiop://localhost:12345");
			conf.setDCPSInfoRepo(infoRepo);
			DomainParticipantQosConfiguration domainPartQos = new DomainParticipantQosConfiguration();
			domainPartQos.EntityFactoryQosPolicy = new EntityFactoryQosPolicyConfiguration();
			domainPartQos.EntityFactoryQosPolicy.setAutoenableCreatedEntities(true);
			domainPartQos.UserDataQosPolicy = new UserDataQosPolicyConfiguration();
			domainPartQos.UserDataQosPolicy.setValue(new Byte[] { 102, 105 });
			domainPartQos.PropertyQosPolicy = new PropertyQosPolicyConfiguration();
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_IDENTITY_CA = domainPartQos.PropertyQosPolicy.new Property(
					"ciao", false);
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_IDENTITY_CERT = domainPartQos.PropertyQosPolicy.new Property(
					"ciao", false);
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_IDENTITY_PRIVKEY = domainPartQos.PropertyQosPolicy.new Property(
					"ciao", false);
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_PERM_CA = domainPartQos.PropertyQosPolicy.new Property("ciao",
					false);
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_PERM_DOC = domainPartQos.PropertyQosPolicy.new Property("ciao",
					false);
			domainPartQos.PropertyQosPolicy.DDSSEC_PROP_PERM_GOV_DOC = domainPartQos.PropertyQosPolicy.new Property(
					"ciao", false);
			conf.setDomainParticipantQos(domainPartQos);
			String[] confRes = conf.getConfiguration();
			hRes = dataDistribution.Initialize(conf);
		} else {
			hRes = dataDistribution.Initialize(args[0]);
		}

		if (hRes.getFailed()) {
			System.out.println("Error in configuration.");
			return;
		}

		hRes = dataDistribution.Start(10000);

		if (hRes.getFailed()) {
			return;
		}
		OpenDDSChannelConfiguration channelConf = new OpenDDSChannelConfiguration(conf);
		String[] channelConfRes = channelConf.getConfiguration();
		SmartDataDistributionChannel mytestTopic;
		try {
			mytestTopic = dataDistribution.CreateSmartChannel(SmartDataDistributionChannel.class, "test");
		} catch (Throwable e) {
			e.printStackTrace();
			return;
		}

		mytestTopic.addListener(new DataAvailableListener() {
			public void OnDataAvailable(String channelName, String key, byte[] buffer) {
				String s = new String(buffer);
				System.out.println(String.format("Channel %s with key %s is saying %s", channelName, key, s));
			}
		});

		mytestTopic.addListener(new ConditionOrErrorListener() {
			public void OnConditionOrError(String channelName, DDM_UNDERLYING_ERROR_CONDITION errorCode, int nativeCode,
					String subSystemReason) {
				System.out.println(String.format("Channel %s with errorCode %s nativeCode %d subSystemReason %s",
						channelName, (errorCode != null) ? errorCode.name() : "", nativeCode, subSystemReason));
			}
		});

		System.out.println("After CeateSmartChannel...\n");

		mytestTopic.StartChannel(10000);

		try {
			System.out.println("Starting sending...\n");
			int counter = 100;
			String str = "test";
			while (true) {
				hRes = HRESULT.S_OK;
				if (direction == DDM_CHANNEL_DIRECTION.TRANSMITTER) {
					hRes = mytestTopic.WriteOnChannel(null, str);
				}
				if (hRes == HRESULT.S_OK) {
					str = String.format("%d", counter++);
					if ((counter % THRESHOLD) == 0)
						System.out.println(String.format("SendData Reached %d", counter));
				}
				Thread.sleep(1000);
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}