// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#if DRX_PLATFORM_DURANGO && USE_DURANGOLIVE

// Definition of noexcept to cover future deprecation of throw()
	#define noexcept throw()

// {00000000-0000-0000-0000-000000000000}
static GUID const NULL_GUID =
{
	0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

// WinRT core
	<DinrusX/winapifamily.h>

	<DinrusX/wrl.h>
	<DinrusX/wrl/ftm.h>
	#pragma warning(push)
	#pragma warning(disable : 28204)
	<DinrusX/wrl/async.h>
	#pragma warning(pop)
	<DinrusX/wrl/client.h>
	<DinrusX/wrl/event.h>
	<DinrusX/wrl/module.h>

	#include "robuffer.h"
	<DinrusX/Objbase.h>
	<DinrusX/tchar.h>

// Windows concurrency
	<DinrusX/ctxtcall.h>
	#pragma warning(push)
	#pragma warning(disable : 4355)
	#include "ppltasks_extra.h"
	#pragma warning(pop)

	#include "wmistr.h"
	#include "etwplus.h"
	#include "evntprov.h"
	#include "evntrace.h"

	#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_TV_TITLE)
		<DinrusX/winsock2.h>
		<DinrusX/ws2tcpip.h>
	#endif
	#include "conio.h"

	<DinrusX/stdio.h>

// StdLib
	<DinrusX/fstream>
	<DinrusX/iostream>
	<DinrusX/ios>
	#pragma warning(push)
	#pragma warning(disable : 4715)
	<DinrusX/filesystem>
	#pragma warning(pop)
	<DinrusX/tuple>
	<DinrusX/bitset>
	<DinrusX/memory>
	<DinrusX/sstream>
	<DinrusX/type_traits>
	<DinrusX/string>
	<DinrusX/sstream>

// STL Containers
	<DinrusX/deque>
	<DinrusX/array>
	<DinrusX/vector>
	<DinrusX/list>
	<DinrusX/string>
	<DinrusX/queue>
	<DinrusX/map>
	<DinrusX/unordered_map>
	<DinrusX/unordered_set>
	<DinrusX/set>

// STL Misc
	<DinrusX/regex>
	<DinrusX/functional>
	<DinrusX/random>
	<DinrusX/filesystem>

// STL Algorithms
	<DinrusX/algorithm>
	<DinrusX/numeric>

// STL concurrency
	<DinrusX/atomic>
	<DinrusX/functional>
	<DinrusX/thread>
	<DinrusX/mutex>
	<DinrusX/future>
	<DinrusX/condition_variable>
	<DinrusX/chrono>

// WinRT headers
	<DinrusX/Windows.Foundation.h>
	<DinrusX/Windows.ApplicationModel.Core.h>
	<DinrusX/Windows.Xbox.ApplicationModel.Store.h>
	<DinrusX/Windows.Data.Json.h>
	<DinrusX/Windows.Networking.h>
	<DinrusX/Windows.Storage.Streams.h>
	<DinrusX/Windows.System.h>
	<DinrusX/Windows.Ui.Core.h>
	<DinrusX/Windows.Xbox.Networking.h>
	<DinrusX/Windows.Xbox.Management.Deployment.h>

// <DinrusX/Windows.System.Threading.h>

	#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_TV_TITLE)
		<DinrusX/ixmlhttprequest2.h>
	#else
		<DinrusX/msxml6.h>
	#endif
	<DinrusX/Windows.Xbox.Input.h>
	<DinrusX/Windows.Xbox.Storage.h>
	<DinrusX/Windows.Xbox.System.h>
	<DinrusX/Windows.Xbox.Multiplayer.h>
	<DinrusX/Windows.Xbox.Networking.h>
	#pragma warning(push)
	#pragma warning(disable : 6244 28251)
	<DinrusX/Microsoft.Xbox.Services.h>
	#pragma warning(pop)
	<DinrusX/Microsoft.Xbox.Services.Marketplace.h>

	#include "AbiUtil.h"

namespace ABI {
namespace Windows {
namespace ApplicationModel {
namespace Core {
class CoreApplication
{
};
typedef ICoreApplication ICoreApplicationStatics;
}     // Core
}     // ApplicationModel
namespace System
{
class Launcher;
class ILauncher;
}
namespace Xbox {
namespace ApplicationModel {
namespace Store  {
class Product;
class IProduct;
}       // Store
}       // ApplicationModel
namespace System {
}       // } System
namespace Multiplayer {
class Party
{
};
class IParty
{
};
}   // } Multiplayer
}   // } Xbox
}   // } Windows

//ABI::Windows::Xbox::ApplicationMode
namespace Microsoft {
namespace Xbox {
namespace Services {
CX_TYPEDEF_F(IXboxLiveContext);
CX_TYPEDEF(IXboxLiveContextSettings);
namespace Multiplayer
{
CX_TYPEDEF_F(IMultiplayerSession);
CX_TYPEDEF_F(IMultiplayerSessionReference);
CX_TYPEDEF(IMultiplayerSessionMember);
CX_TYPEDEF(IMultiplayerSessionStates);
CX_TYPEDEF(IMultiplayerSessionConstants);
CX_TYPEDEF(IMultiplayerSessionProperties);
CX_TYPEDEF(IMultiplayerService);
}         // } Multiplayer
namespace Matchmaking
{
CX_TYPEDEF(IHopperStatisticsResponse);
CX_TYPEDEF(ICreateMatchTicketResponse);
CX_TYPEDEF(IMatchTicketDetailsResponse);
//          CX_TYPEDEF(IMatchesForUserResponse);
CX_TYPEDEF(IMatchmakingService);
}         // } Matchmaking
namespace Marketplace
{
CX_TYPEDEF(ICatalogService);
CX_TYPEDEF(ICatalogItem);
CX_TYPEDEF(IBrowseCatalogResult);
CX_TYPEDEF(IInventoryService);
CX_TYPEDEF(IInventoryItemsResult);
CX_TYPEDEF(IInventoryItem);
CX_TYPEDEF(IConsumeInventoryItemResult);
}         // } MarketPlace
namespace Presence
{
CX_TYPEDEF_F(IPresenceData);
CX_TYPEDEF(IPresenceService);
}         // } Presence
namespace TitleStorage
{
CX_TYPEDEF_F(ITitleStorageBlobMetadata);
CX_TYPEDEF(ITitleStorageBlobResult);
CX_TYPEDEF(ITitleStorageService);
} // } TitleStorage
} // } Services
} // } Xbox
} // } Microsoft
} // } ABI

USING_RTC_F_B(ABI::Windows::Foundation::Uri, ABI::Windows::Foundation::IUriRuntimeClass);

USING_RTC_S(ABI::Windows::ApplicationModel::Core::CoreApplication, ABI::Windows::ApplicationModel::Core::ICoreApplication);
//USING_RTC_S( ABI::Windows::ApplicationModel::Core::CoreApplicationContext, ABI::Windows::ApplicationModel::Core::ICoreApplicationContext );

USING_RTC_S_B(ABI::Windows::Data::Json::JsonObject, ABI::Windows::Data::Json::IJsonObject);
USING_RTC_B(ABI::Windows::Data::Json::JsonArray, ABI::Windows::Data::Json::IJsonArray);
USING_RTC_S_B(ABI::Windows::Data::Json::JsonValue, ABI::Windows::Data::Json::IJsonValue);

USING_RTC_F(ABI::Windows::Storage::Streams::Buffer, ABI::Windows::Storage::Streams::IBuffer);
USING_RTC_F_B(ABI::Windows::Storage::Streams::DataWriter, ABI::Windows::Storage::Streams::IDataWriter);
USING_RTC_F_B(ABI::Windows::Storage::Streams::DataReader, ABI::Windows::Storage::Streams::IDataReader);

USING_RTC_S_B(ABI::Windows::System::Launcher, ABI::Windows::System::ILauncher);

USING_RTC_S_B(ABI::Windows::Xbox::ApplicationModel::Store::Product, ABI::Windows::Xbox::ApplicationModel::Store::IProduct);

USING_RTC_S(ABI::Windows::Xbox::Input::Controller, ABI::Windows::Xbox::Input::IController);
USING_RTC_S(ABI::Windows::Xbox::Input::Gamepad, ABI::Windows::Xbox::Input::IGamepad);
USING_RTC_S_B(ABI::Windows::Xbox::Management::Deployment::PackageTransferUpr, ABI::Windows::Xbox::Management::Deployment::IPackageTransferUpr);
USING_RTC_S_B(ABI::Windows::Xbox::Multiplayer::Party, ABI::Windows::Xbox::Multiplayer::IParty);
USING_RTC_F_B(ABI::Windows::Xbox::Multiplayer::MultiplayerSessionReference, ABI::Windows::Xbox::Multiplayer::IMultiplayerSessionReference);
USING_RTC_B(ABI::Windows::Xbox::Storage::ConnectedStorageContainer, ABI::Windows::Xbox::Storage::IConnectedStorageContainer);
USING_RTC_S_B(ABI::Windows::Xbox::Storage::ConnectedStorageSpace, ABI::Windows::Xbox::Storage::IConnectedStorageSpace);
USING_RTC_S_B(ABI::Windows::Xbox::System::User, ABI::Windows::Xbox::System::IUser);

USING_RTC_S_B(ABI::Windows::Xbox::Networking::SecureDeviceAssociationTemplate, ABI::Windows::Xbox::Networking::ISecureDeviceAssociationTemplate);
USING_RTC_S_B(ABI::Windows::Xbox::Networking::SecureDeviceAssociation, ABI::Windows::Xbox::Networking::ISecureDeviceAssociation);
USING_RTC_S_B(ABI::Windows::Xbox::Networking::SecureDeviceAddress, ABI::Windows::Xbox::Networking::ISecureDeviceAddress);

USING_RTC_F_B(ABI::Microsoft::Xbox::Services::XboxLiveContext, ABI::Microsoft::Xbox::Services::IXboxLiveContext);
USING_RTC_F_B(ABI::Microsoft::Xbox::Services::Multiplayer::MultiplayerSession, ABI::Microsoft::Xbox::Services::Multiplayer::IMultiplayerSession);
USING_RTC_F_B(ABI::Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference, ABI::Microsoft::Xbox::Services::Multiplayer::IMultiplayerSessionReference);
USING_RTC_F_B(ABI::Microsoft::Xbox::Services::Presence::PresenceData, ABI::Microsoft::Xbox::Services::Presence::IPresenceData);
USING_RTC_F_B(ABI::Microsoft::Xbox::Services::TitleStorage::TitleStorageBlobMetadata, ABI::Microsoft::Xbox::Services::TitleStorage::ITitleStorageBlobMetadata);
#endif
