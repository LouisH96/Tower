#include "pch.h"
#include "TowerApp.h"
#include "Framework/GameFrame.h"

int main()
{
	App::Win32::Window::Options options{};
	options.CursorFocusMode = true;
	options.ClientSize = { 1300,900 };
	Framework::GameFrame::Run<TowerGame::TowerApp>(L"Tower", options);
}
