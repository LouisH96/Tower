#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>
#include <Framework/CoreServices.h>

TowerApp::TowerApp(const Framework::CoreServices& services)
	: m_Window{ services.Window }
	, m_Gpu{ services.Gpu }
	, m_Canvas{ services.Canvas }
	, m_CameraController{ services.Camera, services.Window.GetKeyboard(), services.Window.GetMouse() }
	, m_Renderer{ services }
	, m_Bow{ services.Gpu }
	, m_Terrain{ services.Gpu }
{
	m_CameraController.SetPosition({ 0,1,0 });
	m_Bow.Register(m_Renderer.GetTransformRenderer());
	m_Terrain.Register(m_Renderer);
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

	const Math::Float2 movement{ Globals::pKeyboard->GetWasdInput(Globals::DeltaTime*5) };
	m_CameraController.MoveRelative({ movement.x, 0, movement.y });

	m_CameraController.Update();
	m_Bow.Update(m_CameraController.GetTransform(), m_Renderer.GetTransformRenderer());
}

void TowerApp::Render()
{
	m_Renderer.Render(m_CameraController);
}
