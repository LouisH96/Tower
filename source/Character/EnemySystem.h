#pragma once
#include "Enemy.h"

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
		class Texture;
	}
}

class EnemySystem
{
public:
	EnemySystem(int nrEnemies, const Float2& target);
	~EnemySystem();

	void LinkRenderers();
	void Update();

	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pMesh{};

	Array<Enemy> m_Enemies;

	const Float2 m_Target;
};
