#pragma once
#include <Rendering\Structs\VertexTypes.h>
#include <Systems\StaticMeshes\StaticMeshSystem.h>
#include <Systems\StaticMeshes\StaticModelSystem.h>

namespace MyEngine
{
	namespace Rendering
	{
		class Mesh;
	}
}

namespace TowerGame
{
class Tower
{
public:
	struct Desc {
		Float3 Position;
		Float2 RoofSize;
		float Height;
	};

public:
	Tower() = default;
	void Init(Desc& desc);

	~Tower() = default;

	void AddModelsIn(
		StaticModelSystem::InitData& staticModelData);

private:
	List<Rendering::V_PosNorCol> m_Vertices;
	Float3 m_Position;
	Float2 m_RoofSize;
	float m_Height{};
};
}
