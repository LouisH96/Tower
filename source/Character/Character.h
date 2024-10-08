#pragma once
#include <Camera/FpsCameraController.h>

namespace TowerGame
{
class Character
{
public:
	explicit Character(const Float3& position);

	void Update();

	const FpsCameraController& GetCameraController() const { return m_CameraController; }
	const Float3& GetPosition() const { return m_CameraController.GetPosition(); }

private:
	FpsCameraController m_CameraController;

	char m_MoveLeftKey;
	char m_MoveRightKey;
	char m_MoveUpKey;
	char m_MoveDownKey;
};
}
