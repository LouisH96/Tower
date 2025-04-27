#include "pch.h"
#include "Tower.h"

#include <Rendering\State\Mesh.h>
#include <Rendering\Structs\VertexTypes.h>
#include <Systems\Collisions\CollisionSystem.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;
using namespace Rendering;

void Tower::Init(Desc& desc)
{
	m_Position = desc.Position;
	m_RoofSize = desc.RoofSize;
	m_Height = desc.Height;
}

void Tower::GenerateMeshIn(
	StaticMeshSystem::MeshData& staticMeshData,
	MeshCollidable& collidable)
{
	return;

	StaticMeshSystem::MeshData towerMeshData{};

	//todo: clean
	Generate(collidable, m_Position, m_RoofSize, m_Height);
	towerMeshData.Vertices.Add(m_Vertices);
	towerMeshData.Indices.Add(collidable.Indices);

	staticMeshData.Add(towerMeshData);
}

void Tower::AddModelsIn(StaticModelSystem::InitData& staticModelData)
{
	using TextureGroup = StaticModelSystem::InitData::TextureGroup;
	using ModelGroup = StaticModelSystem::InitData::ModelGroup;

	m_Position.z -= 1;
	m_Position.y += 3;

	struct Orientation
	{
		Float3 Pos;
		Float3 Eulers;
	};
	struct Model {
		std::wstring Path;
		Float3 Scale{ 1, 1, 1 };
		List<Orientation> Instances;
	};
	struct Texture {
		std::wstring Path;
		List<Model> Models;
	};

	//Input
	List<Texture> data{};

	Texture& adventure{ data.AddEmpty() };
	adventure.Path = L"PolyAdventureTexture_01.png";

	Model& village3{ adventure.Models.AddEmpty() };
	village3.Path = L"SM_Bld_Village_03.fbx";
	village3.Scale = { 1,2,1 };
	village3.Instances.Add({
		{ -3.96f, -1.02f, -2.09f },
		{0.f, -180.f, 0.f } });

	Model& village5{ adventure.Models.AddEmpty() };
	village5.Path = L"SM_Bld_Village_05.fbx";
	village5.Instances.Add({
		{3.23f, 3.06f, -0.2500038f},
		{0,-90,0} });

	Model& village6{ adventure.Models.AddEmpty() };
	village6.Path = L"SM_Bld_Village_06.fbx";
	village6.Scale = { 1,1,1.05373f };
	village6.Instances.Add({
		{1.649994f, 0, -1.099998f},
		{ 0,-180,0 } });

	Model& drapes{ adventure.Models.AddEmpty() };
	drapes.Path = L"SM_Bld_Village_WindowDrapes_01.fbx";
	drapes.Scale = Float3{ 0.97691f, 1.11162f, 1.34364f };
	drapes.Instances.Add({
		{5.18f, 2.72f, -1.900002f},
		{ 0, 91, 0 } });
	drapes.Instances.Add({
		{1.167f, 6.054f, -0.2f},
		{4.886f,-88.835f, 0.f } });

	Model& stall{ adventure.Models.AddEmpty() };
	stall.Path = L"SM_Bld_Stall_Cover_02.fbx";
	stall.Instances.Add({
		{3.729973f, 0, -5.740005f},
		{0,-180,0} });

	Model& cart{ adventure.Models.AddEmpty() };
	cart.Path = L"SM_Prop_Cart_03.fbx";
	cart.Instances.Add({
		{ 0.35f, -0.1f, -5.89f },
		{5.f, 105.f, 5.f} });

	Model& crate{ adventure.Models.AddEmpty() };
	crate.Path = L"SM_Prop_Crate_01.fbx";
	crate.Scale = Float3{ 1.27147f };
	crate.Instances.Add({
		{4.109985f,3.23f,-3.210007f},
		{0,-167.389f,0} });
	crate.Instances.Add({
		{-1.407001f,3.21f,-1.306f},
		{0,-162.879f,0} });

	Texture& knights{ data.AddEmpty() };
	knights.Path = L"Texture_01.png";

	Model& stairs{ knights.Models.AddEmpty() };
	stairs.Path = L"SM_Bld_Rockwall_Stairs_01.fbx";
	stairs.Scale = Float3{ 1.1679f };
	stairs.Instances.Add({
		{-1.439999f, 2.26f, -2.930031f},
		{0,90,0} });
	stairs.Instances.Add({
		{0.1199989f, 0, 3.029999f},
		{0,90,0} });

	Model& trapDoor{ knights.Models.AddEmpty() };
	trapDoor.Path = L"SM_Prop_Trapdoor_01.fbx";
	trapDoor.Instances.Add({
		{-3.159f, 5.532f, -1.124f},
		{} });

	Model& dirtFloor{ knights.Models.AddEmpty() };
	dirtFloor.Path = L"SM_Env_Path_Dirt_02.fbx";
	dirtFloor.Scale = Float3{ 1.1591f };
	dirtFloor.Instances.Add({
		{-2.28f, 5.52f, -0.38f},
		{} });

	Texture& western{ data.AddEmpty() };
	western.Path = L"PolygonWestern_Texture_01_A.png";

	Model& ladder{ western.Models.AddEmpty() };
	ladder.Path = L"SM_Prop_Ladder_01.fbx";
	ladder.Scale = { 1.2917f, 1.390386f, 1.29171f };
	ladder.Instances.Add({
		{-2.606f, 5.569f, -1.077f},
		{78.157f, 87.524f, 1.905f} });

	//Add to InitData
	for (unsigned iTexture{ 0 }; iTexture < data.GetSize(); ++iTexture)
	{
		Texture& textureData{ data[iTexture] };

		TextureGroup& texture{ staticModelData.TextureGroups.AddEmpty() };
		texture.Path = Resources::Local(textureData.Path);

		for (unsigned iModel{ 0 }; iModel < textureData.Models.GetSize(); ++iModel)
		{
			Model& modelData{ textureData.Models[iModel] };

			ModelGroup& model{ texture.ModelGroups.AddEmpty() };
			model.Path = Resources::Local(modelData.Path);
			model.Scale = modelData.Scale;

			for (unsigned iInstance{ 0 }; iInstance < modelData.Instances.GetSize(); ++iInstance)
			{
				Orientation& instanceData{ modelData.Instances[iInstance] };

				model.Matrices.Add(WorldMatrix::FromPosAndQuat(
					instanceData.Pos + m_Position,
					Quaternion::FromEulerDegrees(instanceData.Eulers)
				));
			}
		}
	}
}

void Tower::Generate(MeshCollidable& collidable,
	const Float3& position, const Float2& roofSize,
	float towerHeight)
{
	constexpr float rampWidth{ 2 };
	const float rampHeight{ towerHeight / 4 };
	const Float3 color{ Float3::Color(248,240,164) };

	constexpr int nrPlanes{ 5 + 4 * 4 };
	m_Vertices.EnsureIncrease(4 * nrPlanes);
	collidable.Points = { 4 * nrPlanes };
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
	AddPlane(collidable, right, up, rightUp, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//right
	origin += right;
	right = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//back
	origin += right;
	right = Float3{ -roofSize.x, 0, 0 };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//left
	origin += right;
	right = Float3{ 0,0,-roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//roof
	origin = position + Float3{ rampWidth, towerHeight, rampWidth };
	right = Float3{ roofSize.x, 0,0 };
	up = Float3{ 0,0,roofSize.y };
	rightUp = { right + up };
	AddPlane(collidable, right, up, rightUp, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//ramp-front
	origin = position + Float3{ rampWidth, 0, rampWidth };
	Float3 rampWidthAxis{ 0,0,-1 };
	Float3 rampDepthAxis{ 1,0,0 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//ramp-right
	origin += rampDepthAxis * roofSize.x + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ 1,0,0 };
	rampDepthAxis = Float3{ 0,0,1 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y, origin, color, m_Vertices, iVertex, iIndex, iTriangle);
	Float3 rampSideOrigin{ position + Float3{2 * rampWidth + roofSize.x, 0, 0} };
	right = Float3{ 0,0,rampWidth * 2 + roofSize.y };
	up = Float3{ 0,rampHeight, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//ramp-back
	origin += rampDepthAxis * roofSize.y + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ 0,0,1 };
	rampDepthAxis = Float3{ -1,0,0 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.x, origin, color, m_Vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 2 * rampWidth + roofSize.x, 0, 2 * rampWidth + roofSize.y };
	right = Float3{ -(rampWidth * 2 + roofSize.x) ,0,0 };
	up = Float3{ 0, rampHeight * 2, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, m_Vertices, iVertex, iIndex, iTriangle);

	//ramp-left
	origin += rampDepthAxis * roofSize.x + Float3{ 0, rampHeight, 0 };
	rampWidthAxis = Float3{ -1,0,0 };
	rampDepthAxis = Float3{ 0,0,-1 };
	AddRamp(collidable, rampWidthAxis, rampDepthAxis, rampWidth, rampHeight, roofSize.y - rampWidth, origin, color, m_Vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0, 0, 2 * rampWidth + roofSize.y };
	right = Float3{ 0,0,-(rampWidth + roofSize.y) };
	up = Float3{ 0,rampHeight * 3, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, m_Vertices, iVertex, iIndex, iTriangle);
	rampSideOrigin = position + Float3{ 0, 0, rampWidth };
	right = Float3{ rampWidth, 0, 0 };
	up = Float3{ 0,rampHeight * 4, 0 };
	rightUp = right + up;
	AddPlane(collidable, right, up, rightUp, rampSideOrigin, color, m_Vertices, iVertex, iIndex, iTriangle);
}

void Tower::AddPlane(
	MeshCollidable& collidable,
	const Float3& right, const Float3& up, const Float3& rightUp,
	const Float3& origin,
	const Float3& color,
	List<V_PosNorCol>& vertices, int& verticesIdx,
	int& indicesIdx, int& triangleNormalIdx)
{
	const Float3 normal{ up.Cross(right).Normalized() };

	//left-bot
	vertices.Add({ origin, normal, color });
	collidable.Points[verticesIdx] = origin;
	collidable.Indices[indicesIdx] = verticesIdx;

	//left-top
	vertices.Add({ origin + up, normal, color });
	collidable.Points[++verticesIdx] = origin + up;
	collidable.Indices[++indicesIdx] = verticesIdx;

	//right-top
	vertices.Add({ origin + rightUp, normal, color });
	collidable.Points[++verticesIdx] = origin + rightUp;
	collidable.Indices[++indicesIdx] = verticesIdx;
	collidable.Indices[++indicesIdx] = verticesIdx;

	//right-bot
	vertices.Add({ origin + right, normal, color });
	collidable.Points[++verticesIdx] = origin + right;
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
	const Float3& position, const Float3& color, List<V_PosNorCol>& vertices, int& verticesIdx,
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
