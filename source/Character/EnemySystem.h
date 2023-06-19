#pragma once
#include "Enemy.h"
#include "Rendering/Structs/VertexTypes.h"

class EnemySystem
{
public:
	EnemySystem(int nrEnemies, const Float2& target);
	~EnemySystem() = default;

	void LinkRenderers();
	void Update();

	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Array<Rendering::V_PosNorUv> m_Vertices{};
	Array<Enemy> m_Enemies;
	const Float2 m_Target;
};
