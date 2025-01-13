#include "pch.h"
#include "EnemySystem.h"

#include "Environment/Terrain.h"
#include "Framework/Resources.h"
#include "Io/Fbx/FbxClass.h"
#include "Rendering/Renderers/R_LambertCam_Tex_Tran_Inst.h"
#include "Services/CollisionService.h"
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"
#include "Renderer/TowerGameRenderer.h"

#include <Timing\Counter.h>
#include <Io\Fbx\Wrapping\FbxData.h>

using namespace Animations;
using namespace Io::Fbx;
using namespace TowerGame;
using namespace Rendering;

const InputLayout::Element EnemySystem::Instance::ELEMENTS[]{
	{"WORLD", InputLayout::ElementType::Float4X4},
	{"BONE_OFFSET", InputLayout::ElementType::Uint}
};

EnemySystem::EnemySystem(int nrEnemies, const Float2& target)
	: m_InputLayout{ InputLayout::FromTypes2<Vertex, Instance>() }
	, m_Enemies{ nrEnemies }
	, m_Target{ target }
	, m_Shader{ Resources::Local(L"Enemy.hlsl") }
{
	//MESH
	constexpr float modelScale{ .01f };
	const std::wstring meshPath{ Resources::Local(L"SK_Character_DarkElf_01.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath, modelScale };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<Vertex> vertices{ geom.Points.GetSize() };
	for (unsigned i = 0; i < geom.Points.GetSize(); i++)
	{
		Vertex& vertex{ vertices[i] };
		vertex.Pos = geom.Points[i];
		vertex.Normal = geom.Normals[i];
		vertex.Uv = geom.Uvs[i];

		const Io::Fbx::FbxClass::BlendData& blendData{ geom.Weights[i] };
		vertex.BoneIds = blendData.IndicesAsInt4();
		vertex.BoneWeights = blendData.WeightsAsFloat4();
	}

	m_InstanceArray = InstanceArray{ PtrRangeConst<Vertex>{vertices}, sizeof(Instance), Uint::Cast(nrEnemies) };
	m_InstanceArray.SetInstanceCount(Uint::Cast(nrEnemies));
	m_Texture = Texture{ Resources::Local(L"FantasyRivals_Texture_01_A.png") };

	//ANIMATIONS
	const std::wstring animationPath{ Resources::Local(L"Zombie Running.fbx") };
	Io::Fbx::FbxClass animationFbx{ animationPath, modelScale };

	const FbxAnimation& animation{ animationFbx.GetAnimations().First() };
	m_Animation = Animation{ animationFbx, animation };
	m_BatchLimit = TowerGameRenderer::BONES_BUFFER_SIZE / m_Animation.GetNrBones();

	//ENEMIES
	for (unsigned i = 0; i < m_Enemies.GetSize(); i++)
	{
		const float angle = (rand() % 360) * Constants::TO_RAD;
		float x = cos(angle) * GameplaySystems::GetTerrain().GetSize().x / 2.f;
		float y = sin(angle) * GameplaySystems::GetTerrain().GetSize().y / 2.f;
		if (abs(x) < 5.f) x = x < 0.f ? -5.f : 5.f;
		if (abs(y) < 5.f) y = y < 0.f ? -5.f : 5.f;
		const Float3 initPos{ target.x + x, 0, target.y + y };

		m_Enemies[i] = Enemy{ m_Animation, initPos };
		m_Enemies[i].GetTransform().LookAt(Float3::FromXz(m_Target));
	}

	//COLLIDABLES
	EnemiesCollidable& collidable{ GameplaySystems::GetCollisionService().Enemies };
	collidable.pEnemies = &m_Enemies;
	collidable.Points = { vertices.GetSize() };
	collidable.TriangleNormals = { vertices.GetSize() / 3 };
	for (unsigned i = 0; i < vertices.GetSize(); i++)
		collidable.Points[i] = vertices[i].Pos;
	for (unsigned iVertex = 0, iTriangle = 0; iVertex < vertices.GetSize(); iVertex += 3, iTriangle++)
		collidable.TriangleNormals[iTriangle] = vertices[iVertex].Normal;
}

void EnemySystem::LinkRenderers()
{
	/*RenderSystems::InstanceTransformRenderer& renderer{ RenderSystems::GetInstanceTransformRenderer() };
	const int modelIdx{ renderer.CreateModel({ Resources::Local(L"FantasyRivals_Texture_01_A.png") }, m_Vertices.GetData(), m_Vertices.GetSize()) };

	for (unsigned i = 0; i < m_Enemies.GetSize(); i++)
		renderer.AddInstance(modelIdx, m_Enemies[i].GetTransform());*/
}

void EnemySystem::Update()
{
	//Update cpu data
	for (unsigned i = 0; i < m_Enemies.GetSize(); i++)
		m_Enemies[i].Update(m_Target, 50 * Globals::DeltaTime, m_Animation);

	//Update gpu data
	Instance* pInstances{ m_InstanceArray.BeginUpdateInstances<Instance>() };
	for (unsigned iEnemy{ 0 }; iEnemy < m_Enemies.GetSize(); ++iEnemy)
	{
		const Enemy& enemy{ m_Enemies[iEnemy] };
		Instance& instance{ pInstances[iEnemy] };
		instance.World = enemy.GetTransform().AsMatrix();
		instance.BoneIdOffset = (iEnemy % m_BatchLimit) * m_Animation.GetNrBones();
	}
	m_InstanceArray.EndUpdateInstances();
}

void EnemySystem::Render_Internal(
	Rendering::ConstantBuffer<Float4X4>& bones)
{
	//
	m_Texture.ActivatePs();
	m_InputLayout.Activate();

	//
	//Timing::Counter counter{};
	const unsigned bonesPerEnemy{ m_Animation.GetNrBones() };

	for (unsigned iEnemy{ 0 }; iEnemy < m_Enemies.GetSize();)
	{
		const unsigned firstEnemy{ iEnemy };
		unsigned iBatchEnemy{ 0 };

		Float4X4* pBone{ bones.StartUpdate() };
		for (;
			iBatchEnemy < m_BatchLimit && iEnemy < m_Enemies.GetSize();
			++iBatchEnemy, ++iEnemy)
		{
			const Enemy& enemy{ m_Enemies[iEnemy] };
			enemy.GetAnimator().GetBones().CopyTo(pBone);
			pBone += bonesPerEnemy;
		}
		bones.EndUpdate();

		m_InstanceArray.Draw(firstEnemy, iBatchEnemy);
	}

	//counter.End("Counter");
}

void EnemySystem::OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy)
{
	enemy.HitByArrow(arrowTransform, arrowIdx);
}
