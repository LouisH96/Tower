#include "pch.h"
#include "EnemyCode.h"

#include <Animations\AnimationUtils.h>
#include <Geometry\Shapes\Line.h>
#include <Other\Random.h>
#include <Physics\CollisionDetection.h>
#include <TowerApp.h>
#include <Transform\WorldMatrix.h>

namespace TowerGame
{
using Enemy = EnemySystem::Enemy;
using Type = EnemySystem::Type;

void EnemyCode::UpdateEnemies(EnemySystem::Enemies& enemiesData)
{
	for (unsigned iType{ 0 }; iType < enemiesData.Types.GetSize(); ++iType)
	{
		Type& type{ enemiesData.Types[iType] };
		InvalidateList<Enemy>& enemies{ type.Enemies };
		for (unsigned iEnemy{ enemies.GetFirstIdx() }; iEnemy < enemies.GetEndIdx(); ++iEnemy)
		{
			Enemy& enemy{ enemies.Get(iEnemy) };
			if (!enemy.IsValid())
				continue;
			if (!EnemyCode::UpdateEnemy(enemiesData.Target, type, enemy))
				continue;
			switch (enemy.State)
			{
			case Enemy::State::Completed:
				enemies.Invalidate(iEnemy);
				break;
			}
		}
	}
}

bool EnemyCode::UpdateEnemy(const Float2& target, EnemySystem::Type& type, EnemySystem::Enemy& enemy)
{
	if (enemy.State == Enemy::State::Dead ||
		enemy.State == Enemy::State::Completed)
		return true;

	if (enemy.State == Enemy::State::Running)
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

	else if (enemy.State == Enemy::State::Falling)
	{
		UpdateFall(enemy);
		for (unsigned i = 0; i < enemy.Arrows.GetSize(); i++)
			SYSTEMS.Arrows.SetArrowTransform(enemy.Arrows[i].ArrowIdx, Transform::LocalToWorld(enemy.Arrows[i].Local, enemy.World));
	}

	return false;
}

void EnemyCode::InitSpawningData(EnemySystem::Spawning& spawn)
{
	spawn.IntervalChange = 0;
	spawn.IntervalChange2 = (spawn.MinSpawnInterval - spawn.SpawnInterval)
		/ (spawn.IntervalChangeDuration * spawn.IntervalChangeDuration);
}

void EnemyCode::UpdateEnemySpawning(EnemySystem::Spawning& spawn, EnemySystem::Enemies& enemies)
{
	//Progress SpawnTime
	spawn.NextSpawn -= DELTA_TIME;
	if (spawn.NextSpawn > 0)
		return;

	//Spawn
	SpawnEnemy(spawn, enemies);

	//Find next spawn time
	spawn.NextSpawn = spawn.SpawnInterval;
	const float rateChange{ spawn.IntervalChange2 * spawn.NextSpawn };
	spawn.IntervalChange += rateChange;
	spawn.SpawnInterval += spawn.IntervalChange * spawn.NextSpawn;
	spawn.IntervalChange += rateChange;
	spawn.SpawnInterval = Float::Max(spawn.SpawnInterval, spawn.MinSpawnInterval);
}

void EnemyCode::HitByArrow(
	const Transform& worldArrowTransform, int arrowIdx, EnemySystem::Enemy& enemy)
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

bool EnemyCode::IsColliding(
	const Line& line,
	const EnemySystem::Type& type, const EnemySystem::Enemy& enemy,
	Physics::CollisionDetection::Collision& hit)
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

		if (Physics::CollisionDetection::IsLineInTriangle(
			localBegin, localEnd,
			pb0, pb1, pb2,
			tbNormal,
			hit))
		{
			return true;
		}
	}
	return false;
}

void EnemyCode::SpawnEnemies(unsigned count, const EnemySystem::Spawning& spawnData, EnemySystem::Enemies& systemData)
{
	for (unsigned i{ 0 }; i < count; ++i)
		SpawnEnemy(spawnData, systemData);
}

EnemySystem::Enemy& EnemyCode::SpawnEnemy(const Float2& position, EnemySystem::Enemies& systemData)
{
	Type& type{ Random::Item(systemData.Types) };
	InvalidateList<Enemy>& enemies{ type.Enemies };

	Enemy* pEnemy{};
	enemies.Validate(pEnemy);

	Enemy& enemy{ enemies.Validate() };
	enemy.State = Enemy::State::Running;
	enemy.World.Position = Float3::FromXz(position);
	enemy.World.LookAt(Float3::FromXz(systemData.Target));
	enemy.Animator.SetAnimation(type.Animation);
	enemy.Arrows.Clear();

	return enemy;
}

EnemySystem::Enemy& EnemyCode::SpawnEnemy(const EnemySystem::Spawning& spawnData, EnemySystem::Enemies& systemData)
{
	return SpawnEnemyAtRelativeDistance(1.f, spawnData, systemData);
}

EnemySystem::Enemy& EnemyCode::SpawnEnemyAtRelativeDistance(float relDistance, const EnemySystem::Spawning& spawnData, EnemySystem::Enemies& systemData)
{
	const Float2 direction{ Random::UnitVector2() * spawnData.SpawnRadius * relDistance };
	const Float2 initPos{ systemData.Target + direction };

	return SpawnEnemy(initPos, systemData);
}

unsigned EnemyCode::CountNrEnemies(const EnemySystem::Enemies& systemData)
{
	unsigned count{ 0 };
	for (unsigned i{ 0 }; i < systemData.Types.GetSize(); ++i)
		count += systemData.Types[i].Enemies.GetSize();
	return count;
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
	using Enemy = EnemySystem::Enemy;
	using Enemies = InvalidateList<Enemy>;

	EnemySystem& system{ SYSTEMS.Enemies };

	//Activate shared resources
	rendering.Texture.ActivatePs();
	rendering.InputLayout.Activate();
	rendering.Instances.Activate(1);

	for (unsigned iType{ 0 }; iType < types.GetSize(); ++iType)
	{
		using Instance = EnemySystem::RenderInstance;

		EnemySystem::Type& type{ types[iType] };
		Enemies& enemies{ type.Enemies };
		const unsigned nrEnemiesInInstances{ rendering.Instances.GetCapacity() };
		const unsigned boneOffsetModulo{ type.NrEnemiesInBonesBuffer * type.Animation.GetNrBones() };

		//Active EnemyType resources
		type.Vertices.Activate(0);

		//Render all enemies
		unsigned iEnemy{ enemies.GetFirstIdx() };
		const unsigned iEnemyEnd{ enemies.GetEndIdx() };
		while (iEnemy < iEnemyEnd)
		{
			//Render in batches
			//	1. fill the InstanceBuffer
			//	2. fill the BonesBuffer
			//	3. Draw the enemies included in the BonesBuffer
			//	(as the InstanceBuffer should include more Enemies than the BonesBuffer)

			const unsigned iEnemyFirstInInstances{ iEnemy };

			//Fill InstanceBuffer
			Instance* pInstance{ rendering.Instances.BeginUpdateData() };
			const Instance* pInstanceBegin{ pInstance };
			const Instance* pInstanceEnd{ pInstance + nrEnemiesInInstances }; //end of instance buffer
			Instance copyInst{};

			for (; iEnemy < iEnemyEnd && pInstance < pInstanceEnd; ++iEnemy)
			{
				const Enemy& enemy{ type.Enemies.Get(iEnemy) };
				if (!enemy.IsValid())
					continue;
				copyInst.World = enemy.World.AsMatrix();
				WorldMatrix::TranslateRelativeXz(copyInst.World, -enemy.RootPos);

				*pInstance++ = copyInst;
				copyInst.BoneIdOffset += type.Animation.GetNrBones();
				copyInst.BoneIdOffset %= boneOffsetModulo;
			}
			rendering.Instances.EndUpdateData();
			const unsigned iEnemyEndInInstances{ iEnemy };
			iEnemy = iEnemyFirstInInstances;

			//Fill BonesBuffer
			unsigned iInstanceEnemy{ 0 };
			while (iEnemy < iEnemyEndInInstances)
			{
				unsigned iBoneEnemy{ 0 };

				Float4X4* pBone{ bones.StartUpdate() };
				const Float4X4* pBoneBegin{ pBone };
				for (; iEnemy < iEnemyEndInInstances && iBoneEnemy < type.NrEnemiesInBonesBuffer; ++iEnemy)
				{
					const Enemy& enemy{ type.Enemies.Get(iEnemy) };
					if (!enemy.IsValid())
						continue;
					enemy.Animator.GetBones().CopyTo(pBone);
					pBone += type.Animation.GetNrBones();
					++iBoneEnemy;
				}
				bones.EndUpdate();
				rendering.Instances.DrawAsInstances(type.Vertices.GetCapacity(), iBoneEnemy, iInstanceEnemy);
				iInstanceEnemy += iBoneEnemy;
			}
		}
	}
}

void EnemyCode::UpdateMove(const Float2& target, const Float2& movement, EnemySystem::Enemy& enemy)
{
	constexpr float TARGET_RANGE_SQRT{ 2.5f };
	constexpr float TARGET_RANGE{ TARGET_RANGE_SQRT * TARGET_RANGE_SQRT };

	if (enemy.World.Position.Xz().DistanceSq(target) <= TARGET_RANGE)
	{
		enemy.State = EnemySystem::Enemy::State::Completed;
		return;
	}

	enemy.World.MoveRelativeXz(movement);
	enemy.World.Position.y = SYSTEMS.Terrain.GetHeight(enemy.World.Position.Xz());
}

void EnemyCode::UpdateFall(EnemySystem::Enemy& enemy)
{
	const Quaternion rotation{ Quaternion::FromAxis(enemy.FallAxis, 200 * Constants::TO_RAD * Globals::DeltaTime) };
	enemy.World.Rotation.RotateBy(rotation);

	const Float3 head{ enemy.World.LocalToWorld({0, 1.8f,0}) };
	const float terrainHeight{ SYSTEMS.Terrain.GetHeight(head.Xz()) };

	if (head.y <= terrainHeight)
	{
		enemy.State = EnemySystem::Enemy::State::Dead;
	}
}

}