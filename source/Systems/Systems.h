#pragma once

#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Character\Character.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Systems\Enemies\EnemySystem.h>
#include <Systems\Shadows\ShadowSystem.h>
#include <Systems\Skydome\SkyDomeSystem.h>
#include <Systems\StaticMeshes\StaticMeshSystem.h>
#include <Systems\Terrain\Terrain.h>
#include <Systems\Tower\Tower.h>
#include <TowerGameRenderer.h>
#include "Ui\TowerUi.h"

namespace TowerGame
{
class Systems
{
public:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;

	//Class
	Systems() = default;
	~Systems();

	void Init();
	void EarlyUpdate();
	void Update();
	
	ArrowSystem Arrows;
	Bow Bow;
	Character Character;
	CollisionSystem Collisions;
	EnemySystem Enemies;
	ShadowSystem Shadows;
	SkyDomeSystem Skydome;
	Terrain Terrain;
	Tower Tower;
	TowerUi Ui;
	StaticMeshSystem StaticMeshes;
	SimpleRenderer* pSimpleRenderer{};
};
}