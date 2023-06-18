#include "pch.h"
#include "GameplaySystems.h"

const GameplaySystems* GameplaySystems::m_pStatic = nullptr;

GameplaySystems::GameplaySystems()
{
	if (m_pStatic) Logger::PrintError("A GameplaySystem-class has been made before");
	m_pStatic = this;
}
