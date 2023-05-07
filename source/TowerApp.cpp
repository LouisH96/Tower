#include "pch.h"
#include "TowerApp.h"

#include <Framework/BasicAppFrame.h>
#include <Framework/CoreServices.h>

#include "Character/Character.h"
#include "Character/EnemySystem.h"
#include "Environment/Tower.h"
#include "Services/TowerAppServices.h"

using namespace Math;

TowerApp::TowerApp(const Framework::CoreServices& coreServices)
	: m_Window{ coreServices.Window }
	, m_Gpu{ coreServices.Gpu }
	, m_Canvas{ coreServices.Canvas }
	, m_Services{ coreServices, {coreServices}, {} }
{
	const Math::Float2 terrainSize{ 150,150 };
	const Math::Float2 towerPosition{ terrainSize / 2 };
	const Math::Float3 towerPosition3{ Float3::FromXz(terrainSize / 2) };
	const Math::Float2 towerRoofSize{ 10,6 };
	constexpr float towerHeight{ 8 };
	m_pTerrain = new Terrain(m_Services, {}, terrainSize);
	m_pTower = new Tower(m_Services, towerPosition3, towerRoofSize, towerHeight);
	const Float3 characterPosition{ towerPosition3 + Float3::FromXz(towerRoofSize * .5) + Float3{ 0,towerHeight, 0 } };
	//const Float3 characterPosition{};
	m_pCharacter = new Character(coreServices, characterPosition);

	m_pCharacter->Register(*m_pTerrain);
	m_pCharacter->Register(m_Services.Renderer);
	m_pCharacter->Register(*m_pTower);

	//Enemy-System
	m_pEnemySystem = new EnemySystem(coreServices, m_Services.Renderer, 10, towerPosition, *m_pTerrain);
}

void TowerApp::Release()
{
	delete m_pEnemySystem;
	delete m_pTower;
	delete m_pTerrain;
	delete m_pCharacter;
	m_Services.Renderer.Release();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		m_Window.SetIsDestroyed();
		return;
	}

	m_pCharacter->Update(m_Services);
	m_pEnemySystem->Update(m_Services);
}

void TowerApp::Render()
{
	m_Services.Renderer.Render(m_pCharacter->GetCameraController());
}
