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
	, m_Bow{ services.GetGpu() }
{
	m_CameraController.SetPosition({ 0,1,0 });
	m_Bow.Register(m_Renderer.GetTransformRenderer());
}

void TowerApp::Release()
{
	m_Renderer.Release();
}

void TowerApp::Update()
{
	if(m_Window.GetKeyboard().IsDown_('\x1b'))
	{
		m_Window.SetIsDestroyed();
		return;
	}

	m_CameraController.Update();
	m_Bow.Update(m_CameraController.GetWorldMatrix());
}

void TowerApp::Render()
{
	m_Renderer.Render(m_CameraController);
}
