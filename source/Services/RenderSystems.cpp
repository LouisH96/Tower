#include "pch.h"
#include "RenderSystems.h"

const RenderSystems* RenderSystems::m_pStatic = nullptr;

RenderSystems::RenderSystems()
{
	if (m_pStatic) Logger::PrintError("A RenderSystems-class has been made before");
	m_pStatic = this;
}