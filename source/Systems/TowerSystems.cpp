#include "pch.h"
#include "TowerSystems.h"

#include <Generation/Shapes/ArrowGenerator.h>

using namespace TowerGame;

TowerSystems::~TowerSystems()
{
	delete pSimpleRenderer;
}

void TowerSystems::Init()
{
	//TERRAIN & TOWER
	const Float2 terrainGenOriginToTower{ Float2{125} }; //position of tower relative to the terrain's generation origin
	const float minTerrainSizeAroundTower{ 150 }; //starting from the tower, there should be at least this amount of terrain in all directions

	const Float2 terrainNegSize{
		Float::Max(terrainGenOriginToTower.x, minTerrainSizeAroundTower),
		Float::Max(terrainGenOriginToTower.y, minTerrainSizeAroundTower) };
	const Float2 terrainPosSize{
		Float::Max(-terrainGenOriginToTower.x, minTerrainSizeAroundTower)
		+ terrainNegSize.x,
		Float::Max(-terrainGenOriginToTower.y, minTerrainSizeAroundTower)
		+ terrainNegSize.y
	};
	const Float2 terrainOrigin{ 0 };

	Tower::Desc towerDesc{};
	towerDesc.Position = Float3::FromXz(terrainOrigin + terrainGenOriginToTower);
	towerDesc.RoofSize = { 10,6 };
	towerDesc.Height = 8;

	Terrain.Init(Float3::FromXz(terrainOrigin), terrainPosSize, terrainNegSize);
	Tower.Init(towerDesc);

	//CHARACTER & BOW
	const Float3 characterPosition{
		towerDesc.Position + Float3::FromXz(towerDesc.RoofSize * .5) + Float3{ 0,towerDesc.Height, 0 }
		+ Float3{-2,6,-3} };
	Character.Init(characterPosition);

	//ENEMIES
	const Float2 enemiesTarget{ towerDesc.Position.Xz()
		+ Float2{0,-2.5f} };
	const float enemySpawnRadius{ minTerrainSizeAroundTower };
	Enemies.Init(100, enemiesTarget, enemySpawnRadius);

	//SIMPLE RENDERER
	pSimpleRenderer = Rendering::RendererFactory::CreateSimpleRenderer();

	//SHADOWS
	Shadows.Init(Character.GetCameraController().GetCamera());

	//TERRAIN-RENDERER
	StaticMeshSystem::MeshData staticMeshData{};
	Terrain.GenerateMeshIn(staticMeshData);
	StaticMeshes.Init(staticMeshData);

	StaticModelSystem::InitData staticModelData{};
	Tower.AddModelsIn(staticModelData);

	StaticModels.Init(staticModelData);

	//PIVOT-ARROWS
	Array<Rendering::V_PosColNorm> pivotVertices{};
	Array<int> pivotIndices{};
	Generation::ArrowGenerator::CreatePivotArrows(pivotVertices, pivotIndices, 16, { 0,0,0 });
	pSimpleRenderer->AddMesh(pivotVertices, pivotIndices);
}

void TowerSystems::EarlyUpdate()
{
	Character.Update();
}

void TowerSystems::Update()
{
	Bow.Update(Character.GetCameraController().GetTransform());
	Enemies.Update();
	Arrows.Update();
	Shadows.Update();

	Ui.Update();
}
