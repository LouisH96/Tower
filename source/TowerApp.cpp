#include "pch.h"
#include "TowerApp.h"

#include <App/Win32/Window.h>
#include <Framework/Resources.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering/Renderers/Texture2DRenderer.h>

using namespace TowerGame;

TowerApp* TowerApp::pApp{ nullptr };

TowerApp::TowerApp()
{
	pApp = this;
	m_TowerSystems.Init();
}

TowerApp::~TowerApp()
{
}

void TowerApp::OnCanvasResized(const App::ResizedEvent& event)
{
	m_Renderer.OnCanvasResized(event);
	m_Systems.OnCanvasResized(event);
}

void TowerApp::EarlyUpdate()
{
	m_TowerSystems.EarlyUpdate();
	m_Systems.EarlyUpdate();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		Globals::pWindow->SetIsDestroyed();
		return;
	}

	m_TowerSystems.Update();
	m_Systems.Update();
}

void TowerApp::PreRender()
{
	m_Renderer.PreRender();
	m_Systems.PreRender();
}

void TowerApp::Render()
{
	m_Renderer.Render();
	m_Systems.Render();
}

void TowerApp::RenderUi()
{
}
