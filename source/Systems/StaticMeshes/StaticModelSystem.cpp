#include "pch.h"
#include "StaticModelSystem.h"

#include "..\Collisions\CollisionSystem.h"
#include <Geometry\PointUtils.h>
#include <Geometry\Shapes\Triangle.h>
#include <Rendering\ModelLoaders\ModelLoader.h>
#include <TowerApp.h>
#include <Transform\WorldMatrix.h>

using namespace TowerGame;
using namespace Rendering;

StaticModelSystem::StaticModelSystem()
	: m_InputLayout{ InputLayout::FromType<Vertex>() }
	, m_Shader{ Resources::GlobalShader(L"LambertCam_Tex.hlsl") }
{
}

void StaticModelSystem::Init(const InitData& initData)
{
	for (unsigned iTexture{ 0 }; iTexture < initData.TextureGroups.GetSize(); ++iTexture)
	{
		const InitData::TextureGroup& initTextureGroup{ initData.TextureGroups[iTexture] };

		MeshData<Vertex, TOPOLOGY> textureMesh{};

		for (unsigned iModel{ 0 }; iModel < initTextureGroup.ModelGroups.GetSize(); ++iModel)
		{
			const InitData::ModelGroup& initModelGroup{ initTextureGroup.ModelGroups[iModel] };

			MeshData<Vertex, TOPOLOGY> modelMesh{};
			ModelLoader::Load(initModelGroup.Path, SYNTY_SCALE, modelMesh);

			//Apply scale
			if (initModelGroup.Scale != Float3{ 1, 1, 1 })
			{
				const Float3 invScale{ initModelGroup.Scale.Inversed() };

				for (unsigned iPoint{ 0 }; iPoint < modelMesh.Vertices.GetSize(); ++iPoint)
				{
					Vertex& vertex{ modelMesh.Vertices[iPoint] };

					vertex.Pos *= initModelGroup.Scale;
					vertex.Normal *= invScale;
					vertex.Normal.Normalize();
				}
			}

			CreateCollidables(modelMesh, initModelGroup.Matrices);

			textureMesh.Vertices.EnsureIncrease(
				modelMesh.Vertices.GetSize() * initModelGroup.Matrices.GetSize());

			for (unsigned iInstance{ 0 }; iInstance < initModelGroup.Matrices.GetSize(); ++iInstance)
			{
				const Float4X4& world{ initModelGroup.Matrices[iInstance] };

				for (unsigned iVertex{ 0 }; iVertex < modelMesh.Vertices.GetSize(); ++iVertex)
				{
					textureMesh.Vertices.Add(modelMesh.Vertices[iVertex]);
					Vertex& vertex{ textureMesh.Vertices.Last() };

					WorldMatrix::TransformPoint(world, vertex.Pos);
					WorldMatrix::RotatePoint(world, vertex.Normal);
				}
			}
		}

		TextureGroup texture{};
		texture.Texture = Texture{ initTextureGroup.Path };
		texture.Buffers.CreateBuffers(textureMesh);
		m_TextureGroups.Add(std::move(texture));
	}
}

void StaticModelSystem::Render()
{
	PrimitiveTopology::Activate(TOPOLOGY);
	m_InputLayout.Activate();
	m_Shader.Activate();

	for (unsigned iTexture{ 0 }; iTexture < m_TextureGroups.GetSize(); ++iTexture)
	{
		const TextureGroup& texture{ m_TextureGroups[iTexture] };
		texture.Texture.Activate();
		texture.Buffers.ActivateAndDraw();
	}

	//temp
	//SYSTEMS.Collisions.Models.RenderDebugBoundingBoxes();
}

void StaticModelSystem::CreateCollidables(
	const MeshData<Vertex, TOPOLOGY>& mesh, const List<Float4X4>& instances)
{
	ModelCollidable collidable{};
	collidable.Points = { mesh.Vertices.GetSize() };
	collidable.TriangleNormals = { mesh.Vertices.GetSize() / 3 };
	collidable.Instances.EnsureIncrease(instances.GetSize());

	for (
		unsigned iPoint{ 2 }, iTriangle{ 0 };
		iPoint < mesh.Vertices.GetSize();
		iPoint += 3, ++iTriangle)
	{
		const Float3& p0{ mesh.Vertices[iPoint - 2].Pos };
		const Float3& p1{ mesh.Vertices[iPoint - 1].Pos };
		const Float3& p2{ mesh.Vertices[iPoint - 0].Pos };

		//Point & Normal
		collidable.Points[iPoint - 2] = p0;
		collidable.Points[iPoint - 1] = p1;
		collidable.Points[iPoint - 0] = p2;
		collidable.TriangleNormals[iTriangle] =
			Triangle::FindNormal(p0, p1, p2);
	}

	//Bounds
	Float3 minBounds{ Float::MAX };
	Float3 maxBounds{ -Float::MAX };
	for (unsigned iPoint{ 0 }; iPoint < mesh.Vertices.GetSize(); ++iPoint)
		Geometry::PointUtils::UpdateBounds(
			mesh.Vertices[iPoint].Pos, minBounds, maxBounds);
	collidable.Size = maxBounds - minBounds;

	for (unsigned iPoint{ 0 }; iPoint < collidable.Points.GetSize(); ++iPoint)
		collidable.Points[iPoint] -= minBounds;

	//Instances
	for (unsigned iInstance{ 0 }; iInstance < instances.GetSize(); ++iInstance)
	{
		ModelCollidable::Instance instance{};
		instance.World = instances[iInstance];
		WorldMatrix::TranslateRelative(instance.World, minBounds);
		instance.WorldInverse = WorldMatrix::GetInversed(instance.World);
		collidable.Instances.Add(instance);
	}

	SYSTEMS.Collisions.Models.Models.Add(collidable);
}
