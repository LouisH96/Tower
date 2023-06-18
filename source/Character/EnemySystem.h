#pragma once
#include "Enemy.h"

class Bow;
struct TowerAppServices;
class Tower;

namespace MyEngine
{
	namespace Rendering
	{
		class R_LambertCam_Tex_Transform;
		class Texture;
		class Mesh;
	}
}

namespace MyEngine
{
	namespace Framework
	{
		class CoreServices;
	}
}

class TowerAppRenderer;

class EnemySystem
{
public:
	EnemySystem(
		TowerAppServices& services, 
		int nrEnemies, 
		const Float2& target, const Terrain& terrain);
	~EnemySystem();

	void Update(TowerAppServices& services);

	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Rendering::R_LambertCam_Tex_Transform& m_Renderer;
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pMesh{};

	Array<Enemy> m_Enemies;

	const Terrain& m_Terrain;
	const Float2 m_Target;
};
