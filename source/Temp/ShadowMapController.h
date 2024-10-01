#pragma once

#include <Applied\NewUi\Elements\Extender.h>
#include <Applied\NewUi\Elements\Image.h>

namespace TowerGame
{
class ShadowRenderer;

class ShadowMapController
{
public:
	ShadowMapController() = default;

	void Start(ShadowRenderer& shadowMapRenderer);
	void OnCanvasResized(ShadowRenderer& shadowMapRenderer);

private:
	static Rendering::Texture MakeTexture(ShadowRenderer& shadowMapRenderer);
	static Float2 CalcImageSize(const float maxSide = 200.f);

	NewUi::Extender* m_pExtender;
	NewUi::Image* m_pImage;
};
}
