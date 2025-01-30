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

	const Transform& GetWorldTransform() const { return m_WorldTransform; }
	const Array<Float4X4>& GetBones() const { return m_Animator.GetBones(); }

	bool IsLoaded() const { return m_ArrowIdx != -1; }

private:
	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::Texture m_Texture{};
	MeshBuffers<Vertex, TOPOLOGY> m_Buffers{};

	Animations::Animation m_Animation;
	Animations::Animator m_Animator;

	int m_ArrowIdx{ -1 };
	float m_ArrowToBow{};

	static constexpr unsigned BOW_STRING_JOINT_IDX{ 1 };
	static constexpr float CHARGE_ARROW_TIMESCALE{ 1.f / .5f };
	static float RELEASE_ARROW_TIMESCALE;
};
}
