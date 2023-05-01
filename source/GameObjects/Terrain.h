#pragma once

namespace MyEngine
{
	namespace Game
	{
		class Transform;
	}
}

class TowerAppRenderer;

namespace MyEngine
{
	namespace Rendering
	{
		class Gpu;
		class Texture;
		class Mesh;
	}
}

class Terrain
{
public:
	explicit Terrain(Rendering::Gpu& gpu);
	~Terrain();

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	void Register(const TowerAppRenderer& renderer) const;

private:
	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pMesh{};
};
