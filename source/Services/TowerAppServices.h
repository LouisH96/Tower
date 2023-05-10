#pragma once
#include "CollisionService.h"
#include "../TowerAppRenderer.h"

class EnemySystem;

namespace MyEngine
{
	namespace Framework
	{
		class CoreServices;
	}
}

struct TowerAppServices
{
	const Framework::CoreServices& Core;
	TowerAppRenderer Renderer;
	CollisionService Collision;
	EnemySystem* pEnemySystem{};
};

