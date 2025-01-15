#pragma once

#include <Animations\Animation.h>
#include <Animations\Animator.h>

namespace TowerGame
{
class Enemy
{
public:
	enum class State {
		Running, Falling, Dead, Completed
	};

	Enemy();
	Enemy(const Animations::Animation& animation, const Float3& initPos);

	void Update(
		const Float2& target,
		const Animations::Animation& animation);

	Transform& GetTransform() { return m_World; }
	const Transform& GetTransform() const { return m_World; }
	const Animations::Animator& GetAnimator() const { return m_Animator; }
	const Float2& GetRootPos() const { return m_RootPos; }

	void HitByArrow(const Transform& worldArrowTransform, int arrowIdx);

	static constexpr float HEIGHT = 2.f;
	static Float2 FullAnimationMovement;

private:
	struct AttachedArrow
	{
		int arrowIdx;
		Transform Local;
	};

	Animations::Animator m_Animator;
	Transform m_World;
	Float3 m_FallOverAxis;
	Float2 m_RootPos;
	State m_State;
	std::vector<AttachedArrow> m_Arrows{};

	void UpdateMove(const Float2& target, const Float2& maxMovement);
	void UpdateFall();
};
}
