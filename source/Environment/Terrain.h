#pragma once
#include "HeightMap.h"

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
	explicit Terrain(const Float3& position, const Float2& size);
	~Terrain();
	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	void LinkRenderers();

	float GetHeight(const Float2& point) const; //xz
	const Float2& GetSize() const { return m_HeightMap.GetSize(); }

private:
	Rendering::Mesh* m_pMesh{};
	HeightMap m_HeightMap;

	static HeightMap MakeHeightMap(const Float2& worldSize, const Int2& nrPoints);
};
