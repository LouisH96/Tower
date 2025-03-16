#pragma once
#include <Camera/FpsCameraController.h>

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
	static constexpr float SPEED{ 25.f };
	FpsCameraController m_CameraController;

	char m_MoveLeftKey;
	char m_MoveRightKey;
	char m_MoveUpKey;
	char m_MoveDownKey;
};
}
