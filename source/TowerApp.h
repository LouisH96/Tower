#pragma once
#include "TowerAppRenderer.h"
#include "Services/TowerAppServices.h"

class EnemySystem;
class Character;

class TowerApp
{
public:
	explicit TowerApp(const Framework::CoreServices& coreServices);
	void Release();
	void Update();
	void Render();

private:
	TowerAppServices m_Services;

	Character* m_pCharacter;
	Tower* m_pTower{};
	EnemySystem* m_pEnemySystem{};
};
