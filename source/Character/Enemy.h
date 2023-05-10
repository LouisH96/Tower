#pragma once
#include "Game/Transform.h"

struct TowerAppServices;
class Terrain;

class Enemy
{
public:
	Enemy();
	Enemy(const Float3& initPos);

	void Update(
		const TowerAppServices& services,
		const Float2& target, float maxMovement);
	void UpdateArrows() const;

	Game::Transform& GetTransform() { return m_World; }
	const Game::Transform& GetTransform() const { return m_World; }

	void HitByArrow(Game::Transform& worldArrowTransform);

	static constexpr float HEIGHT = 2.f;

private:
	struct AttachedArrow
	{
		Game::Transform* pWorld;
		Game::Transform Local;
	};

	Game::Transform m_World;
	Float3 m_FallOverAxis;

	std::vector<AttachedArrow> m_Arrows{};

	void UpdateMove(
		const TowerAppServices& services,
		const Float2& target, float maxMovement);

	void UpdateFall(const TowerAppServices& services);
};

