#include "pch.h"
#include "StaticModelSystem.h"

#include <Rendering\ModelLoaders\ModelLoader.h>
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
					vertex.Normal = WorldMatrix::RotatePoint(world, vertex.Normal);
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
}
