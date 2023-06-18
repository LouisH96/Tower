#pragma once
class EnemySystem;
struct CollisionService;
class Bow;
class Tower;
class Terrain;
class Character;

class GameplaySystems
{
public:
	static Terrain& GetTerrain() { return *m_pStatic->pTerrain; }
	static Tower& GetTower() { return *m_pStatic->pTower; }
	static Character& GetCharacter() { return *m_pStatic->pCharacter; }
	static Bow& GetBow() { return *m_pStatic->pBow; }
	static EnemySystem& GetEnemySystem() { return *m_pStatic->pEnemySystem; }
	static CollisionService& GetCollisionService() { return *m_pStatic->pCollisionService; }

	GameplaySystems();

	Terrain* pTerrain{};
	Tower* pTower{};
	Character* pCharacter{};
	Bow* pBow{};
	EnemySystem* pEnemySystem{};
	CollisionService* pCollisionService{};

private:
	static const GameplaySystems* m_pStatic;
};
