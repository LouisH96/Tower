#include "pch.h"
#include "Tower.h"

#include "../TowerAppRenderer.h"
#include "Framework/CoreServices.h"
#include "Rendering/State/Mesh.h"
#include "Rendering/Structs/VertexTypes.h"

using namespace Math;
using namespace Rendering;

Tower::Tower(
	const Framework::CoreServices& services, const TowerAppRenderer& renderer,
	const Math::Float3& position, const Math::Float2& roofSize, float height)
{
	Generate(services.Gpu, position, roofSize, height);
	renderer.GetTerrainRenderer().AddEntry(*m_pMesh);
}

Tower::~Tower()
{
	delete m_pMesh;
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

void Tower::Generate(const Gpu& gpu,
	const Math::Float3& position, const Math::Float2& roofSize,
	float towerHeight)
{
	constexpr float rampWidth{ 2 };
	const float rampHeight{ towerHeight / 4 };
	const Float3 color{ Float3::RgbNormalize(248,240,164) };

	constexpr int nrPlanes{ 5 + 4 * 4 };
	Array<V_PosNorCol> vertices{ 4 * nrPlanes }; //4 per side
	m_Points = { vertices.GetSize() };
	m_Indices = { 3 * 2 * nrPlanes }; //3/triangle, 2/side
	m_TriangleNormals = { m_Indices.GetSize() / 3 };

	//front
	Float3 right{ roofSize.x, 0,0 };
	Float3 up{ 0, towerHeight,0 };
	Float3 rightUp{ right + up };
	Float3 origin{ position + Float3{rampWidth, 0, rampWidth} };
	int iVertex = 0;
	int iIndex = 0;
	int iTriangle = 0;
	AddPlane(right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//right
	origin += right;
	right = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//back
	origin += right;
	right = Float3{ -roofSize.x, 0, 0 };
	rightUp = { right + up };
	AddPlane(right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//left
	origin += right;
	right = Float3{ 0,0,-roofSize.y };
	rightUp = { right + up };
	AddPlane(right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//roof
	origin = position + Float3{ rampWidth, towerHeight, rampWidth };
	right = Float3{ roofSize.x, 0,0 };
	up = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(right, up, rightUp, origin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-front
	origin = position + Float3{ rampWidth, 0, rampWidth };
	Float3 rampWidthAxis{ 0,0,-1 };
	Float3 rampDepthAxis{ 1,0,0 };
	AddRamp(rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-right
	origin += rampDepthAxis * roofSize.x + Float3{ 0,rampHeight, 0 };
	rampWidthAxis = Float3{ 1,0,0 };
	rampDepthAxis = Float3{ 0,0,1 };
	AddRamp(rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y, origin, color, vertices, iVertex, iIndex, iTriangle);
	Float3 rampSideOrigin{ position + Float3{2 * rampWidth + roofSize.x, 0, 0} };
	right = Float3{ 0,0,rampWidth * 2 + roofSize.y };
	up = Float3{ 0,rampHeight, 0 };
	rightUp = right + up;
	AddPlane(right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-back
	origin += rampDepthAxis * roofSize.y + Float3{ 0,rampHeight, 0 };
	rampWidthAxis = Float3{ 0,0,1 };
	rampDepthAxis = Float3{ -1,0,0 };
	AddRamp(rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 2 * rampWidth + roofSize.x, 0, 2 * rampWidth + roofSize.y };
	right = Float3{ -(rampWidth * 2 + roofSize.x) ,0,0 };
	up = Float3{ 0, rampHeight * 2, 0 };
	rightUp = right + up;
	AddPlane(right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);

	//ramp-left
	origin += rampDepthAxis * roofSize.x + Float3{ 0,rampHeight, 0 };
	rampWidthAxis = Float3{ -1,0,0 };
	rampDepthAxis = Float3{ 0,0,-1 };
	AddRamp(rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y-rampWidth, origin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0,0,2 * rampWidth + roofSize.y };
	right = Float3{ 0,0,-(rampWidth + roofSize.y) };
	up = Float3{ 0,rampHeight * 3, 0 };
	rightUp = right + up;
	AddPlane(right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0,0,rampWidth };
	right = Float3{ rampWidth, 0, 0 };
	up = Float3{ 0,rampHeight * 4, 0 };
	rightUp = right + up;
	AddPlane(right, up, rightUp, rampSideOrigin, color, vertices, iVertex, iIndex, iTriangle);
	//MESH
	m_pMesh = Mesh::Create(gpu, vertices, m_Indices);
}

void Tower::AddPlane(
	const Float3& right, const Float3& up, const Float3& rightUp,
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
	vertices[++verticesIdx] = { origin + rightUp, normal, color };
	m_Points[verticesIdx] = origin + rightUp;
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

void Tower::AddRamp(const Math::Float3& width, const Math::Float3& depth,
	float rampWidth, float rampHeight, float rampDepth,
	const Math::Float3& position, const Math::Float3& color, Array<Rendering::V_PosNorCol>& vertices, int& verticesIdx,
	int& indicesIdx, int& triangleNormalIdx)
{
	//ramp
	Float3 origin{ position };
	Float3 right{ width * rampWidth };
	Float3 up{ depth * rampDepth + Float3{0,rampHeight,0} };
	Float3 rightUp{ right + up };
	AddPlane(right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);

	//ramp side
	origin += right;
	right = { depth * (rampDepth + rampWidth) };
	up = Float3{ depth * rampDepth } + Float3{ 0, rampHeight, 0 };
	rightUp = right + Float3{ 0,rampHeight, 0 };
	AddPlane(right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);

	//ramp top
	origin = position + depth * rampDepth + Float3{ 0,rampHeight, 0 };
	right = width * rampWidth;
	up = depth * rampWidth;
	rightUp = right + up;
	AddPlane(right, up, rightUp, origin, color, vertices, verticesIdx, indicesIdx, triangleNormalIdx);
}