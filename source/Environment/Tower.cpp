#include "pch.h"
#include "Tower.h"

#include "../TowerAppRenderer.h"
#include "Framework/CoreServices.h"
#include "Rendering/State/Mesh.h"
#include "Rendering/Structs/VertexTypes.h"

using namespace Math;
using namespace Rendering;

Tower::Tower(const Framework::CoreServices& services)
{
	Generate(services.Gpu);
}

Tower::~Tower()
{
	delete m_pMesh;
}

void Tower::Register(const TowerAppRenderer& renderer) const
{
	renderer.GetTerrainRenderer().AddEntry(*m_pMesh);
}

bool Tower::IsColliding(const Float3& begin, const Float3& end) const
{
	Physics::CollisionDetection::Collision collision;
	return Physics::CollisionDetection::Detect(begin, end, m_Points, m_TriangleNormals, m_Indices, collision);
}

bool Tower::IsColliding(const Float3& begin, const Float3& end,
	Physics::CollisionDetection::Collision& collision) const
{
	return Physics::CollisionDetection::Detect(begin, end, m_Points, m_TriangleNormals, m_Indices, collision);
}

void Tower::Generate(const Gpu& gpu)
{
	const Float3 outerSize{ 6,8,6 };
	const Float3 position{ 10,0,10 }; //left-bot
	const Float3 color{ Float3::RgbNormalize(248,240,164) };

	Array<V_PosNorCol> vertices{ 4 * 5 }; //4 per side, 5 sides
	m_Points = { vertices.GetSize() };
	m_Indices = { 3 * 2 * 5 }; //3/triangle, 2/side
	m_TriangleNormals = { m_Indices.GetSize() / 3};

	//front
	Float3 right{ outerSize.x, 0,0 };
	Float3 up{ 0, outerSize.y,0 };
	Float3 origin{ position };
	int iVertex = 0;
	int iIndex = 0;
	int iTriangle = 0;
	AddPlane(right, up, origin, color, vertices, iVertex, iIndex, iTriangle);

	//right
	origin += right;
	right = Float3{ 0,0,outerSize.x };
	AddPlane(right, up, origin, color, vertices, iVertex, iIndex, iTriangle);

	//back
	origin += right;
	right = Float3{ -outerSize.x, 0, 0 };
	AddPlane(right, up, origin, color, vertices, iVertex, iIndex, iTriangle);

	//left
	origin += right;
	right = Float3{ 0,0,-outerSize.x };
	AddPlane(right, up, origin, color, vertices, iVertex, iIndex, iTriangle);

	//roof
	origin = position + up;
	right = Float3{ outerSize.x, 0,0 };
	up = Float3{ 0,0,outerSize.z };
	AddPlane(right, up, origin, color, vertices, iVertex, iIndex, iTriangle);

	//MESH
	m_pMesh = Mesh::Create(gpu, vertices, m_Indices);
}

void Tower::AddPlane(
	const Float3& right, const Float3& up,
	const Float3& origin,
	const Float3& color,
	Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
	int& indicesIdx, int& triangleNormalIdx)
{
	const Float3 normal{ up.Cross(right).Normalized() };

	//left-bot
	vertices[verticesIdx] = { origin, normal, color };
	m_Points[verticesIdx] = origin;
	m_Indices[indicesIdx] = verticesIdx;

	//left-top
	vertices[++verticesIdx] = { origin + up, normal, color };
	m_Points[verticesIdx] = origin + up;
	m_Indices[++indicesIdx] = verticesIdx;

	//right-top
	vertices[++verticesIdx] = { origin + up + right, normal, color };
	m_Points[verticesIdx] = origin + up + right;
	m_Indices[++indicesIdx] = verticesIdx;
	m_Indices[++indicesIdx] = verticesIdx;

	//right-bot
	vertices[++verticesIdx] = { origin + right, normal, color };
	m_Points[verticesIdx] = origin + right;
	m_Indices[++indicesIdx] = verticesIdx;
	m_Indices[++indicesIdx] = verticesIdx - 3;

	m_TriangleNormals[triangleNormalIdx++] = normal;
	m_TriangleNormals[triangleNormalIdx++] = normal;

	verticesIdx++;
	indicesIdx++;
}
