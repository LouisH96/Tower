#pragma once

#include "EnemySystem.h"

namespace TowerGame
{
class EnemyCode
{
public:
	static void UpdateEnemy(const Float2& target, const EnemySystem::Type& type, EnemySystem::Enemy& enemy);
	static void HitByArrow(const Transform& worldArrowTransform, int arrowIdx, EnemySystem::Enemy& enemy);
	static bool IsColliding(
		const Line& line,
		const EnemySystem::Type& type,
		const EnemySystem::Enemy& enemy);

	static Float3 ToAnimationSpace(const Float3& worldSpace, const EnemySystem::Enemy& enemy);
	static List<EnemySystem::Vertex> CreateVertices(const MyEngine::Io::Fbx::FbxClass& fbx);
	static Rendering::VertexArray<EnemySystem::Vertex> CreateVertexBuffer(const List<EnemySystem::Vertex>& vertices);

	static void Render(Rendering::ConstantBuffer<Float4X4>& bones, EnemySystem::EnemiesRendering& rendering, List<EnemySystem::Type>& types);
private:
	static void UpdateMove(const Float2& target, const Float2& maxMovement, EnemySystem::Enemy& enemy);
	static void UpdateFall(EnemySystem::Enemy& enemy);
};
}