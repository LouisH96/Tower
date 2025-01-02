#pragma once
#include <Rendering\Mesh\MeshBuffers.h>
#include <Rendering\Structs\VertexTypes.h>
#include <Animations\Animation.h>
#include <Animations\Animator.h>

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
	using Vertex = Rendering::V_PosNorUvSkin;
	static constexpr ModelTopology TOPOLOGY{ ModelTopology::TriangleList };

	Bow();
	~Bow() = default;

	void Update(const Transform& cameraTransform);
	void Render();
	void LinkRenderers();

	const Transform& GetWorldTransform() const { return m_WorldTransform; }
	const Rendering::CB_BonesBuffer& GetBonesBuffer() const { return m_Animator.GetBonesBuffer(); }

private:
	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::Texture m_Texture{};
	MeshBuffers<Vertex, TOPOLOGY> m_Buffers{};

	Animations::Animation m_Animation;
	Animations::Animator m_Animator;

	int m_ArrowIdx{ -1 };
};
}
