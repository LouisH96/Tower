#include "pch.h"
#include "TowerApp.h"
#include <Framework/BasicAppFrame.h>
#include <Framework/CoreServices.h>

using namespace Math;

TowerApp::TowerApp(const Framework::CoreServices& services)
	: m_Window{ services.Window }
	, m_Gpu{ services.Gpu }
	, m_Canvas{ services.Canvas }
	, m_Renderer{ services }
{
	const Math::Float2 terrainSize{ 150,150 };
	const Math::Float3 towerPosition{ Math::Float3::FromXz(terrainSize / 2) };
	const Math::Float2 towerRoofSize{ 10,6 };
	constexpr float towerHeight{ 8 };
	m_pTerrain = new Terrain(services.Gpu, m_Renderer, {}, terrainSize);
	m_pTower = new Tower(services, m_Renderer, towerPosition, towerRoofSize, towerHeight);
	m_pCharacter = new Character(services, towerPosition + Float3::FromXz(towerRoofSize * .5) + Float3{ 0,towerHeight, 0 });


	m_pCharacter->Register(*m_pTerrain);
	m_pCharacter->Register(m_Renderer);
	m_pCharacter->Register(*m_pTower);
}

void TowerApp::Release()
{
	delete m_pTower;
	delete m_pTerrain;
	delete m_pCharacter;
	m_Renderer.Release();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		m_Window.SetIsDestroyed();
		return;
	}

	m_pCharacter->Update();
}

void TowerApp::Render()
{
	m_Renderer.Render(m_pCharacter->GetCameraController());
}
