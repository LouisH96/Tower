#pragma once
#include <App\Win32\Keyboard_.h>
#include <Camera\FpsCameraController.h>

namespace TowerGame
{
class Character
{
public:
	constexpr static float HEIGHT{ 1.8f };

	Character();
	void Init(const Float3& position);

	void Update();

	const FpsCameraController& GetCameraController() const { return m_CameraController; }
	const Float3& GetPosition() const { return m_CameraController.GetPosition(); }
	Float3 GetFeetPosition() const;

private:
	static constexpr float SPEED{ 5.f };
	static constexpr float JUMP_HEIGHT{ 1.1f };
	static constexpr float SPRINT_SPEED{ SPEED * 1.5f };
	static const float JUMP_VELOCITY;
	FpsCameraController m_CameraController;

	Keyboard::Key m_MoveLeftKey;
	Keyboard::Key m_MoveRightKey;
	Keyboard::Key m_MoveUpKey;
	Keyboard::Key m_MoveDownKey;
	Keyboard::Key m_JumpKey;
	Keyboard::Key m_SprintKey;
	Keyboard::Key m_DebugUpKey;
	Keyboard::Key m_DebugDownKey;
	Float3 m_Velocity{};
};
}
