#include "pch.h"
#include "PauseMenu.h"

#include <App/Win32/Window.h>

namespace TowerGame
{
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
}

void PauseMenu::Close()
{
	m_IsPaused = false;
	Globals::TimeScale = 1;
	Globals::pWindow->SetCursorFocusMode(true);
}
}
