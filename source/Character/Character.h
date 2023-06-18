#pragma once
#include <Game/Camera/FpsCameraController.h>

class Character
{
public:
	explicit Character(const Float3& position);

	void Update();

	const FpsCameraController& GetCameraController() const { return m_CameraController; }

private:
	FpsCameraController m_CameraController;
};
