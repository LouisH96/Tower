#include "pch.h"
//#include <Framework/BasicAppFrame.h>
#include <Framework/Frame_NoCam.h>
#include "TowerApp.h"

int main()
{
	App::Win32::Window::Options options{};
	options.CursorFpsMode = true;
	Framework::Frame_NoCam<TowerApp>::Run(L"Tower", options);
}
