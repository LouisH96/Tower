#pragma once
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

	void Render(TowerAppRenderer& renderer, const TowerAppRenderData& data);

	Rendering::Mesh& GetBowMesh() const { return *m_pBowMesh; }
	Rendering::Texture& GetTexture() const { return *m_pTexture; }
	const DirectX::XMMATRIX& GetWorld() const { return m_World; }
	const DirectX::XMMATRIX& GetInverseTranspose() const { return m_InverseTranspose; }

private:
	DirectX::XMMATRIX m_World{};
	DirectX::XMMATRIX m_InverseTranspose{};
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};
	Rendering::Mesh* m_pArrowMesh{};
};
