#pragma once
#include <Game/Transform.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Transform.h>

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

	void Update(const DirectX::XMMATRIX& cameraWorld, Rendering::R_LambertCam_Tex_Transform& renderer);

	void Register(Rendering::R_LambertCam_Tex_Transform& renderer);

private:
	struct BowData
	{
		Game::Transform* pTransform;
		Math::Float3 Velocity;
	};

	DirectX::XMMATRIX m_LocalMatrix{};
	DirectX::XMMATRIX m_WorldMatrix{};

	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};
	Rendering::Mesh* m_pArrowMesh{};

	std::vector<BowData> m_BowData{};
};
