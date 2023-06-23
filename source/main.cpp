#include "pch.h"
#include "TowerApp.h"
#include "Framework/GameFrame.h"

int main()
{
	App::Win32::Window::Options options{};
	options.CursorFocusMode = true;
	options.ClientSize = { 2000,1350 };
	Framework::GameFrame::Run<TowerApp>(L"Tower", options);
}
