/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/net-device.h"

#include "dchc-helper.h"

namespace ns3 {

	NS_LOG_COMPONENT_DEFINE("DCHCHelper");

	DCHCHelper::DCHCHelper()
	{
		NS_LOG_FUNCTION(this);
		m_deviceFactory.SetTypeId("ns3::DCHCNetDevice");
	}

	void DCHCHelper::SetDeviceAttribute(std::string n1, const AttributeValue& v1)
	{
		NS_LOG_FUNCTION(this);
		m_deviceFactory.Set(n1, v1);
	}

	NetDeviceContainer DCHCHelper::Install(const NetDeviceContainer c)
	{
		NS_LOG_FUNCTION(this);

		NetDeviceContainer devs;

		for (uint32_t i = 0; i < c.GetN(); ++i)
		{
			Ptr<NetDevice> device = c.Get(i);
			NS_ASSERT_MSG(device != 0, "No NetDevice found in the node " << int(i));

			Ptr<Node> node = device->GetNode();
			NS_LOG_LOGIC("**** Install DCHC on node " << node->GetId());

			Ptr<DCHCNetDevice> dev = m_deviceFactory.Create<DCHCNetDevice>();

			devs.Add(dev);
			node->AddDevice(dev);
			dev->SetNetDevice(device);
		}

		return devs;
	}
}