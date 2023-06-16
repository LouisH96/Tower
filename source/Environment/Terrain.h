#pragma once
#include "HeightMap.h"

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
	static void Init(const TowerAppServices& services, const Float3& position, const Float2& size);
	static void Release();
	static Terrain& Get() { return *m_pStatic; }

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	float GetHeight(const Float2& point) const; //xz
	const Float2& GetSize() const { return m_HeightMap.GetSize(); }

private:
	explicit Terrain(const TowerAppServices& services, const Float3& position, const Float2& size);
	~Terrain();

	static Terrain* m_pStatic;

	Rendering::Mesh* m_pMesh{};
	HeightMap m_HeightMap;

	static HeightMap MakeHeightMap(const Float2& worldSize, const Int2& nrPoints);
};
