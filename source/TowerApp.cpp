#include "pch.h"
#include "TowerApp.h"

#include <Character/Character.h>
#include <Character/EnemySystem.h>
#include <Debug/Rendering/DebugRenderer.h>
#include <Environment/Tower.h>
#include <Framework/Resources.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering/Renderers/Texture2DRenderer.h>
#include <Services/CollisionService.h>
#include <Weapons/Bow.h>

#include "App/Win32/Window.h"
#include "Environment/Terrain.h"
#include "Weapons/ArrowSystem.h"

using namespace TowerGame;

TowerApp* TowerApp::pApp{ nullptr };

TowerApp::TowerApp()
{
	pApp = this;

	InitGameplay();
	LinkGameplay();
	InitRendering();
	LinkRendering();

	CreateArrows();
}

TowerApp::~TowerApp()
{
	delete m_Rendering.pTerrainRenderer;
	delete m_Rendering.pSimpleRenderer;

	delete m_Gameplay.pArrowSystem;
	delete m_Gameplay.pEnemySystem;
	delete m_Gameplay.pCollisionService;
	delete m_Gameplay.pBow;
	delete m_Gameplay.pTerrain;
	delete m_Gameplay.pTower;
	delete m_Gameplay.pCharacter;
}

void TowerApp::OnCanvasResized(const App::ResizedEvent& event)
{
	m_Renderer.OnCanvasResized(event);
}

void TowerApp::EarlyUpdate()
{
	m_Gameplay.pCharacter->Update();
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		Globals::pWindow->SetIsDestroyed();
		return;
	}

	m_Gameplay.pBow->Update(m_Gameplay.pCharacter->GetCameraController().GetTransform());
	m_Gameplay.pEnemySystem->Update();
	m_Gameplay.pArrowSystem->Update();
}

void TowerApp::PreRender()
{
	m_Renderer.GetShadowRenderer().MoveShadow(m_Gameplay.GetCharacter());
	m_Renderer.PreRender();
}

void TowerApp::Render()
{
	m_Renderer.Render();
}

void TowerApp::RenderUi()
{
}

void TowerApp::InitGameplay()
{
	//TERRAIN & TOWER
	const Float2 terrainSize{ 250,250 };
	const Float2 towerPosition{ terrainSize / 2 };
	const Float3 towerPosition3{ Float3::FromXz(terrainSize / 2) };
	const Float2 towerRoofSize{ 10,6 };
	constexpr float towerHeight{ 8 };
	m_Gameplay.pTerrain = new Terrain({}, terrainSize);
	m_Gameplay.pTower = new Tower(towerPosition3, towerRoofSize, towerHeight);

	//CHARACTER & BOW
	const Float3 characterPosition{ towerPosition3 + Float3::FromXz(towerRoofSize * .5) + Float3{ 0,towerHeight, 0 } };
	m_Gameplay.pCharacter = new Character(characterPosition);
	m_Gameplay.pBow = new Bow();
	m_Gameplay.pArrowSystem = new ArrowSystem();

	//COLLIDABLES
	m_Gameplay.pCollisionService = new CollisionService();

	//ENEMIES
	m_Gameplay.pEnemySystem = new EnemySystem(100, towerPosition);
}

void TowerApp::LinkGameplay()
{
	m_Gameplay.pTower->LinkGameplay();
}

void TowerApp::InitRendering()
{
	using namespace Rendering;
	RenderSystems& r{ m_Rendering };

	r.pTerrainRenderer = new TowerGame::TerrainRenderer();
	r.pSimpleRenderer = RendererFactory::CreateSimpleRenderer();

	m_Renderer.GetShadowRenderer().InitShadow(m_Gameplay.GetCharacter());
}

void TowerApp::LinkRendering()
{
	m_Gameplay.pBow->LinkRenderers();
	m_Gameplay.pEnemySystem->LinkRenderers();

	//Terrain
	List<TowerGame::TerrainRenderComponent*> terrainComponents{};
	terrainComponents.Add(m_Gameplay.pTerrain);
	terrainComponents.Add(m_Gameplay.pTower);
	m_Rendering.pTerrainRenderer->Start({ terrainComponents });
}

void TowerApp::CreateArrows()
{
	Array<Rendering::V_PosColNorm> pivotVertices{};
	Array<int> pivotIndices{};
	Generation::ArrowGenerator::CreatePivotArrows(pivotVertices, pivotIndices, 16, { 0,0,0 });
	RenderSystems::GetSimpleRenderer().AddMesh(pivotVertices, pivotIndices);
}
