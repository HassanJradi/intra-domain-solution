/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef COMPRESSED_HEADER_H
#define COMPRESSED_HEADER_H

#include "ns3/header.h"
#include "utils.h"

using namespace std;

namespace ns3
{

	class CompressedHeader : public Header
	{
	public:
		uint8_t ruleID;

		string diffServResidue;
		string flowLabelResidue;
		string payloadLengthResidue;
		string nextHeaderResidue;
		string hopLimitResidue;
		string sourcePrefixResidue;
		string sourceHostResidue;
		string destinationPrefixResidue;
		string destinationHostResidue;

		static TypeId GetTypeId(void);
		virtual TypeId GetInstanceTypeId(void) const;
		CompressedHeader();
		~CompressedHeader();

		virtual uint32_t GetSerializedSize(void) const;
		virtual void Serialize(Buffer::Iterator start) const;
		virtual uint32_t Deserialize(Buffer::Iterator start);
		virtual void Print(std::ostream &os) const;
	};

}

#endif /* COMPRESSED_HEADER_H */