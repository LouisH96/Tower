#pragma once
#include "../Weapons/Bow.h"
#include <Game/Camera/FpsCameraController.h>
#include <Framework/CoreServices.h>

class Terrain;

class Character
{
public:
	explicit Character(const Framework::CoreServices& services, const Float3& position);

	void Register(const Terrain& terrain);
	void Register(const Tower& tower);
	void Register(TowerAppRenderer& renderer);

	void Update(const TowerAppServices& services);
	void Render(const Camera& camera);

	const FpsCameraController& GetCameraController() const { return m_CameraController; }
	Bow& GetBow() { return m_Bow; }

private:
	FpsCameraController m_CameraController;
	Bow m_Bow;
	const Terrain* m_pTerrain{};
	const Tower* m_pTower{};
};

