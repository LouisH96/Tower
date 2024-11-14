#pragma once
#include "Enemy.h"

#include <Rendering\Structs\VertexTypes.h>
#include <Rendering\Structs\InstanceTypes.h>
#include <Rendering\DrawData\InstanceArray.h>

namespace TowerGame
{

class EnemySystem
{
public:
	using Vertex = Rendering::V_PosNorUv;
	using Instance = Rendering::I_ModelMatrices;

	EnemySystem(int nrEnemies, const Float2& target);
	~EnemySystem() = default;

	void LinkRenderers();
	void Update();
	void Render();

	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Array<Enemy> m_Enemies;
	const Float2 m_Target;

	//Rendering
	Rendering::Texture m_Texture;
	Rendering::InstanceArray m_InstanceArray;
};
}