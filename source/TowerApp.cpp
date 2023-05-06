#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>
#include <Framework/CoreServices.h>

TowerApp::TowerApp(const Framework::CoreServices& services)
	: m_Window{ services.Window }
	, m_Gpu{ services.Gpu }
	, m_Canvas{ services.Canvas }
	, m_Renderer{ services }
	, m_Character{ services }
	, m_Terrain{ services.Gpu }
{
	m_Terrain.Register(m_Renderer);
	m_Character.Register(m_Terrain);
	m_Character.Register(m_Renderer);
}

void TowerApp::Release()
{
	m_Renderer.Release();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		m_Window.SetIsDestroyed();
		return;
	}

	m_Character.Update();
}

void TowerApp::Render()
{
	m_Renderer.Render(m_Character.GetCameraController());
}
