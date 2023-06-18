#pragma once
#include <Game/Transform.h>
#include "../Environment/Terrain.h"
#include "../Renderers/ArrowRenderer.h"

class Tower;
class TowerAppRenderer;
struct TowerAppRenderData;

namespace MyEngine
{
	namespace Rendering
	{
		class R_LambertCam_Tex_Transform;
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
	void Render(const Camera& camera);

	void Register(Rendering::R_LambertCam_Tex_Transform& renderer);
	void Register(const Terrain& terrain);
	void Register(const Tower& tower);

	void SetArrowTransform(int idx, const Transform& transform);

private:
	struct ArrowData
	{
		Transform Transform;
		Float3 Velocity;
	};

	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	ArrowRenderer m_ArrowRenderer;
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};

	std::vector<ArrowData> m_ArrowData{};

	const Terrain* m_pTerrain{};
	const Tower* m_pTower{};

	void UpdateArrow(const TowerAppServices& services, int arrowIdx, ArrowData& arrowData) const;
};
