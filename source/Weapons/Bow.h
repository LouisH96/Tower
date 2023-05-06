#pragma once
#include <Game/Transform.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>

#include "../Environment/Terrain.h"

class Tower;
class TowerAppRenderer;
struct TowerAppRenderData;

namespace MyEngine
{
	namespace Rendering
	{
		class Gpu;
		class Texture;
		class Mesh;
	}
}

class Bow
{
public:
	Bow(Rendering::Gpu& gpu);
	~Bow();

	void Update(const Game::Transform& cameraTransform);

	void Register(Rendering::R_LambertCam_Tex_Transform& renderer);
	void Register(const Terrain& terrain);
	void Register(const Tower& tower);

private:
	struct ArrowData
	{
		Game::Transform* pTransform;
		Math::Float3 Velocity;
	};

	Game::Transform m_LocalTransform{};
	Game::Transform m_WorldTransform{};

	Rendering::R_LambertCam_Tex_Transform* m_pRenderer{};
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};
	Rendering::Mesh* m_pArrowMesh{};

	std::vector<ArrowData> m_ArrowData{};

	const Terrain* m_pTerrain{};
	const Tower* m_pTower{};

	void UpdateArrow(ArrowData& arrowData) const;
};
