#pragma once
#include "Enemy.h"

#include <Animations\Animation.h>
#include <Animations\Animator.h>
#include <Rendering\DrawData\InstanceArray.h>
#include <Rendering\Structs\InstanceTypes.h>
#include <Rendering\Structs\VertexTypes.h>
#include <Rendering\State\Shader.h>

namespace TowerGame
{

class EnemySystem
{
public:
	struct Instance {
		Float4X4 World;
		unsigned BoneIdOffset;

		static const Rendering::InputLayout::Element ELEMENTS[];
		static constexpr unsigned NR_ELEMENTS{ 2 };
	};
	using Vertex = Rendering::V_PosNorUvSkin;

	EnemySystem(int nrEnemies, const Float2& target);
	~EnemySystem() = default;

	void LinkRenderers();
	void Update();
	void OnCollision(const Transform& arrowTransform, int arrowIdx, Enemy& enemy);

	template<
		Rendering::Shader::Function::Flag Function = Rendering::Shader::Function::Both,
		bool UnsetOthers = true >
	void Render(Rendering::ConstantBuffer<Float4X4>& bones);

private:
	Array<Enemy> m_Enemies;
	const Float2 m_Target;
	unsigned m_BatchLimit;

	//Rendering
	Rendering::Texture m_Texture;
	Rendering::InstanceArray m_InstanceArray;
	Rendering::Shader m_Shader;

	Rendering::InputLayout m_InputLayout;

	//Animations
	Animations::Animation m_Animation;

	void Render_Internal(
		Rendering::ConstantBuffer<Float4X4>& bones);
};
template<Rendering::Shader::Function::Flag Function, bool UnsetOthers>
inline void EnemySystem::Render(Rendering::ConstantBuffer<Float4X4>& bones)
{
	m_Shader.Activate<Function, UnsetOthers>();
	Render_Internal(bones);
}
}