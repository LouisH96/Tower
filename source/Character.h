#pragma once
#include "Bow.h"
#include <Game/Camera/FpsCameraController.h>
#include <Framework/CoreServices.h>

class Character
{
public:
	explicit Character(const Framework::CoreServices& services, const Math::Float3& position);

	void Register(const Terrain& terrain);
	void Register(const Tower& tower);
	void Register(const TowerAppRenderer& renderer);

	void Update();

	const Game::FpsCameraController& GetCameraController() const { return m_CameraController; }

private:
	Game::FpsCameraController m_CameraController;
	Bow m_Bow;
	const Terrain* m_pTerrain{};
	const Tower* m_pTower{};
};

