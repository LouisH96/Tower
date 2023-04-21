#include "pch.h"
#include "TowerApp.h"
#include <Rendering/FpsDisplay.h>
#include <App/FpsControl.h>

TowerApp::TowerApp(App::Wrappers::Win32::Window& window, Rendering::Gpu& gpu, Rendering::Canvas& canvas,
	Game::Camera& camera, App::FpsControl& fpsControl)
	: m_Window{ window }
	, m_Gpu{ gpu }
	, m_Canvas{ canvas }
	, m_Renderer{ window, gpu, canvas, fpsControl ,camera }
{
	std::cout << "Constructor\n";
}

void TowerApp::Release()
{
	std::cout << "Release\n";
}

void TowerApp::Update()
{
	m_Renderer.Render();
}
