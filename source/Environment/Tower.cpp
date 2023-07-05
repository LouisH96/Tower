#include "pch.h"
#include "Tower.h"

#include "Rendering/State/Mesh.h"
#include "Rendering/Structs/VertexTypes.h"
#include "Services/CollisionService.h"
#include "Services/GameplaySystems.h"
#include "Services/RenderSystems.h"

using namespace Rendering;

Tower::Tower(const Float3& position, const Float2& roofSize, float height)
	: m_Position{ position }
	, m_RoofSize{ roofSize }
	, m_Height{ height }
{
}

Tower::~Tower()
{
	delete m_pMesh;
}

void Tower::LinkGameplay()
{
	Generate(GameplaySystems::GetCollisionService().Tower,
		m_Position, m_RoofSize, m_Height);
}

void Tower::LinkRenderers()
{
	RenderSystems::GetTerrainRenderer().AddEntry(*m_pMesh);
}

void Tower::Generate(MeshCollidable& collidable,
	const Float3& position, const Float2& roofSize,
	float towerHeight)
{
	constexpr float rampWidth{ 2 };
	const float rampHeight{ towerHeight / 4 };
	const Float3 color{ Float3::Color(248,240,164) };

	constexpr int nrPlanes{ 5 + 4 * 4 };
	Array<V_PosNorCol> vertices{ 4 * nrPlanes }; //4 per side
	collidable.Points = { vertices.GetSize() };
	collidable.Indices = { 3 * 2 * nrPlanes }; //3/triangle, 2/side
	collidable.TriangleNormals = { collidable.Indices.GetSize() / 3 };

	//front
	Float3 right{ roofSize.x, 0,0 };
	Float3 up{ 0, towerHeight,0 };
	Float3 rightUp{ right + up };
	Float3 origin{ position + Float3{rampWidth, 0, rampWidth} };
	int iVertex = 0;
	int iIndex = 0;
	int iTriangle = 0;
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//right
	origin += right;
	right = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//back
	origin += right;
	right = Float3{ -roofSize.x, 0, 0 };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//left
	origin += right;
	right = Float3{ 0,0,-roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//roof
	origin = position + Float3{ rampWidth, towerHeight, rampWidth };
	right = Float3{ roofSize.x, 0,0 };
	up = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-front
	origin = position + Float3{ rampWidth, 0, rampWidth };
	Float3 rampWidthAxis{ 0,0,-1 };
	Float3 rampDepthAxis{ 1,0,0 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-right
	origin += rampDepthAxis * roofSize.x + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ 1,0,0 };
	rampDepthAxis = Float3{ 0,0,1 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y, origin, color, vertices, iVertex, iIndex, iTriangle);
	Float3 rampSideOrigin{ position + Float3{2 * rampWidth + roofSize.x, 0, 0} };
	right = Float3{ 0,0,rampWidth * 2 + roofSize.y };
	up = Float3{ 0,rampHeight, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-back
	origin += rampDepthAxis * roofSize.y + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ 0,0,1 };
	rampDepthAxis = Float3{ -1,0,0 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 2 * rampWidth + roofSize.x, 0, 2 * rampWidth + roofSize.y };
	right = Float3{ -(rampWidth * 2 + roofSize.x) ,0,0 };
	up = Float3{ 0, rampHeight * 2, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-left
	origin += rampDepthAxis * roofSize.x + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ -1,0,0 };
	rampDepthAxis = Float3{ 0,0,-1 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y - rampWidth, origin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0, 0, 2 * rampWidth + roofSize.y };
	right = Float3{ 0,0,-(rampWidth + roofSize.y) };
	up = Float3{ 0,rampHeight * 3, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0, 0, rampWidth };
	right = Float3{ rampWidth, 0, 0 };
	up = Float3{ 0,rampHeight * 4, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);
	//MESH
	m_pMesh = Mesh::Create(vertices, collidable.Indices);
}

void Tower::AddPlane(
	MeshCollidable& collidable,
	const Float3& right, const Float3& up, const Float3& rightUp,
	const Float3& origin,
	const Float3& color,
	Array<V_PosNorCol>& vertices, int& verticesIdx,
	int& indicesIdx, int& triangleNormalIdx)
{
	const Float3 normal{ up.Cross(right).Normalized() };

	//left-bot
	vertices[verticesIdx] = { origin, normal, color };
	collidable.Points[verticesIdx] = origin;
	collidable.Indices[indicesIdx] = verticesIdx;

	//left-top
	vertices[++verticesIdx] = { origin + up, normal, color };
	collidable.Points[verticesIdx] = origin + up;
	collidable.Indices[++indicesIdx] = verticesIdx;

	//right-top
	vertices[++verticesIdx] = { origin + rightUp, normal, color };
	collidable.Points[verticesIdx] = origin + rightUp;
	collidable.Indices[++indicesIdx] = verticesIdx;
	collidable.Indices[++indicesIdx] = verticesIdx;

	//right-bot
	vertices[++verticesIdx] = { origin + right, normal, color };
	collidable.Points[verticesIdx] = origin + right;
	collidable.Indices[++indicesIdx] = verticesIdx;
	collidable.Indices[++indicesIdx] = verticesIdx - 3;

	collidable.TriangleNormals[triangleNormalIdx++] = normal;
	collidable.TriangleNormals[triangleNormalIdx++] = normal;

	verticesIdx++;
	indicesIdx++;
}

void Tower::AddRamp(
	MeshCollidable& collidable,
	const Float3& width, const Float3& depth,
	float rampWidth, float rampHeight, float rampDepth,
	const Float3& position, const Float3& color, Array<V_PosNorCol>& vertices, int& verticesIdx,
	int& indicesIdx, int& triangleNormalIdx)
{
	//ramp
	Float3 origin{ position };
	Float3 right{ width * rampWidth };
	Float3 up{ depth * rampDepth + Float3{0,rampHeight,0} };
	Float3 rightUp{ right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);

	//ramp side
	origin += right;
	right = { depth * (rampDepth + rampWidth) };
	up = Float3{ depth * rampDepth } + Float3{ 0, rampHeight, 0 };
	rightUp = right + Float3{ 0, rampHeight, 0 };
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);

	//ramp top
	origin = position + depth * rampDepth + Float3{ 0, rampHeight, 0 };
	right = width * rampWidth;
	up = depth * rampWidth;
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);
}