#include "pch.h"
#include "DuneCreator.h"

#include "../../TowerAppRenderer.h"
#include "Rendering/Image.h"
#include "Rendering/Renderers/Texture2DRenderer.h"

DuneCreator::DuneCreator(TowerAppRenderer& renderer)
	: m_Renderer{ renderer }
	, m_pImage{ new Rendering::Image{{32,32}, {0,1,0}} }
{
	const int idx = renderer.GetTexture2DRenderer().Add({ m_pImage, true }, { {-.5,-.5}, {1,1}, {1,1},{0,0},{1,1,1} });
	m_pTexture = &renderer.GetTexture2DRenderer().GetTexture(idx);
}

DuneCreator::~DuneCreator()
{
	delete m_pImage;
}

float counter = 0;
int hits = 0;
void DuneCreator::Update()
{
	counter += Globals::DeltaTime;
	if (counter < .5f) return;
	counter = 0;
	hits++;

	m_pImage->SetColor({ hits % 2 == 0 ? 1.f : 0, hits % 2 == 1 ? 1.f : 0, 1 });
	m_pTexture->Update(*m_pImage);
}
