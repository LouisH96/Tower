#pragma once
#include <Game/Transform.h>
#include <Environment/Terrain.h>
#include <Renderers/ArrowRenderer.h>

class Bow
{
public:

	Bow();
	~Bow();

	void Update(const Transform& cameraTransform);

	void LinkRenderers();

	void SetArrowTransform(int idx, const Transform& transform);

private:
	struct ArrowData
	{
		Transform Transform;
		Float3 Velocity;
	};

	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};

	std::vector<ArrowData> m_ArrowData{};

	void UpdateArrow(int arrowIdx, ArrowData& arrowData) const;
};
