#pragma once
#include <Transform.h>
#include <Environment/Terrain.h>

namespace MyEngine
{
	namespace Rendering
	{
		class Texture;
	}
}

class Bow
{
public:

	Bow();
	~Bow();

	void Update(const Transform& cameraTransform);
	void LinkRenderers();

	const Transform& GetWorldTransform() const { return m_WorldTransform; }

private:
	Transform m_LocalTransform{};
	Transform m_WorldTransform{};

	Rendering::Texture* m_pTexture{};
	Rendering::Mesh* m_pBowMesh{};

	int m_ArrowIdx{ -1 };
};
