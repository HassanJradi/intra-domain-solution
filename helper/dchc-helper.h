/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DCHC_HELPER_H
#define DCHC_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/object-factory.h"
#include "ns3/dchc-net-device.h"

namespace ns3 {

	class DCHCHelper
	{
	public:
		DCHCHelper();
		void SetDeviceAttribute(std::string n1, const AttributeValue& v1);
		NetDeviceContainer Install(const NetDeviceContainer c);

	private:
		ObjectFactory m_deviceFactory;
	};

}

#endif /* DCHC_HELPER_H */