#include "pch.h"
//#include <Framework/BasicAppFrame.h>
#include <Framework/Frame_NoCam.h>

#include "TowerApp.h"

int main()
{
	Framework::Frame_NoCam<TowerApp>::Run(L"Tower");
}
