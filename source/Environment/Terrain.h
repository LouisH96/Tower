#pragma once
#include "Physics/CollisionDetection.h"

struct TowerAppServices;

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
	explicit Terrain(TowerAppServices& services, const Math::Float3& position, const Math::Float2& size);
	~Terrain();

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	const Math::Float2& GetSize() const { return m_Size; }

private:
	Rendering::Mesh* m_pMesh{};
	Math::Float2 m_Size;

	void FromHeightMap(TowerAppServices& services, const Math::Float3& position);
};
