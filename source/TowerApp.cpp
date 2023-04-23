#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>
#include <Framework/CoreServices.h>

TowerApp::TowerApp(const Framework::CoreServices& services)
	: m_Window{ services.GetWindow() }
	, m_Gpu{ services.GetGpu() }
	, m_Canvas{ services.GetCanvas() }
	, m_CameraController{ services.GetCamera(), services.GetWindow().GetKeyboard(), services.GetWindow().GetMouse() }
	, m_Renderer{ services }
{
	m_CameraController.SetPosition({ 0,0,-2 });
}

void TowerApp::Release()
{
	m_Renderer.Release();
}

void TowerApp::Update()
{
	m_CameraController.Update();
}

void TowerApp::Render()
{
	m_Renderer.Render(m_CameraController.GetCameraPosition(), m_CameraController.GetViewProjectionMatrix());
}
