#pragma once
#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\Structs\VertexTypes.h>

namespace MyEngine
{
	namespace Rendering
	{
		class Texture;
	}
}

namespace TowerGame
{
class Bow
{
public:
	using Vertex = Rendering::V_PosNorUv;
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleList };

	Bow();
	~Bow() = default;

	void Update(const Transform& cameraTransform);
	void Render();
	void LinkRenderers();

	const Transform& GetWorldTransform() const { return m_WorldTransform; }

private:
	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::Texture m_Texture{};
	MeshBuffers<Vertex, TOPOLOGY> m_Buffers{};

	int m_ArrowIdx{ -1 };
};
}
