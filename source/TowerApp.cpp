#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>

TowerApp::TowerApp(const Framework::AppServices& appData)
	: m_Window{ appData.Window }
	, m_Gpu{ appData.Gpu }
	, m_Canvas{ appData.Canvas }
	, m_Renderer{ appData }
{
	std::cout << "Constructor\n";
}

void TowerApp::Release()
{
	m_Renderer.Release();
}

void TowerApp::Update()
{
}

void TowerApp::Render()
{
	m_Renderer.Render();
}
