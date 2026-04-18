#include "pch.h"
#include "PauseMenu.h"

#include <App/Win32/Window.h>
#include <Systems/Ui/Style.h>
#include <TowerApp.h>

namespace TowerGame
{
PauseMenu::PauseMenu()
{
	using namespace Ui;

	//Menu
	AnchorParent* pMenuRoot{ new AnchorParent() };
	m_pRoot = pMenuRoot;
	SimpleParent* pMenu{ Style::Border() };
	pMenuRoot->AddChild(pMenu, { .5f,.5f });

	//Horizontal List
	ListPanel* pMenuList{ ListPanel::Horizontal() };
	pMenu->AddChild(pMenuList);

	//Button List
	ListPanel* pListLeft{ ListPanel::Vertical() };
	pMenuList->AddChild(pListLeft);

	//Buttons
	struct BtnSettings { Buttons Btn{}; std::string Text; };
	List<BtnSettings> buttons{ static_cast<unsigned>(Buttons::END) };
	buttons.Add({ Buttons::CONTINUE, "Continue" });
	buttons.Add({ Buttons::EXIT, "Exit Game" });
	for (unsigned i{ 0 }; i < buttons.GetSize(); ++i)
		Style::CreateButton(pListLeft, buttons[i].Text, BtnClicked, static_cast<unsigned>(buttons[i].Btn));
}

PauseMenu::~PauseMenu()
{
	if (!m_IsPaused)
	{
		delete m_pRoot;
		m_pRoot = nullptr;
	}
}

void PauseMenu::Toggle()
{
	SetOpen(!m_IsPaused);
}

void PauseMenu::SetOpen(bool setOpen)
{
	setOpen ? Open() : Close();
}

void PauseMenu::Open()
{
	m_IsPaused = true;
	Globals::TimeScale = 0;
	Globals::pWindow->SetCursorFocusMode(false);
	UI_ROOT.AddChild(m_pRoot, Float2{ .5f });
}

void PauseMenu::Close()
{
	m_IsPaused = false;
	Globals::TimeScale = 1;
	Globals::pWindow->SetCursorFocusMode(true);
	UI_ROOT.RemoveChild(m_pRoot);
}

void PauseMenu::BtnClicked(unsigned iBtn)
{
	PauseMenu& pauseMenu{ APP.GetPauseMenu() };
	switch (static_cast<Buttons>(iBtn))
	{
	case Buttons::CONTINUE:
		pauseMenu.Close();
		break;

	case Buttons::EXIT:
		Globals::pWindow->SetIsDestroyed();
		break;
	}
}
}
