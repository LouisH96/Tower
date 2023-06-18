#include "pch.h"
#include "EnemySystem.h"

#include "../TowerAppRenderer.h"
#include "../Services/TowerAppServices.h"
#include "Framework/Resources.h"
#include "Io/Fbx/FbxClass.h"
#include "Rendering/Renderers/R_LambertCam_Tex_Transform.h"

using namespace Rendering;
using namespace Math;

EnemySystem::EnemySystem(
	TowerAppServices& services,
	int nrEnemies,
	const Float2& target, const Terrain& terrain)
	: m_Renderer{ services.Renderer.GetTransformRenderer() }
	, m_Enemies{ nrEnemies }
	, m_Terrain{ terrain }
	, m_Target{ target }
{
	//MESH
	const std::wstring meshPath{ Resources::Local(L"SK_Character_DarkElf_01.fbx") };
	Io::Fbx::FbxClass fbxModel{ meshPath };
	Io::Fbx::FbxClass::Geometry& geom = fbxModel.GetGeometries()[0];

	Array<V_PosNorUv> vertices{ geom.Points.GetSize() };
	for (int i = 0; i < geom.Points.GetSize(); i++)
		vertices[i] = V_PosNorUv{ geom.Points[i] * 0.01f, geom.Normals[i], geom.Uvs[i] };

	m_pMesh = Mesh::Create(vertices);

	//TEXTURE
	const std::wstring texturePath{ Resources::Local(L"FantasyRivals_Texture_01_A.png") };
	m_pTexture = new Texture(texturePath);

	//ENEMIES
	for (int i = 0; i < m_Enemies.GetSize(); i++)
	{
		const float angle = (rand() % 360) * Constants::TO_RAD;
		float x = cos(angle) * m_Terrain.GetSize().x / 2;
		float y = sin(angle) * m_Terrain.GetSize().y / 2;
		if (abs(x) < 5) x = x < 0 ? -5 : 5;
		if (abs(y) < 5) y = y < 0 ? -5 : 5;
		m_Enemies[i].GetTransform().Position = Float3{ target.x + x, 0, target.y + y };
		m_Enemies[i].GetTransform().LookAt(Float3::FromXz(m_Target));
		m_Renderer.AddEntry(*m_pMesh, *m_pTexture, m_Enemies[i].GetTransform());
	}

	//COLLIDABLES
	EnemiesCollidable& collidable{ services.Collision.Enemies };
	collidable.pEnemies = &m_Enemies;
	collidable.Points = { vertices.GetSize() };
	collidable.TriangleNormals = { vertices.GetSize() / 3 };
	for (int i = 0; i < vertices.GetSize(); i++)
		collidable.Points[i] = vertices[i].Pos;
	for (int iVertex = 0, iTriangle = 0; iVertex < vertices.GetSize(); iVertex += 3, iTriangle++)
		collidable.TriangleNormals[iTriangle] = vertices[iVertex].Normal;
}

EnemySystem::~EnemySystem()
{
	delete m_pTexture;
	delete m_pMesh;
}

void EnemySystem::Update(TowerAppServices& services)
{
	for (int i = 0; i < m_Enemies.GetSize(); i++)
	{
		m_Enemies[i].Update(services, m_Target, 1 * Globals::DeltaTime);
	}
}

void EnemySystem::OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy)
{
	enemy.HitByArrow(arrowTransform, arrowIdx);
}
