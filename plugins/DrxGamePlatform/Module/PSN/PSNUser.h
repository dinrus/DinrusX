#pragma once

#include "IPlatformUser.h"

<DinrusX/np/np_common.h>
<DinrusX/np/np_npid.h>

namespace Drx
{
	namespace GamePlatform
	{
		namespace PSN
		{
			struct SPSNPlatformError;

			class CUser final : public IUser
			{
			public:
				CUser();
				virtual ~CUser();

				// IUser
				virtual const char* GetNickname() const override { return m_onlineId.data; }
				virtual Identifier GetIdentifier() const override { return m_accountId; }
				virtual void SetStatus(const char* status) override { DRX_ASSERT_MESSAGE(false, "The method or operation is not implemented."); }
				virtual const char*  GetStatus() const override { DRX_ASSERT_MESSAGE(false, "The method or operation is not implemented."); return nullptr; }
				virtual ITexture* GetAvatar(EAvatarSize size) const override { DRX_ASSERT_MESSAGE(false, "The method or operation is not implemented."); return nullptr; }
				// ~IUser

				void Initialize();
				void Update();

				bool GetAuthToken(string& tokenOut, int& issuerId);

				void RefreshAuthorizationCode();

				const SceUserServiceUserId& GetUserServiceUserId() const { return m_userId; }
				SceNpId& GetNpId() { return m_npId; }

				void SetSignedInState(ESignInStatus signInStatus);
				void ReinitializePSN();

			private:
				void GetIDsFromPSN();
				void InitPSN();

				bool m_bUserLibraryOwner;

				SceNpAccountId m_accountId;
				SceNpOnlineId m_onlineId;
				SceUserServiceUserId m_userId;

				SceNpId m_npId;

				enum class EAuthorizationCodeStatus
				{
					Invalid,
					Requested,
					Valid
				};

				ICVar* m_pNpClientIdCVar;
				EAuthorizationCodeStatus m_authorizationCodeStatus;

				SceNpAuthorizationCode m_npAuthorizationCode;
				int m_npAuthorizationIssuerId;

				int m_authorizationCodeRequestId;

				int m_npCheckAvailabilityRequestId;

				ESignInStatus m_PSNSignInStatus;
				bool m_bInitialized;
				bool m_bPSNInitialized;

				bool m_bWaitingForSignup;

				SPSNPlatformError* m_pUserSignInError;
				SPSNPlatformError* m_pPatchError;
				SPSNPlatformError* m_pUnderageError;
			};
		}
	}
}