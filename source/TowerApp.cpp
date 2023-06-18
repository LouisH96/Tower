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
	: m_Services{ coreServices, {coreServices}, {} }
{
	const Float2 terrainSize{ 250,250 };
	const Float2 towerPosition{ terrainSize / 2 };
	const Float3 towerPosition3{ Float3::FromXz(terrainSize / 2) };
	const Float2 towerRoofSize{ 10,6 };
	constexpr float towerHeight{ 8 };
	Terrain::Init(m_Services, {}, terrainSize);
	m_pTower = new Tower(m_Services, towerPosition3, towerRoofSize, towerHeight);
	const Float3 characterPosition{ towerPosition3 + Float3::FromXz(towerRoofSize * .5) + Float3{ 0,towerHeight, 0 } };
	m_pCharacter = new Character(coreServices, characterPosition);

	m_pCharacter->Register(Terrain::Get());
	m_pCharacter->Register(m_Services.Renderer);
	m_pCharacter->Register(*m_pTower);
	m_Services.pBowSystem = &m_pCharacter->GetBow();

	//Enemy-System
	m_pEnemySystem = new EnemySystem(m_Services, 20, towerPosition, Terrain::Get());
	m_Services.pEnemySystem = m_pEnemySystem;
}

void TowerApp::Release()
{
	delete m_pEnemySystem;
	delete m_pTower;
	Terrain::Release();
	delete m_pCharacter;
	m_Services.Renderer.Release();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		Globals::pWindow->SetIsDestroyed();
		return;
	}

	m_pCharacter->Update(m_Services);
	m_pEnemySystem->Update(m_Services);
}

void TowerApp::Render()
{
	m_Services.Renderer.Render(m_pCharacter->GetCameraController());
}
