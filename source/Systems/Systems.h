#pragma once

#include <Systems\Arrows\ArrowSystem.h>
#include <Systems\Bow\Bow.h>
#include <Systems\Character\Character.h>
#include <Systems\Collisions\CollisionService.h>
#include <Systems\Enemies\EnemySystem.h>
#include <Systems\Shadows\ShadowRenderer.h>
#include <Systems\Skydome\SkyDomeRenderer.h>
#include <Systems\Terrain\TerrainRenderer.h>
#include <Systems\Terrain\Terrain.h>
#include <Systems\Terrain\Tower.h>
#include <TowerGameRenderer.h>

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
	CollisionService Collisions;
	EnemySystem Enemies;
	ShadowRenderer Shadows;
	SkyDomeRenderer Skydome;
	Terrain Terrain;
	Tower Tower;
	TerrainRenderer TerrainRenderer;
	SimpleRenderer* pSimpleRenderer;
};
}