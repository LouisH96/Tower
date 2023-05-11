#include "pch.h"
//#include <Framework/BasicAppFrame.h>
#include <Framework/Frame_NoCamContr.h>
#include "TowerApp.h"

int main()
{
	App::Win32::Window::Options options{};
	options.CursorFpsMode = true;
	options.ClientSize = { 2000,1350 };
	Framework::Frame_NoCamContr<TowerApp>::Run(L"Tower", options);
}
