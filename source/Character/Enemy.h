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
	void UpdateArrows() const;

	Game::Transform& GetTransform() { return m_World; }
	const Game::Transform& GetTransform() const { return m_World; }

	void AddArrow(Game::Transform& worldArrowTransform);

	static constexpr float HEIGHT = 2.f;

private:
	struct AttachedArrow
	{
		Game::Transform* pWorld;
		Game::Transform Local;
	};

	Game::Transform m_World;

	std::vector<AttachedArrow> m_Arrows{};
};

