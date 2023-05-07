#pragma once
#include "Game/Transform.h"

struct TowerAppServices;
class Terrain;

class Enemy
{
public:
	Enemy() = default;
	Enemy(const Math::Float3& initPos);

	void Update(
		const TowerAppServices& services,
		const Math::Float2& target, float maxMovement);

	Game::Transform& GetTransform() { return m_World; }

private:
	Game::Transform m_World;
};

