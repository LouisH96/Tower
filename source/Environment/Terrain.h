#pragma once

struct TowerAppServices;

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
	}
}

class Terrain
{
public:
	explicit Terrain(TowerAppServices& services, const Float3& position, const Float2& size);
	~Terrain();

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	const Float2& GetSize() const { return m_Size; }

private:
	Rendering::Mesh* m_pMesh{};
	Float2 m_Size;

	void FromHeightMap(TowerAppServices& services, const Float3& position);
};
