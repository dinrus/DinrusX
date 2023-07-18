#pragma once

#include "IPlatformStatistics.h"

<DinrusX/np.h>

namespace Drx
{
	namespace GamePlatform
	{
		namespace PSN
		{
			class CStatistics final : public IStatistics
			{
			public:
				virtual ~CStatistics();

				// IPlatformStatistics
				virtual void AddListener(IListener& listener) override {}
				virtual void RemoveListener(IListener& listener) override {}

				virtual bool ResetStatistics() override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }
				virtual bool ResetAchievements() override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }

				virtual bool Download() override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }
				virtual bool Upload() override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }

				virtual bool Set(const char* name, int value) override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }
				virtual bool Set(const char* name, float value) override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }

				virtual bool Get(const char* name, int& value) const override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }
				virtual bool Get(const char* name, float& value) const override { DRX_ASSERT_MESSAGE(false, "Not supported by PSN!"); return false; }

				virtual IAchievement* GetAchievement(const char* szName) override { DRX_ASSERT_MESSAGE(false, "Retrieving achievements by name is not supported by PSN!"); return nullptr; }
				virtual IAchievement* GetAchievement(int id) override;
				// ~IPlatformStatistics

				void Initialize();

			private:
				void ParseTrophies();

			private:
				std::vector<std::unique_ptr<IAchievement>> m_achievements;

				SceNpTrophyContext m_trophyContext;
				SceNpTrophyGameDetails m_trophyGameDetails;
				SceNpTrophyGameData m_trophyGameData;
			};
		}
	}
}