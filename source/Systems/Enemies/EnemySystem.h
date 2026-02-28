#pragma once

#include <Animations\Animation.h>
#include <Animations\Animator.h>
#include <Rendering\DrawData\VertexArray.h>

namespace TowerGame
{
class EnemySystem
{
	//---| Types |---
public:
	struct RenderInstance {
		Float4X4 World;
		unsigned BoneIdOffset;

		static const Rendering::InputLayout::Element ELEMENTS[];
		static constexpr unsigned NR_ELEMENTS{ 2 };
	};
	using Vertex = Rendering::V_PosNorUvSkin;
	//---| Data |---
public:
	struct Enemy {
		enum class State {
			Running, Falling, Dead, Completed
		};
		struct AttachedArrow {
			int ArrowIdx{};
			Transform Local;
		};
		Animations::Animator Animator;
		Transform World;
		Float3 FallAxis;
		Float2 RootPos;
		State State{ State::Running };
		List<AttachedArrow> Arrows{};

		Enemy();
		Enemy(const Animations::Animation& animation, const Float3& initPos);
	};
	struct Type {
		struct CollisionVertex
		{
			Float3 Point;
			Int4 BoneIndices;
			Float4 BoneWeights;
		};
		//Rendering
		Rendering::VertexArray<Vertex> Vertices;
		unsigned NrEnemiesInBonesBuffer{};
		//Animations
		Animations::Animation Animation;
		//Collision
		List<CollisionVertex> CollisionVertices{};
		//Logic
		List<Enemy> Enemies{};
		Float2 RootAnimationMovement;
		float Height{};
		float Radius{};
	};
	struct Enemies {
		List<Type> Types{};
		Float2 Target;
	};
	struct EnemiesRendering {
		Rendering::Texture Texture;
		Rendering::Shader Shader;
		Rendering::InputLayout InputLayout;
		Rendering::VertexArray<RenderInstance> Instances;

		EnemiesRendering();
	};
	//---| System |---
public:
	//Create
	EnemySystem();
	void Init(unsigned nrEnemies, const Float2& target);
	~EnemySystem() = default;

	//Update
	void Update();

	//Render
	template<Rendering::Shader::Function::Flag Function = Rendering::Shader::Function::Both, bool UnsetOthers = true>
	void Render(Rendering::ConstantBuffer<Float4X4>& bones);

	//Collision
	EnemySystem::Enemy* IsColliding(const Line& line);
	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Enemies m_Enemies;
	EnemiesRendering m_Rendering;

	struct TypeDesc {
		struct Attachment {
			std::wstring FbxPath;
			std::string ParentJoint;
		};
		float Scale{ 1.f };
		float Height{ 2.f };
		float Radius{ .5f };
		std::wstring ModelPath;
		std::wstring AnimationPath;
		List<Attachment> Attachments{};
	};
	unsigned CreateType(TypeDesc desc);
	void CreateTypes();

	void Render_Internal(Rendering::ConstantBuffer<Float4X4>& bones);
};
template<Rendering::Shader::Function::Flag Function, bool UnsetOthers>
inline void EnemySystem::Render(Rendering::ConstantBuffer<Float4X4>& bones)
{
	m_Rendering.Shader.Activate<Function, UnsetOthers>();
	Render_Internal(bones);
}
}