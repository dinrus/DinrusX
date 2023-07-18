// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Move IObjectDump to separate header?

#pragma once

#include <DinrusX/CoreX/Math/Drx_Geo.h>
#include <DinrusX/CoreX/Serialization/Forward.h>

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/RuntimeParamMap.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>

// Forward declare structures.
struct SRendParams;
struct SRenderingPassInfo;
struct IEntity;

namespace Schematyc
{

// Forward declare interfaces.
struct IRuntimeClass;
// Forward declare structures.
struct STimerDuration;
// Forward declare classes.
class CAction;

enum class EObjectSimulationUpdatePolicy
{
	None = 0,
	OnChangeOnly, // Only set simulation if mode changes.
	Always,       // Always reset simulation.
};

struct SObjectSignal
{
public:

	inline SObjectSignal() {}

	explicit inline SObjectSignal(const DrxGUID& _typeGUID, const DrxGUID& _senderGUID = DrxGUID())
		: typeGUID(_typeGUID)
		, senderGUID(_senderGUID)
	{}

	inline SObjectSignal(const SObjectSignal& rhs)
		: typeGUID(rhs.typeGUID)
		, senderGUID(rhs.senderGUID)
		, params(rhs.params)
	{}

	template<typename SIGNAL> static inline SObjectSignal FromSignalClass(const SIGNAL& _signal, const DrxGUID& _senderGUID = DrxGUID())
	{
		return SObjectSignal(_signal, GetTypeDesc<SIGNAL>().GetGUID(), _senderGUID);
	}

private:

	template<typename SIGNAL> inline SObjectSignal(const SIGNAL& _signal, const DrxGUID& _typeGUID, const DrxGUID& _senderGUID)
		: typeGUID(_typeGUID)
		, senderGUID(_senderGUID)
	{
		RuntimeParamMap::FromInputClass(params, _signal);
	}

public:

	DrxGUID              typeGUID;
	DrxGUID              senderGUID;
	StackRuntimeParamMap params;
};

typedef std::function<EVisitStatus(const IEntityComponent&)> ObjectComponentConstVisitor;

enum class EObjectDumpFlags : uint32
{
	None      = 0x00000000,
	All       = 0xffffffff,

	States    = BIT(0),
	Variables = BIT(1),
	Timers    = BIT(2)
};

typedef CEnumFlags<EObjectDumpFlags> ObjectDumpFlags;

struct IObjectDump
{
	struct SState
	{
		inline SState()
			: szName(nullptr)
		{}

		DrxGUID     guid;
		const char* szName;
	};

	struct SStateMachine
	{
		inline SStateMachine(const DrxGUID& _guid, const char* _szName)
			: guid(_guid)
			, szName(_szName)
		{}

		DrxGUID     guid;
		const char* szName;
		SState      state;
	};

	struct SVariable
	{
		inline SVariable(const DrxGUID& _guid, const char* _szName, const CAnyConstRef& _value)
			: guid(_guid)
			, szName(_szName)
			, value(_value)
		{}

		DrxGUID      guid;
		const char*  szName;
		CAnyConstRef value;
	};

	struct STimer
	{
		inline STimer(const DrxGUID& _guid, const char* _szName, const STimerDuration& _timeRemaining)
			: guid(_guid)
			, szName(_szName)
			, timeRemaining(_timeRemaining)
		{}

		DrxGUID               guid;
		const char*           szName;
		const STimerDuration& timeRemaining;
	};

	virtual ~IObjectDump() {}

	virtual void operator()(const SStateMachine& stateMachine) {}
	virtual void operator()(const SVariable& variable)         {}
	virtual void operator()(const STimer& timer)               {}
};

struct IObject
{
	virtual ~IObject() {}

	virtual ObjectId                      GetId() const = 0;
	virtual const IRuntimeClass&          GetClass() const = 0;
	virtual const char*                   GetScriptFile() const = 0;
	virtual void*                         GetCustomData() const = 0;
	virtual ESimulationMode               GetSimulationMode() const = 0;

	virtual bool                          SetSimulationMode(ESimulationMode simulationMode, EObjectSimulationUpdatePolicy updatePolicy) = 0;
	virtual void                          ProcessSignal(const SObjectSignal& signal) = 0;
	virtual void                          StopAction(CAction& action) = 0; // #SchematycTODO : We need a better way for actions to signal that they're done! Perhaps it would be best to pass a callback?

	virtual EVisitResult                  VisitComponents(const ObjectComponentConstVisitor& visitor) const = 0;
	virtual void                          Dump(IObjectDump& dump, const ObjectDumpFlags& flags = EObjectDumpFlags::All) const = 0;

	virtual IEntity*                      GetEntity() const = 0;

	virtual IObjectPropertiesPtr          GetObjectProperties() const = 0;

	template<typename SIGNAL> inline void ProcessSignal(const SIGNAL& signal, const DrxGUID& senderGUID = DrxGUID())
	{
		ProcessSignal(SObjectSignal::FromSignalClass(signal, senderGUID));
	}
};

struct IObjectPreviewer
{
	virtual ~IObjectPreviewer() {}

	virtual void     SerializeProperties(Serialization::IArchive& archive) = 0;
	virtual ObjectId CreateObject(const DrxGUID& classGUID) const = 0;
	virtual ObjectId ResetObject(ObjectId objectId) const = 0;
	virtual void     DestroyObject(ObjectId objectId) const = 0;
	virtual Sphere   GetObjectBounds(ObjectId objectId) const = 0;
	virtual void     RenderObject(const IObject& object, const SRendParams& params, const SRenderingPassInfo& passInfo) const = 0; // #SchematycTODO : Pass SRenderContext instead of SRendParams and SRenderingPassInfo?
};

} // Schematyc
