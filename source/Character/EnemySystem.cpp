#include "pch.h"
#include "EnemySystem.h"

#include "Environment/Terrain.h"
#include "Framework/Resources.h"
#include "Io/Fbx/FbxClass.h"
#include "Rendering/Renderers/R_LambertCam_Tex_Tran_Inst.h"
#include "Services/CollisionService.h"
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"

using namespace Rendering;

EnemySystem::EnemySystem(int nrEnemies, const Float2& target)
	: m_Enemies{ nrEnemies }
	, m_Target{ target }
{
	//MESH
	const std::wstring meshPath{ Resources::Local(L"SK_Character_DarkElf_01.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	m_Vertices = { geom.Points.GetSize() };
	for (int i = 0; i < geom.Points.GetSize(); i++)
		m_Vertices[i] = V_PosNorUv{ geom.Points[i] * 0.01f, geom.Normals[i], geom.Uvs[i] };

	//ENEMIES
	for (int i = 0; i < m_Enemies.GetSize(); i++)
	{
		const float angle = (rand() % 360) * Constants::TO_RAD;
		float x = cos(angle) * GameplaySystems::GetTerrain().GetSize().x / 2;
		float y = sin(angle) * GameplaySystems::GetTerrain().GetSize().y / 2;
		if (abs(x) < 5) x = x < 0 ? -5 : 5;
		if (abs(y) < 5) y = y < 0 ? -5 : 5;
		m_Enemies[i].GetTransform().Position = Float3{ target.x + x, 0, target.y + y };
		m_Enemies[i].GetTransform().LookAt(Float3::FromXz(m_Target));
	}

	//COLLIDABLES
	EnemiesCollidable& collidable{ GameplaySystems::GetCollisionService().Enemies };
	collidable.pEnemies = &m_Enemies;
	collidable.Points = { m_Vertices.GetSize() };
	collidable.TriangleNormals = { m_Vertices.GetSize() / 3 };
	for (int i = 0; i < m_Vertices.GetSize(); i++)
		collidable.Points[i] = m_Vertices[i].Pos;
	for (int iVertex = 0, iTriangle = 0; iVertex < m_Vertices.GetSize(); iVertex += 3, iTriangle++)
		collidable.TriangleNormals[iTriangle] = m_Vertices[iVertex].Normal;
}

void EnemySystem::LinkRenderers()
{
	RenderSystems::InstanceTransformRenderer& renderer{ RenderSystems::GetInstanceTransformRenderer() };
	const int modelIdx{ renderer.CreateModel({ Resources::Local(L"FantasyRivals_Texture_01_A.png") }, m_Vertices.GetData(), m_Vertices.GetSizeU()) };

	for (int i = 0; i < m_Enemies.GetSize(); i++)
		renderer.AddInstance(modelIdx, m_Enemies[i].GetTransform());
}

void EnemySystem::Update()
{
	for (int i = 0; i < m_Enemies.GetSize(); i++)
	{
		m_Enemies[i].Update(m_Target, 1 * Globals::DeltaTime);
	}
}

void EnemySystem::OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy)
{
	enemy.HitByArrow(arrowTransform, arrowIdx);
}
