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
	Bow();
	~Bow();

	void Update(const TowerAppServices& services, const Transform& cameraTransform);

	void Register(Rendering::R_LambertCam_Tex_Transform& renderer);
	void Register(const Terrain& terrain);
	void Register(const Tower& tower);

private:
	struct ArrowData
	{
		Transform* pTransform;
		Float3 Velocity;
	};

	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::R_LambertCam_Tex_Transform* m_pRenderer{};
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};
	Rendering::Mesh* m_pArrowMesh{};

	std::vector<ArrowData> m_ArrowData{};

	const Terrain* m_pTerrain{};
	const Tower* m_pTower{};

	void UpdateArrow(const TowerAppServices& services, ArrowData& arrowData) const;
};
