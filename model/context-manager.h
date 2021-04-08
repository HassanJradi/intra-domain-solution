/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef CONTEXT_MANAGER_H
#define CONTEXT_MANAGER_H

#include <map>
#include "ns3/object.h"
#include "ns3/ipv6-header.h"
#include "utils.h"

using namespace std;

namespace ns3
{

	enum DatabaseMode : uint8_t;
	enum FieldID : uint8_t
	{
		IPv6_DiffServ,
		IPv6_FlowLabel,
		IPv6_PayloadLength,
		IPv6_NextHeader,
		IPv6_HopLimit,
		IPv6_SourcePrefix,
		IPv6_SourceHost,
		IPv6_DestinationPrefix,
		IPv6_DestinationHost
	};
	enum DirectionIndicator : uint8_t
	{
		Up,
		Dw,
		Bi
	};
	enum MatchingOperator : uint8_t
	{
		Equal,
		Ignore,
		MSB,
		MatchMapping
	};
	enum CompressionDecompressionAction : uint8_t
	{
		NotSent,
		ValueSent,
		LSB,
		MappingSent,
		Compute,
		SourceHost,
		DestinationHost
	};

	struct RuleField
	{
		FieldID fieldID;
		uint8_t fieldPosition;
		DirectionIndicator directionIndicator;
		string targetValue;
		MatchingOperator matchingOperator;
		CompressionDecompressionAction compressionDecompressionAction;

		uint16_t GetSize();
		static uint8_t *Serialize(RuleField *ruleField, uint16_t &length);
		static RuleField *Deserialize(uint8_t *serializedRuleField, uint16_t length);
	};

	struct Rule
	{
		vector<RuleField *> ruleFields;

		uint16_t GetSize();
		static uint8_t *Serialize(Rule *rule, uint16_t &length);
		static Rule *Deserialize(uint8_t *serializedRule, uint16_t length);
	};

	struct Context
	{
		uint64_t ID;				//set by the network or the device and not serialized
		map<uint8_t, Rule *> rules; //incremental ruleID

		uint16_t GetSize();
		static uint8_t *Serialize(Context *context, uint16_t &length);
		static Context *Deserialize(uint8_t *serializedContext, uint16_t length);
	};

	class ContextManager : public Object
	{
	public:
		static TypeId GetTypeId(void);
		ContextManager();

		Context *GetContext();
		Context *CreateContext(Ipv6Header *header);
		bool IsContextExist(uint64_t ID); //change here if it should be independant of L3 address
		void SwitchSourceDestinationRuleFields(Context *context);
		void DispatchContext(Context *context, DatabaseMode databaseMode);

	private:
		vector<Context *> contexts;
		Context *currentContext;

	protected:
		virtual void DoDispose(void);
	};
}

#endif /* CONTEXT_MANAGER_H */