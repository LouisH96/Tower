#pragma once
#include <Camera/FpsCameraController.h>

class Character
{
public:
	explicit Character(const Float3& position);

	void Update();

	const FpsCameraController& GetCameraController() const { return m_CameraController; }

private:
	FpsCameraController m_CameraController;

	char m_MoveLeftKey;
	char m_MoveRightKey;
	char m_MoveUpKey;
	char m_MoveDownKey;
};
