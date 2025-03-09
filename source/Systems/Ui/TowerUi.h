#pragma once
#include <Ui\Elements\DynamicNumberLabel.h>
#include <Ui\Elements\ListPanel.h>

namespace TowerGame
{
class TowerUi
{
public:
	TowerUi();

	void Update();

	void AddScore(unsigned score);

private:
	static constexpr float SCORE_GAIN_DURATION{ 3.f }; //how long the ScoreIncrease should be visible
	static constexpr float SCORE_FONT_SIZE{ 18 };

	DynamicNumberLabel* m_pScoreLabel{};
	unsigned m_Score{ 0 }; //Todo: needs to be added to a Session (or something) object/system

	ListPanel* m_pList{};

	struct GainInfo {
		float Timer{ SCORE_GAIN_DURATION };
		Elem* pElem{};
	};
	List<GainInfo> m_Gains{};

	void SyncScore();
	void SetGain(unsigned gain);
};
}