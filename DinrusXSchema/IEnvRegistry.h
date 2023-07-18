// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvInterface.h>
#include <DinrusX/DinrusXSchema/INetworkSpawnParams.h>
#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>
#include <DinrusX/DinrusXSchema/GUID.h>
#include <DinrusX/DinrusXSchema/TypeUtils.h>

namespace Schematyc
{

// Forward declare interfaces.
struct IEnvAction;
struct IEnvClass;
struct IEnvComponent;
struct IEnvDataType;
struct IEnvFunction;
struct IEnvModule;
struct IEnvPackage;
struct IEnvRegistrar;
struct IEnvSignal;

typedef std::unique_ptr<IEnvPackage>                          IEnvPackagePtr;
typedef std::shared_ptr<IEnvElement>                          IEnvElementPtr;

typedef std::function<EVisitStatus(const IEnvPackage&)>           EnvPackageConstVisitor;
typedef std::function<EVisitStatus(const IEnvModule&)>            EnvModuleConstVisitor;
typedef std::function<EVisitStatus(const IEnvDataType&)>          EnvDataTypeConstVisitor;
typedef std::function<EVisitStatus(const IEnvSignal&)>            EnvSignalConstVisitor;
typedef std::function<EVisitStatus(const IEnvFunction&)>          EnvFunctionConstVisitor;
typedef std::function<EVisitStatus(const IEnvClass&)>             EnvClassConstVisitor;
typedef std::function<EVisitStatus(const IEnvInterface&)>         EnvInterfaceConstVisitor;
typedef std::function<EVisitStatus(const IEnvInterfaceFunction&)> EnvInterfaceFunctionConstVisitor;
typedef std::function<EVisitStatus(const IEnvComponent&)>         EnvComponentConstVisitor;
typedef std::function<EVisitStatus(const IEnvAction&)>            EnvActionConstVisitor;

struct IEnvRegistryListener
{
	virtual ~IEnvRegistryListener() {}
	virtual void OnEnvElementAdd(IEnvElementPtr pElement) = 0;
	virtual void OnEnvElementDelete(IEnvElementPtr pElement) = 0;
};

typedef std::vector<IEnvRegistryListener*> TEnvRegistryListeners;

struct IEnvRegistry
{
	virtual ~IEnvRegistry() {}

	virtual void                         RegisterListener(IEnvRegistryListener* pListener) = 0;
	virtual void                         UnregisterListener(IEnvRegistryListener* pListener) = 0;

	virtual bool                         RegisterPackage(IEnvPackagePtr&& pPackage) = 0;
	virtual void                         DeregisterPackage(const DrxGUID& guid) = 0;
	virtual const IEnvPackage*           GetPackage(const DrxGUID& guid) const = 0;
	virtual void                         VisitPackages(const EnvPackageConstVisitor& visitor) const = 0;

	virtual const IEnvElement&           GetRoot() const = 0;
	virtual const IEnvElement*           GetElement(const DrxGUID& guid) const = 0;

	virtual const IEnvModule*            GetModule(const DrxGUID& guid) const = 0;
	virtual void                         VisitModules(const EnvModuleConstVisitor& visitor) const = 0;

	virtual const IEnvDataType*          GetDataType(const DrxGUID& guid) const = 0;
	virtual void                         VisitDataTypes(const EnvDataTypeConstVisitor& visitor) const = 0;

	virtual const IEnvSignal*            GetSignal(const DrxGUID& guid) const = 0;
	virtual void                         VisitSignals(const EnvSignalConstVisitor& visitor) const = 0;

	virtual const IEnvFunction*          GetFunction(const DrxGUID& guid) const = 0;
	virtual void                         VisitFunctions(const EnvFunctionConstVisitor& visitor) const = 0;

	virtual const IEnvClass*             GetClass(const DrxGUID& guid) const = 0;
	virtual void                         VisitClasses(const EnvClassConstVisitor& visitor) const = 0;

	virtual const IEnvInterface*         GetInterface(const DrxGUID& guid) const = 0;
	virtual void                         VisitInterfaces(const EnvInterfaceConstVisitor& visitor) const = 0;

	virtual const IEnvInterfaceFunction* GetInterfaceFunction(const DrxGUID& guid) const = 0;
	virtual void                         VisitInterfaceFunctions(const EnvInterfaceFunctionConstVisitor& visitor) const = 0;

	virtual const IEnvComponent*         GetComponent(const DrxGUID& guid) const = 0;
	virtual void                         VisitComponents(const EnvComponentConstVisitor& visitor) const = 0;

	virtual const IEnvAction*            GetAction(const DrxGUID& guid) const = 0;
	virtual void                         VisitActions(const EnvActionConstVisitor& visitor) const = 0;

	virtual void                         BlacklistElement(const DrxGUID& guid) = 0;
	virtual bool                         IsBlacklistedElement(const DrxGUID& guid) const = 0;
};

} // Schematyc
