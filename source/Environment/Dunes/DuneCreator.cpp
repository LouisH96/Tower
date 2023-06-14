#include "pch.h"
#include "DuneCreator.h"

#include "../../TowerAppRenderer.h"
#include "Rendering/Image.h"
#include "Rendering/Renderers/Texture2DRenderer.h"

DuneCreator::DuneCreator(TowerAppRenderer& renderer)
	: m_Renderer{ renderer }
	, m_pImage{ new Rendering::Image{{32,32}, {0,1,0}} }
{
	renderer.GetTexture2DRenderer().Add({ m_pImage }, { {-.5,-.5}, {1,1}, {1,1},{0,0},{1,1,1} });
}

DuneCreator::~DuneCreator()
{
	delete m_pImage;
}
