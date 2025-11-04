#pragma once

#include "StaticMeshes\StaticModelSystem.h"
#include "Ui\TowerUi.h"
#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Character\Character.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Systems\Enemies\EnemySystem.h>
#include <Systems\Shadows\ShadowSystem.h>
#include <Systems\Shadows\ShadowSystem2.h>
#include <Systems\Skydome\SkyDomeSystem.h>
#include <Systems\StaticMeshes\StaticMeshSystem.h>
#include <Systems\Terrain\Terrain.h>
#include <Systems\Tower\Tower.h>
#include <TowerGameRenderer.h>

#include "Shadows\ShadowSystem2.h"

namespace TowerGame
{
class TowerSystems
{
public:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;

	//Class
	TowerSystems() = default;
	~TowerSystems();

	void Init();
	void EarlyUpdate();
	void Update();
	
	ArrowSystem Arrows;
	Bow Bow;
	Character Character;
	CollisionSystem Collisions;
	EnemySystem Enemies;
	ShadowSystem Shadows;
	ShadowSystem2 Shadows2;
	SkyDomeSystem Skydome;
	Terrain Terrain;
	Tower Tower;
	TowerUi Ui;
	StaticMeshSystem StaticMeshes;
	StaticModelSystem StaticModels;
	SimpleRenderer* pSimpleRenderer{};
};
}