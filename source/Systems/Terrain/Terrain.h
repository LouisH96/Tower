#pragma once
#include "HeightMap.h"
#include <Systems\StaticMeshes\StaticMeshSystem.h>

namespace MyEngine::Rendering
{
class Mesh;
}

namespace TowerGame
{
class Terrain
{
public:
	Terrain() = default;
	~Terrain() = default;
	void Init(const Float3& origin, const Float2& posSize, const Float2& negSize); //posSize is towards rightTop, negSize is towards leftBot from origin

	Terrain(const Terrain& other) = delete;
	Terrain(Terrain&& other) noexcept = delete;
	Terrain& operator=(const Terrain& other) = delete;
	Terrain& operator=(Terrain&& other) noexcept = delete;

	float GetHeight(Float2 point) const; //xz
	const Float2& GetSize() const { return m_HeightMap.GetSize(); }

	void GenerateMeshIn(StaticMeshSystem::MeshData& data);

private:
	HeightMap m_HeightMap;
	Float3 m_Position;

	void FillHeightMap();
};
}
