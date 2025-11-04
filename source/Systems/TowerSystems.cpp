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
	const Float2 terrainSize{ 250,250 };

	Tower::Desc towerDesc{};
	towerDesc.Position = Float3::FromXz(terrainSize / 2);
	towerDesc.RoofSize = { 10,6 };
	towerDesc.Height = 8;

	Terrain.Init({}, terrainSize);
	Tower.Init(towerDesc);

	//CHARACTER & BOW
	const Float3 characterPosition{
		towerDesc.Position + Float3::FromXz(towerDesc.RoofSize * .5) + Float3{ 0,towerDesc.Height, 0 }


	+ Float3{-2,6,-3} };
	Character.Init(characterPosition);

	//ENEMIES
	Enemies.Init(100, towerDesc.Position.Xz());

	//SIMPLE RENDERER
	pSimpleRenderer = Rendering::RendererFactory::CreateSimpleRenderer();

	//SHADOWS
	Shadows.Init(Character);
	Shadows2.Init(Character.GetCameraController().GetCamera());

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
	Shadows.MoveShadow(Character);
	Shadows2.Update();

	Ui.Update();
}
