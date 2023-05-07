#pragma once
#include "Physics/CollisionDetection.h"

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
	explicit Terrain(Rendering::Gpu& gpu, const TowerAppRenderer& renderer, const Math::Float3& position, const Math::Float2& size);
	~Terrain();

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	bool IsColliding(const Math::Float3& begin, const Math::Float3& end) const;
	bool IsColliding(const Math::Float3& begin, const Math::Float3& end, Physics::CollisionDetection::Collision& collision) const;

	const Math::Float2& GetSize() const { return m_Size; }

private:
	Rendering::Mesh* m_pMesh{};

	Math::Float2 m_Size;
	Array<Math::Float3> m_Points;
	Array<Math::Float3> m_TriangleNormals;
	Array<int> m_Indices;

	void FromHeightMap(const Rendering::Gpu& gpu, const Math::Float3& position);
};
