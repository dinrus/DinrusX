// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Optimize storage?

#pragma once

#include <DinrusX/DinrusXSys/IDrxLink.h>

#include <DinrusX/DinrusXSchema/DrxLinkUtils.h>
#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/IString.h>

namespace Schematyc
{
enum class ELogMetaField
{
	LinkCommand, // Link command associated with log message.
	ElementGUID, // GUID of element associated with log message.
	DetailGUID,  // GUID of detail associated with log message.
	EntityId,    // Id of entity associated with log message.
	Function,    // Name of function associated with log message.
	Count
};

class CLogMetaData
{
public:

	inline CLogMetaData()
		: m_linkCommand(DrxLinkUtils::ECommand::None)
		, m_entityId(INVALID_ENTITYID)
		, m_szFunction(nullptr)
	{}

	inline bool IsEmpty() const
	{
		return (m_linkCommand == DrxLinkUtils::ECommand::None) && GUID::IsEmpty(m_elementGUID) && GUID::IsEmpty(m_detailGUID) && (m_entityId == INVALID_ENTITYID) && !m_szFunction;
	}

	inline bool Set(ELogMetaField field, DrxLinkUtils::ECommand input)
	{
		switch (field)
		{
		case ELogMetaField::LinkCommand:
			{
				m_linkCommand = input;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Get(ELogMetaField field, DrxLinkUtils::ECommand& output) const
	{
		switch (field)
		{
		case ELogMetaField::LinkCommand:
			{
				output = m_linkCommand;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Set(ELogMetaField field, const DrxGUID& input)
	{
		switch (field)
		{
		case ELogMetaField::ElementGUID:
			{
				m_elementGUID = input;
				return true;
			}
		case ELogMetaField::DetailGUID:
			{
				m_detailGUID = input;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Get(ELogMetaField field, DrxGUID& output) const
	{
		switch (field)
		{
		case ELogMetaField::ElementGUID:
			{
				output = m_elementGUID;
				return true;
			}
		case ELogMetaField::DetailGUID:
			{
				output = m_detailGUID;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Set(ELogMetaField field, EntityId input)
	{
		switch (field)
		{
		case ELogMetaField::EntityId:
			{
				m_entityId = input;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	bool Get(ELogMetaField field, EntityId& output) const
	{
		switch (field)
		{
		case ELogMetaField::EntityId:
			{
				output = m_entityId;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Set(ELogMetaField field, const char* szInput)
	{
		switch (field)
		{
		case ELogMetaField::Function:
			{
				m_szFunction = szInput;
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool Get(ELogMetaField field, IString& output) const
	{
		switch (field)
		{
		case ELogMetaField::Function:
			{
				output.assign(m_szFunction);
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	inline bool CreateUri(IString& output) const
	{
		return DrxLinkUtils::CreateUri(output, m_linkCommand, m_elementGUID, m_detailGUID);
	}

private:

	DrxLinkUtils::ECommand m_linkCommand;
	DrxGUID                  m_elementGUID;
	DrxGUID                  m_detailGUID;
	EntityId               m_entityId;
	const char*            m_szFunction;
};
}
