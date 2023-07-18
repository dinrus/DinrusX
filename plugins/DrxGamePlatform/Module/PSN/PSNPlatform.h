#pragma once

#include "IGamePlatform.h"
#include "PSNUser.h"
#include "PSNStatistics.h"
#include "PSNLeaderboards.h"

namespace Drx
{
	namespace GamePlatform
	{
		namespace PSN
		{
			class CPlugin final : public IPlugin
			{
				DRXINTERFACE_BEGIN()
					DRXINTERFACE_ADD(Drx::GamePlatform::IPlugin)
					DRXINTERFACE_ADD(Drx::IEnginePlugin)
				DRXINTERFACE_END()

				DRXGENERATE_SINGLETONCLASS_GUID(CPlugin, "Plugin_GamePlatform", "{80A16DB3-6216-4726-B933-3D5E94BADA56}"_drx_guid)

			public:
				virtual ~CPlugin() = default;

				// IDrxPlugin
				virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
				virtual void MainUpdate(float frameTime) override;
				// ~IDrxPlugin

				// IGamePlatform
				virtual bool IsInitialized() const override { return true; }
				virtual EType GetType() const override { return EType::PSN; }

				virtual void AddListener(IListener& listener) override {}
				virtual void RemoveListener(IListener& listener) override {}

				virtual int GetBuildIdentifier() const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return 0; }

				virtual IUser* GetLocalClient() override { return& m_user; }
				virtual IUser* GetUserById(IUser::Identifier id) override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }

				virtual bool IsFriendWith(IUser::Identifier otherUserId) const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return false; }

				virtual IServer* CreateServer(bool bLocal) override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }
				virtual IServer* GetLocalServer() const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }

				virtual ILeaderboards* GetLeaderboards() const override { return m_pLeaderboards.get(); }
				virtual IStatistics* GetStatistics() const override { return const_cast<CStatistics*>(&m_statistics); }
				virtual IRemoteStorage* GetRemoteStorage() const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }

				virtual IMatchmaking* GetMatchmaking() const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }

				virtual INetworking* GetNetworking() const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return nullptr; }

				virtual bool OwnsApplication(ApplicationIdentifier id) const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return false; }
				virtual ApplicationIdentifier GetApplicationIdentifier() const override  { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return 0; }

				virtual bool OpenDialog(EDialog dialog) const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return false; }
				virtual bool OpenDialog(const char* szPage) const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return false; }
				virtual bool OpenBrowser(const char* szURL) const override { DRX_ASSERT_MESSAGE(false, "Not implemented for PSN!"); return false; }

				virtual bool CanOpenPurchaseOverlay() const override { return false; }

				virtual bool GetAuthToken(string& tokenOut, int& issuerId) override { return m_user.GetAuthToken(tokenOut, issuerId); }

				virtual EConnectionStatus GetConnectionStatus() const override { return m_connectionStatus; }
				virtual void CanAccessMultiplayerServices(std::function<void(bool authorized)> asynchronousCallback) override;
				// ~IGamePlatform

				void SetConnectionStatus(EConnectionStatus status) { m_connectionStatus = status; }

				static CPlugin* GetInstance() { return s_pInstance; }

			private:
				CUser m_user;
				CStatistics m_statistics;

				std::unique_ptr<CLeaderboards> m_pLeaderboards;
				std::function<void(bool authorized)> m_multiplayerAccessCallback;

				int m_sceNetCtlCallbackId;
				EConnectionStatus m_connectionStatus;

				SceNpCheckPlusResult m_npCheckPlusResult;
				int m_npCheckPlusRequestId = 0;
				float m_npNotifyPlusFeatureTimer = 0.f;

			private:
				static CPlugin* s_pInstance;
			};
		}
	}
}