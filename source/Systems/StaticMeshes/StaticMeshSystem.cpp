#include "pch.h"
#include "StaticMeshSystem.h"

using namespace TowerGame;

void StaticMeshSystem::Init(MeshData& data)
{
	m_MeshBuffers.CreateBuffers(data);
}

void StaticMeshSystem::Render()
{
	m_MeshBuffers.ActivateAndDraw();
}
