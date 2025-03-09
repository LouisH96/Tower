#include "pch.h"
#include "TowerUi.h"

#include <Ui\Elements\AllElements.h>

using namespace TowerGame;

TowerUi::TowerUi()
{
	AnchorChild rootChild{};
	rootChild.SetFillMin();
	rootChild.SetAnchors({ .5f,1.f });

	Margin* pMargin{ new Margin(15.f) };
	rootChild.pChild = pMargin;
	UI_ROOT.AddChild(rootChild);

	m_pList = new ListPanel(ListPanel::Direction::DownAndLeft);
	m_pList->SetUniformFillSize(false);
	pMargin->AddChild(m_pList);

	m_pScoreLabel = new DynamicNumberLabel(5, Color::Green, SCORE_FONT_SIZE);
	m_pScoreLabel->SetPivot({ .5f,0 });
	m_pList->AddChild(m_pScoreLabel);

	SyncScore();
}

void TowerUi::Update()
{
	for (unsigned iGain{ 0 }; iGain < m_Gains.GetSize();)
	{
		GainInfo& gainInfo{ m_Gains[iGain] };

		if (gainInfo.Timer > 0)
		{
			gainInfo.Timer -= DELTA_TIME;
			++iGain;
		}
		else
		{
			m_pList->DeleteChild(gainInfo.pElem);
			m_Gains.Remove(iGain);
		}
	}
}

void TowerUi::AddScore(unsigned score)
{
	m_Score += score;
	SyncScore();
	SetGain(score);
}

void TowerUi::SyncScore()
{
	m_pScoreLabel->SetNumber(m_Score);
}

void TowerUi::SetGain(unsigned gain)
{
	const std::string text{ "+" + Convert::ToString(gain) };
	Label* pLabel{ new Label(text, 
		Color::Green * .9f,
		static_cast<unsigned>(SCORE_FONT_SIZE * .65f)) };

	m_pList->AddChild(pLabel);

	GainInfo gainInfo{};
	gainInfo.pElem = pLabel;

	m_Gains.Add(gainInfo);
}
