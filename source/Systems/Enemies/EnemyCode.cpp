#include "pch.h"
#include "EnemyCode.h"

#include <Animations\AnimationUtils.h>
#include <Geometry\Shapes\Line.h>
#include <Physics\CollisionDetection.h>
#include <TowerApp.h>
#include <Transform\WorldMatrix.h>

namespace TowerGame
{
void EnemyCode::UpdateEnemy(const Float2& target, const EnemySystem::Type& type, EnemySystem::Enemy& enemy)
{
	if (enemy.State == EnemySystem::Enemy::State::Dead)
		return;

	if (enemy.State == EnemySystem::Enemy::State::Running)
	{
		const unsigned nrLoops{ enemy.Animator.ProgressTime(type.Animation) };

		const Float2 newRootPos{ enemy.Animator.GetRootXz() };
		const Float2 rootMotion{
			(newRootPos - enemy.RootPos)
			+ type.RootAnimationMovement * nrLoops
		};

		enemy.RootPos = newRootPos;
		UpdateMove(target, rootMotion, enemy);
	}
	else if (enemy.State == EnemySystem::Enemy::State::Falling)
	{
		UpdateFall(enemy);
		for (unsigned i = 0; i < enemy.Arrows.GetSize(); i++)
			SYSTEMS.Arrows.SetArrowTransform(enemy.Arrows[i].ArrowIdx, Transform::LocalToWorld(enemy.Arrows[i].Local, enemy.World));
	}
}

void EnemyCode::HitByArrow(const Transform& worldArrowTransform, int arrowIdx, EnemySystem::Enemy& enemy)
{
	enemy.Arrows.Add(
		{
			arrowIdx,
			worldArrowTransform
		});
	enemy.Arrows[enemy.Arrows.GetSize() - 1].Local.SetRelativeTo(enemy.World);

	//fall over
	enemy.FallAxis =
		Float3{ 0,1,0 }.Cross(worldArrowTransform.Rotation.GetForward()).Normalized();
	enemy.State = EnemySystem::Enemy::State::Falling;
}

bool EnemyCode::IsColliding(const Line& line, const EnemySystem::Type& type, const EnemySystem::Enemy& enemy)
{
	using namespace Physics;

	const float cylinderBot{ enemy.World.Position.y };
	const float cylinderTop{ cylinderBot + type.Height };

	//Show enemy bounds 
	//	to pick a good Radius & Height
	//	could be calculated but then it should take the animation in account
	/*DebugRenderer::DrawCube(
		CubeAA{ enemy.World.Position - Float3{type.Radius, 0, type.Radius}, 
		{type.Radius * 2,type.Height,type.Radius * 2} }, Color::Red);*/

	//cube overlap check
	if (!(
		Float::HasOverlap(cylinderBot, cylinderTop, line.a.y, line.b.y)
		&& Float::HasOverlap(enemy.World.Position.x - type.Radius, enemy.World.Position.x + type.Radius, line.a.x, line.b.x)
		&& Float::HasOverlap(enemy.World.Position.z - type.Radius, enemy.World.Position.z + type.Radius, line.a.z, line.b.z)))
		return false;

	const Float3 localBegin{ ToAnimationSpace(line.a, enemy) };
	const Float3 localEnd{ ToAnimationSpace(line.b, enemy) };

	CollisionDetection::Collision collision;

	const Array<Float4X4>& bones{ enemy.Animator.GetBones() };

	using Vertex = EnemySystem::Type::CollisionVertex;
	for (
		unsigned iVertex{ 2 };
		iVertex < type.CollisionVertices.GetSize();
		iVertex += 3)
	{
		const Vertex& v0{ type.CollisionVertices[iVertex - 2] };
		const Vertex& v1{ type.CollisionVertices[iVertex - 1] };
		const Vertex& v2{ type.CollisionVertices[iVertex] };

		const Float3 pb0{ AnimationUtils::Blend(v0.Point, v0.BoneIndices, v0.BoneWeights, bones) };
		const Float3 pb1{ AnimationUtils::Blend(v1.Point, v1.BoneIndices, v1.BoneWeights, bones) };
		const Float3 pb2{ AnimationUtils::Blend(v2.Point, v2.BoneIndices, v2.BoneWeights, bones) };
		const Float3 tbNormal{ Triangle::FindNormal(pb0,pb1,pb2) };

		CollisionDetection::Collision collision{};
		if (Physics::CollisionDetection::IsLineInTriangle(
			localBegin, localEnd,
			pb0, pb1, pb2,
			tbNormal,
			collision))
		{
			return true;
		}
	}
	return false;
}

Float3 EnemyCode::ToAnimationSpace(const Float3& worldSpace, const EnemySystem::Enemy& enemy)
{
	Float3 local{ enemy.World.WorldToLocal(worldSpace) };
	local.AddXz(enemy.RootPos);
	return local;
}

List<EnemySystem::Vertex> EnemyCode::CreateVertices(const Io::Fbx::FbxClass& fbx)
{
	using namespace Io::Fbx;

	//Vertices
	List<EnemySystem::Vertex> vertices{};
	for (unsigned iGeom{ 0 }; iGeom < fbx.GetGeometries().GetSize(); ++iGeom)
	{
		const FbxClass::Geometry& geom{ fbx.GetGeometry(iGeom) };
		vertices.EnsureIncrease(geom.Points.GetSize());

		for (unsigned iVertex = 0; iVertex < geom.Points.GetSize(); iVertex++)
		{
			EnemySystem::Vertex& vertex{ vertices.AddEmpty() };
			vertex.Pos = geom.Points[iVertex];
			vertex.Normal = geom.Normals[iVertex];
			vertex.Uv = geom.Uvs[iVertex];

			const FbxClass::BlendData& blendData{ geom.Weights[iVertex] };
			vertex.BoneIds = blendData.IndicesAsInt4();
			vertex.BoneWeights = blendData.WeightsAsFloat4();
		}
	}
	return vertices;
}

Rendering::VertexArray<EnemySystem::Vertex> EnemyCode::CreateVertexBuffer(const List<EnemySystem::Vertex>& vertices)
{
	return Rendering::VertexArray<EnemySystem::Vertex>{{vertices}, true};
}

void EnemyCode::Render(Rendering::ConstantBuffer<Float4X4>& bones, EnemySystem::EnemiesRendering& rendering, List<EnemySystem::Type>& types)
{
	EnemySystem& system{ SYSTEMS.Enemies };

	//Activate shared resources
	rendering.Texture.ActivatePs();
	rendering.InputLayout.Activate();
	rendering.Instances.Activate(1);

	for (unsigned iType{ 0 }; iType < types.GetSize(); ++iType)
	{
		using Instance = EnemySystem::RenderInstance;

		const unsigned nrEnemiesInInstances{ rendering.Instances.GetCapacity() };
		EnemySystem::Type& type{ types[iType] };

		//Active EnemyType resources
		type.Vertices.Activate(0);

		//Render all enemies
		unsigned iEnemy{ 0 };
		while (iEnemy < type.Enemies.GetSize())
		{
			//Render in batches
			//	1. fill the InstanceBuffer
			//	2. fill the BonesBuffer
			//	3. Draw the enemies included in the BonesBuffer
			//	(as the InstanceBuffer should include more Enemies than the BonesBuffer)

			//Fill InstanceBuffer
			const unsigned firstInInstances{ iEnemy };
			const unsigned endForInstances{
				Uint::Min(firstInInstances + nrEnemiesInInstances, type.Enemies.GetSize()) };

			Instance* pInstance{ rendering.Instances.BeginUpdateData() };
			for (; iEnemy < endForInstances; ++iEnemy, ++pInstance)
			{
				const EnemySystem::Enemy& enemy{ type.Enemies[iEnemy] };
				pInstance->World = enemy.World.AsMatrix();
				pInstance->BoneIdOffset = ((iEnemy - firstInInstances) % type.NrEnemiesInBonesBuffer) * type.Animation.GetNrBones();
				WorldMatrix::TranslateRelativeXz(pInstance->World, -enemy.RootPos);
			}
			rendering.Instances.EndUpdateData();
			iEnemy = firstInInstances;

			//Fill BonesBuffer
			while (iEnemy < endForInstances)
			{
				const unsigned firstInBones{ iEnemy };
				const unsigned endForBones{
					Uint::Min(firstInBones + type.NrEnemiesInBonesBuffer, endForInstances) };

				Float4X4* pBone{ bones.StartUpdate() };
				for (; iEnemy < endForBones; ++iEnemy, pBone += type.Animation.GetNrBones())
				{
					const EnemySystem::Enemy& enemy{ type.Enemies[iEnemy] };
					enemy.Animator.GetBones().CopyTo(pBone);
				}
				bones.EndUpdate();
				rendering.Instances.DrawAsInstances(type.Vertices.GetCapacity(), endForBones - firstInBones, firstInBones - firstInInstances);
			}
		}
	}
}

void EnemyCode::UpdateMove(const Float2& target, const Float2& movement, EnemySystem::Enemy& enemy)
{
	constexpr float minTargetDistanceSq = 6 * 6;

	if (enemy.World.Position.Xz().DistanceSq(target) <= minTargetDistanceSq)
	{
		enemy.State = EnemySystem::Enemy::State::Completed;
		return;
	}

	enemy.World.MoveRelativeXz(movement);
	enemy.World.Position.y = SYSTEMS.Terrain.GetHeight(enemy.World.Position.Xz());
}

void EnemyCode::UpdateFall(EnemySystem::Enemy& enemy)
{
	const Quaternion rotation{ Quaternion::FromAxis(enemy.FallAxis, 150 * Constants::TO_RAD * Globals::DeltaTime) };
	enemy.World.Rotation.RotateBy(rotation);

	const Float3 head{ enemy.World.LocalToWorld({0, 1.8f,0}) };
	const float terrainHeight{ SYSTEMS.Terrain.GetHeight(head.Xz()) };

	if (head.y <= terrainHeight)
	{
		enemy.State = EnemySystem::Enemy::State::Dead;
	}
}

}