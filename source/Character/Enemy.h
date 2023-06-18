#pragma once

class Enemy
{
public:
	Enemy();
	Enemy(const Float3& initPos);

	void Update(const Float2& target, float maxMovement);

	Transform& GetTransform() { return m_World; }
	const Transform& GetTransform() const { return m_World; }

	void HitByArrow(const Transform& worldArrowTransform, int arrowIdx);

	static constexpr float HEIGHT = 2.f;

private:
	struct AttachedArrow
	{
		int arrowIdx;
		Transform Local;
	};

	Transform m_World;
	Float3 m_FallOverAxis;

	std::vector<AttachedArrow> m_Arrows{};

	void UpdateMove(const Float2& target, float maxMovement);
	void UpdateFall();
};
