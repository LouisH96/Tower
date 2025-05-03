#pragma once

#include <Datastructures\List.h>
#include <Geometry\ModelTopology.h>
#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\State\Texture.h>

namespace TowerGame
{
class StaticModelSystem
{
public:
	struct InitData
	{
		struct ModelGroup
		{
			std::wstring Path{};
			Float3 Scale{ 1,1,1 };
			List<Float4X4> Matrices{};
		};
		struct TextureGroup
		{
			std::wstring Path{};
			List<ModelGroup> ModelGroups{};
		};
		List<TextureGroup> TextureGroups{};
	};

public:
	StaticModelSystem();

	void Init(const InitData& initData);
	void Render();

	static constexpr float SYNTY_SCALE{ .01f };

private:
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleList };
	using Vertex = Rendering::V_PosNorUv;
	struct TextureGroup
	{
		Rendering::Texture Texture;
		MeshBuffers<Vertex, TOPOLOGY> Buffers;
	};

	List<TextureGroup> m_TextureGroups{};

	void CreateCollidables(const MeshData<Vertex, TOPOLOGY>& mesh, const List<Float4X4>& instances);
};
}