#pragma once

#include <Animations\Animation.h>
#include <Animations\Animator.h>
#include <DataStructures\InvalidateList.h>
#include <Physics\CollisionDetection.h>
#include <Rendering\DrawData\VertexArray.h>

namespace TowerGame
{
class EnemySystem
{
	//---| Types |---
public:
	struct RenderInstance {
		Float4X4 World;
		unsigned BoneIdOffset{};

		static const Rendering::InputLayout::Element ELEMENTS[];
		static constexpr unsigned NR_ELEMENTS{ 2 };
	};
	using Vertex = Rendering::V_PosNorUvSkin;
	//---| Data |---
public:
	struct Enemy {
		enum class State {
			Running, Falling, Dead, Completed, Invalid
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

		bool IsValid() const { return State != State::Invalid; }
		void Invalidate() { State = State::Invalid; }
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
		InvalidateList<Enemy> Enemies{};
		Float2 RootAnimationMovement;
		float Height{};
		float Radius{};
	};
	struct Spawning {
		static constexpr float IntervalChangeDuration{ 80.f }; //Time(in sec) to go from initial SpawnInterval to MinSpawnInterval
		float SpawnInterval{ 3.5f }; //Initial SpawnInterval(in sec), spawn an enemy each x seconds
		float MinSpawnInterval{ .35f }; //Final SpawnInterval

		float IntervalChange{}; //Rate of change of SpawnInterval
		float IntervalChange2{}; //Second order rate of change, to get exponential change
		float NextSpawn{ 0.f }; //Seconds until the next spawn should happen

		float SpawnRadius{};
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
	void Init(unsigned nrEnemies, const Float2& target, float spawnRadius);
	~EnemySystem() = default;

	//Update
	void Update();

	//Render
	template<Rendering::Shader::Function::Flag Function = Rendering::Shader::Function::Both, bool UnsetOthers = true>
	void Render(Rendering::ConstantBuffer<Float4X4>& bones);

	//Collision
	EnemySystem::Enemy* IsColliding(const Line& line,
		Physics::CollisionDetection::Collision& hit);
	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

private:
	Enemies m_Enemies;
	EnemiesRendering m_Rendering;
	Spawning m_Spawning;

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