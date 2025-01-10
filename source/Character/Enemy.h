#pragma once

#include <Animations\Animation.h>
#include <Animations\Animator.h>

namespace TowerGame
{
class Enemy
{
public:
	Enemy();
	Enemy(const Animations::Animation& animation, const Float3& initPos);

	void Update(
		const Float2& target, float maxMovement,
		const Animations::Animation& animation);

	Transform& GetTransform() { return m_World; }
	const Transform& GetTransform() const { return m_World; }
	const Animations::Animator& GetAnimator() const { return m_Animator; }

	void HitByArrow(const Transform& worldArrowTransform, int arrowIdx);

	static constexpr float HEIGHT = 2.f;

private:
	struct AttachedArrow
	{
		int arrowIdx;
		Transform Local;
	};

	Animations::Animator m_Animator;
	Transform m_World;
	Float3 m_FallOverAxis;

	std::vector<AttachedArrow> m_Arrows{};

	void UpdateMove(const Float2& target, float maxMovement);
	void UpdateFall();
};
}
