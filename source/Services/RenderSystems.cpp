#include "pch.h"
#include "RenderSystems.h"

using namespace TowerGame;

const RenderSystems* RenderSystems::m_pStatic = nullptr;

RenderSystems::RenderSystems()
{
	if (m_pStatic) Logger::PrintError("A RenderSystems-class has been made before");
	m_pStatic = this;
}