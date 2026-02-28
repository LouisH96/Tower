#include "pch.h"
#include "EnemySystem.h"

#include "EnemyCode.h"
#include <Other\Random.h>
#include <TowerApp.h>
#include <Rendering\State\InputLayout.h>

using namespace Animations;
using namespace Rendering;
using namespace TowerGame;

const InputLayout::Element EnemySystem::RenderInstance::ELEMENTS[]{
	{"WORLD", InputLayout::ElementType::Float4X4},
	{"BONE_OFFSET", InputLayout::ElementType::Uint}
};

EnemySystem::EnemySystem()
{
	constexpr unsigned ENEMIES_PER_INSTANCEBUFFER{ 100 };
	m_Rendering.Instances = { sizeof(RenderInstance), ENEMIES_PER_INSTANCEBUFFER, false };
	CreateTypes();
}

void EnemySystem::Init(unsigned nrEnemies, const Float2& target)
{
	m_Enemies.Target = target;
	for (unsigned i = 0; i < nrEnemies; i++)
	{
		//Random position
		const float angle{ Random::Angle() };
		const float x{ cos(angle) * SYSTEMS.Terrain.GetSize().x / 2.f };
		const float y{ sin(angle) * SYSTEMS.Terrain.GetSize().y / 2.f };
		const Float3 initPos{ target.x + x, 0, target.y + y };

		Type& type{ Random::Item(m_Enemies.Types) };
		List<Enemy>& enemies{ type.Enemies };

		Enemy& enemy{ enemies.AddAndGet(Enemy{ type.Animation, initPos }) };
		enemy.World.LookAt(Float3::FromXz(m_Enemies.Target));
	}
}

void EnemySystem::Update()
{
	for (unsigned iType{ 0 }; iType < m_Enemies.Types.GetSize(); ++iType)
	{
		Type& type{ m_Enemies.Types[iType] };
		List<Enemy>& enemies{ type.Enemies };
		for (unsigned iEnemy{ 0 }; iEnemy < enemies.GetSize(); ++iEnemy)
			EnemyCode::UpdateEnemy(m_Enemies.Target, type, enemies[iEnemy]);
	}
}

void EnemySystem::OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy)
{
	EnemyCode::HitByArrow(arrowTransform, arrowIdx, enemy);
}

EnemySystem::Enemy* EnemySystem::IsColliding(const Line& line)
{
	for (unsigned iType{ 0 }; iType < m_Enemies.Types.GetSize(); ++iType)
	{
		const Type& type{ m_Enemies.Types[iType] };
		for (unsigned iEnemy{ 0 }; iEnemy < type.Enemies.GetSize(); ++iEnemy)
		{
			const Enemy& enemy{ type.Enemies[iEnemy] };
			if (EnemyCode::IsColliding(line, type, enemy))
				return &m_Enemies.Types[iType].Enemies[iEnemy];
		}
	}
	return nullptr;
}

unsigned EnemySystem::CreateType(TypeDesc desc)
{
	using namespace Io::Fbx;
	using namespace Rendering;

	//Absolute paths
	desc.ModelPath = Resources::Local(desc.ModelPath);
	desc.AnimationPath = Resources::Local(desc.AnimationPath);
	for (unsigned iAttach{ 0 }; iAttach < desc.Attachments.GetSize(); ++iAttach)
	{
		std::wstring& path{ desc.Attachments[iAttach].FbxPath };
		path = Resources::Local(path);
	}

	//Create Type
	Type type{};
	type.Height = desc.Height;
	type.Radius = desc.Radius;

	//Model
	FbxClass fbx{ desc.ModelPath, desc.Scale };

	//Attachments
	for (unsigned iAttach{ 0 }; iAttach < desc.Attachments.GetSize(); ++iAttach)
	{
		const TypeDesc::Attachment& attach{ desc.Attachments[iAttach] };
		const FbxClass fbxAttach{ attach.FbxPath, desc.Scale };
		fbx.Attach(fbxAttach, attach.ParentJoint);
	}

	//Vertices
	const List<Vertex> vertices{ EnemyCode::CreateVertices(fbx) };
	type.Vertices = EnemyCode::CreateVertexBuffer(vertices);

	//Animations
	FbxClass animationFbx{ desc.AnimationPath, desc.Scale };
	type.Animation = Animation{ animationFbx, animationFbx.GetAnimations().First() };
	type.RootAnimationMovement = type.Animation.GetFullRootMotionXz();
	type.NrEnemiesInBonesBuffer = TowerGameRenderer::BONES_BUFFER_SIZE / type.Animation.GetNrBones();

	//Collision
	type.CollisionVertices = { vertices.GetSize() };
	for (unsigned iVertex = 0; iVertex < vertices.GetSize(); iVertex++)
	{
		const Vertex& modelVertex{ vertices[iVertex] };

		Type::CollisionVertex colVertex{};
		colVertex.Point = modelVertex.Pos;
		colVertex.BoneIndices = modelVertex.BoneIds;
		colVertex.BoneWeights = modelVertex.BoneWeights;

		type.CollisionVertices.Add(colVertex);
	}
	m_Enemies.Types.Add(std::move(type));
	return m_Enemies.Types.LastIdx();
}

void EnemySystem::CreateTypes()
{
	TypeDesc desc{};
	desc.Scale = .01f;

	//Nomad
	desc.ModelPath = L"Chr_Nomad_Male_01.fbx";
	desc.AnimationPath = L"Unarmed Run Forward(v25).fbx";
	desc.Attachments.Add(
		{ L"SM_Chr_Attach_Hair_Wild_01.fbx", "Head" },
		{ L"SM_Chr_Attach_Ear_01.fbx", "Head" },
		{ L"SM_Chr_Attach_Mask_04.fbx", "Head" }
	);
	desc.Height = 1.8f;
	desc.Radius = .5f;
	CreateType(desc);

	//Skeleton
	desc.Attachments.Clear();
	desc.ModelPath = L"Chr_Skeleton_03.fbx";
	desc.AnimationPath = L"Zombie Running(v25).fbx";
	CreateType(desc);
}

void EnemySystem::Render_Internal(Rendering::ConstantBuffer<Float4X4>& bones)
{
	EnemyCode::Render(bones, m_Rendering, m_Enemies.Types);
}

EnemySystem::Enemy::Enemy()
	: State{ State::Running }
{
}

EnemySystem::Enemy::Enemy(const Animations::Animation& animation, const Float3& initPos)
	: Animator{ animation }
	, World{ initPos, {} }
	, State{ State::Running }
{
}

EnemySystem::EnemiesRendering::EnemiesRendering()
	: InputLayout{ InputLayout::FromTypes2<Vertex, RenderInstance>() }
	, Shader{ Resources::Local(L"Enemy.hlsl") }
	, Texture{ Resources::Local(L"Dungeons_2_Texture_01_A.png") }
{
}
