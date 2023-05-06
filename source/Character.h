#pragma once
#include "Bow.h"
#include <Game/Camera/FpsCameraController.h>
#include <Framework/CoreServices.h>

class Character
{
public:
	explicit Character(const Framework::CoreServices& services);

	void Register(const Terrain& terrain);
	void Register(const TowerAppRenderer& renderer);

	void Update();

	const Game::FpsCameraController& GetCameraController() const { return m_CameraController; }

private:
	Game::FpsCameraController m_CameraController;
	Bow m_Bow;
};

