/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"

#include "context-manager.h"
#include "dchc-net-device.h"

NS_LOG_COMPONENT_DEFINE("ContextManager");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED(ContextManager);

	/* Context Manager Functions */

	TypeId ContextManager::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::ContextManager")
								.SetParent<Object>()
								.SetGroupName("DCHC")
								.AddConstructor<ContextManager>();

		return tid;
	}

	ContextManager::ContextManager()
	{
		NS_LOG_FUNCTION(this);
		currentContext = NULL;
	}

	Context *ContextManager::GetContext()
	{
		NS_LOG_FUNCTION(this);
		return currentContext;
	}

	Context *ContextManager::CreateContext(Ipv6Header *header)
	{
		NS_LOG_FUNCTION(this);

		Context *context = new Context();
		uint8_t buf[16];

		context->rules.insert(pair<uint8_t, Rule *>(0, new Rule()));

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_DiffServ,
			0,
			Bi,
			to_string(header->GetTrafficClass()),
			Equal,
			NotSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_FlowLabel,
			0,
			Bi,
			to_string(header->GetFlowLabel()),
			Equal,
			NotSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_PayloadLength,
			0,
			Bi,
			to_string(header->GetPayloadLength()),
			Ignore,
			ValueSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_NextHeader,
			0,
			Bi,
			to_string(header->GetNextHeader()),
			Equal,
			NotSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_HopLimit,
			0,
			Bi,
			to_string(header->GetHopLimit()),
			Equal,
			NotSent});

		header->GetSourceAddress().GetBytes(buf);

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_SourcePrefix,
			0,
			Bi,
			ConvertPointerUint8ToString(buf, 8),
			Equal,
			NotSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_SourceHost,
			0,
			Bi,
			ConvertPointerUint8ToString(buf + 8, 8),
			Equal,
			NotSent});

		header->GetDestinationAddress().GetBytes(buf);

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_DestinationPrefix,
			0,
			Bi,
			ConvertPointerUint8ToString(buf, 8),
			Equal,
			NotSent});

		context->rules[0]->ruleFields.push_back(new RuleField{
			IPv6_DestinationHost,
			0,
			Bi,
			ConvertPointerUint8ToString(buf + 8, 8),
			Equal,
			NotSent});

		return context;
	}

	bool ContextManager::IsContextExist(uint64_t ID)
	{
		NS_LOG_FUNCTION(this);

		for (uint8_t i = 0; i < contexts.size(); i++)
		{
			if (contexts[i]->ID == ID)
			{
				currentContext = contexts[i];
				return true;
			}
		}

		return false;
	}

	void ContextManager::SwitchSourceDestinationRuleFields(Context *context)
	{
		//NS_LOG_FUNCTION(this);

		for (auto it = context->rules.begin(); it != context->rules.end(); ++it)
		{
			Rule *rule = it->second;

			for (uint16_t i = 0; i < rule->ruleFields.size(); i++)
			{
				switch (rule->ruleFields[i]->fieldID)
				{

				case IPv6_SourcePrefix:
				{
					rule->ruleFields[i]->fieldID = IPv6_DestinationPrefix;
					break;
				}

				case IPv6_SourceHost:
				{
					rule->ruleFields[i]->fieldID = IPv6_DestinationHost;
					break;
				}

				case IPv6_DestinationPrefix:
				{
					rule->ruleFields[i]->fieldID = IPv6_SourcePrefix;
					break;
				}

				case IPv6_DestinationHost:
				{
					rule->ruleFields[i]->fieldID = IPv6_SourceHost;
					break;
				}

				default:
					break;
				}
			}
		}
	}

	void ContextManager::DispatchContext(Context *context, DatabaseMode databaseMode)
	{
		NS_LOG_FUNCTION(this << context);

		switch (databaseMode)
		{
		case Light:
		{
			contexts.clear();
			contexts.push_back(currentContext);
			break;
		}

		case Unlimited:
		{
			contexts.push_back(currentContext);
			break;
		}
		}

		currentContext = context;
	}

	void ContextManager::DoDispose()
	{
		NS_LOG_FUNCTION(this);

		contexts.clear();
		currentContext = NULL;

		Object::DoDispose();
	}

	/* Serialize-Deserialize Functions */

	uint16_t RuleField::GetSize()
	{
		return 5 + this->targetValue.size();
	}

	uint8_t *RuleField::Serialize(RuleField *ruleField, uint16_t &length)
	{
		length = ruleField->GetSize();
		uint8_t *serializedRuleField = new uint8_t[length];

		serializedRuleField[0] = ruleField->fieldID;
		serializedRuleField[1] = ruleField->fieldPosition;
		serializedRuleField[2] = ruleField->directionIndicator;
		serializedRuleField[3] = ruleField->matchingOperator;
		serializedRuleField[4] = ruleField->compressionDecompressionAction;

		for (uint16_t i = 0; i < length - 5; i++)
		{
			serializedRuleField[5 + i] = static_cast<uint8_t>(ruleField->targetValue[i]);
		}

		return serializedRuleField;
	}

	RuleField *RuleField::Deserialize(uint8_t *serializedRuleField, uint16_t length)
	{
		RuleField *ruleField = new RuleField();

		ruleField->fieldID = (FieldID)serializedRuleField[0];
		ruleField->fieldPosition = serializedRuleField[1];
		ruleField->directionIndicator = (DirectionIndicator)serializedRuleField[2];
		ruleField->matchingOperator = (MatchingOperator)serializedRuleField[3];
		ruleField->compressionDecompressionAction = (CompressionDecompressionAction)serializedRuleField[4];

		for (uint16_t i = 0; i < length - 5; i++)
		{
			ruleField->targetValue += static_cast<char>(serializedRuleField[5 + i]);
		}

		return ruleField;
	}

	uint16_t Rule::GetSize()
	{
		uint16_t size = 0;

		for (uint16_t i = 0; i < this->ruleFields.size(); i++)
		{
			size += this->ruleFields[i]->GetSize();
		}

		return size + this->ruleFields.size();
	}

	uint8_t *Rule::Serialize(Rule *rule, uint16_t &length)
	{
		length = rule->GetSize();
		uint8_t *serializedRule = new uint8_t[length];

		for (uint16_t i = 0; i < rule->ruleFields.size(); i++)
		{
			*serializedRule = rule->ruleFields[i]->GetSize();
			serializedRule++;
			uint16_t c_length;
			uint8_t *serializedRuleField = RuleField::Serialize(rule->ruleFields[i], c_length);
			memcpy(serializedRule, serializedRuleField, c_length);
			serializedRule = serializedRule + c_length;
		}

		return serializedRule - length;
	}

	Rule *Rule::Deserialize(uint8_t *serializedRule, uint16_t length)
	{
		Rule *rule = new Rule();
		uint16_t read_length = 0;

		do
		{
			uint16_t c_length = *serializedRule;
			serializedRule++;
			rule->ruleFields.push_back(RuleField::Deserialize(serializedRule, c_length));
			serializedRule = serializedRule + c_length;
			read_length += c_length + 1;

		} while (read_length < length);

		return rule;
	}

	uint16_t Context::GetSize()
	{
		uint16_t size = 0;

		for (uint16_t i = 0; i < this->rules.size(); i++)
		{
			size += this->rules[i]->GetSize();
		}

		return size + 2 * this->rules.size();
	}

	uint8_t *Context::Serialize(Context *context, uint16_t &length)
	{
		length = context->GetSize();
		uint8_t *serializedContext = new uint8_t[length];

		for (auto it = context->rules.begin(); it != context->rules.end(); ++it)
		{
			Rule *rule = it->second;
			*serializedContext = rule->GetSize() + 1;
			serializedContext++;
			*serializedContext = it->first;
			serializedContext++;
			uint16_t c_length;
			uint8_t *serializedRule = Rule::Serialize(rule, c_length);
			memcpy(serializedContext, serializedRule, c_length);
			serializedContext = serializedContext + c_length;
		}

		return serializedContext - length;
	}

	Context *Context::Deserialize(uint8_t *serializedContext, uint16_t length)
	{
		Context *context = new Context();
		uint16_t read_length = 0;

		do
		{
			uint16_t c_length = *serializedContext;
			serializedContext++;
			uint8_t ruleID = *serializedContext;
			serializedContext++;
			context->rules.insert(pair<uint8_t, Rule *>(ruleID, Rule::Deserialize(serializedContext, c_length - 1)));
			serializedContext = serializedContext + c_length - 1;
			read_length += c_length + 1;
		} while (read_length < length);

		return context;
	}
}