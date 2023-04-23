#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>
#include <Game/Camera/Camera.h>
#include <Framework/CoreServices.h>

TowerApp::TowerApp(const Framework::CoreServices& services)
	: m_Window{ services.GetWindow() }
	, m_Gpu{ services.GetGpu() }
	, m_Canvas{ services.GetCanvas() }
	, m_Camera{ Math::Int2{services.GetWindow().GetClientWidth(), services.GetWindow().GetClientHeight()} }
{
	m_pRenderer = new TowerAppRenderer(services, m_Camera);

	m_Camera.GetTransform().Position.z -= 1;
}

void TowerApp::Release()
{
	delete m_pRenderer;
}

void TowerApp::Update()
{

}

void TowerApp::Render()
{
	m_pRenderer->Render();
}
