/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"

#include "compressed-header.h"
#include "dchc-net-device.h"

NS_LOG_COMPONENT_DEFINE("CompressedHeader");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED(CompressedHeader);

	/* Compressed Header Functions */

	TypeId CompressedHeader::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::CompressedHeader")
								.SetParent<Header>()
								.SetGroupName("DCHC")
								.AddConstructor<CompressedHeader>();

		return tid;
	}

	TypeId CompressedHeader::GetInstanceTypeId(void) const
	{
		NS_LOG_FUNCTION(this);
		return GetTypeId();
	}

	CompressedHeader::CompressedHeader()
	{
		NS_LOG_FUNCTION(this);
	}

	CompressedHeader::~CompressedHeader()
	{
		NS_LOG_FUNCTION(this);
	}

	uint32_t CompressedHeader::GetSerializedSize(void) const
	{
		return sizeof(ruleID) + 9 * sizeof(uint8_t) + diffServResidue.size() + flowLabelResidue.size() + payloadLengthResidue.size() + nextHeaderResidue.size() + hopLimitResidue.size() + sourcePrefixResidue.size() + sourceHostResidue.size() + destinationPrefixResidue.size() + destinationHostResidue.size();
	}

	void CompressedHeader::Serialize(Buffer::Iterator start) const
	{
		NS_LOG_FUNCTION(this);

		Buffer::Iterator iterator = start; //check if can be omitted

		iterator.WriteU8(ruleID);

		WriteString(&iterator, diffServResidue);
		WriteString(&iterator, flowLabelResidue);
		WriteString(&iterator, payloadLengthResidue);
		WriteString(&iterator, nextHeaderResidue);
		WriteString(&iterator, hopLimitResidue);
		WriteString(&iterator, sourcePrefixResidue);
		WriteString(&iterator, sourceHostResidue);
		WriteString(&iterator, destinationPrefixResidue);
		WriteString(&iterator, destinationHostResidue);
	}

	uint32_t CompressedHeader::Deserialize(Buffer::Iterator start)
	{
		NS_LOG_FUNCTION(this);

		Buffer::Iterator iterator = start; //check if can be omitted

		ruleID = iterator.ReadU8();

		ReadString(&iterator, &diffServResidue);
		ReadString(&iterator, &flowLabelResidue);
		ReadString(&iterator, &payloadLengthResidue);
		ReadString(&iterator, &nextHeaderResidue);
		ReadString(&iterator, &hopLimitResidue);
		ReadString(&iterator, &sourcePrefixResidue);
		ReadString(&iterator, &sourceHostResidue);
		ReadString(&iterator, &destinationPrefixResidue);
		ReadString(&iterator, &destinationHostResidue);

		return GetSerializedSize();
	}

	void CompressedHeader::Print(std::ostream &os) const
	{
		os << static_cast<int>(ruleID) << " "
		   << diffServResidue << " "
		   << flowLabelResidue << " "
		   << payloadLengthResidue << " "
		   << nextHeaderResidue << " "
		   << hopLimitResidue << " "
		   << sourcePrefixResidue << " "
		   << sourceHostResidue << " "
		   << destinationPrefixResidue << " "
		   << destinationHostResidue << " ";
	}

}