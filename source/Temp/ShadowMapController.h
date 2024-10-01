#pragma once

#include <Applied\NewUi\Elements\Extender.h>
#include <Applied\NewUi\Elements\Image.h>
#include <Rendering\State\DepthStencilBuffer.h>

namespace TowerGame
{
class ShadowRenderer;

class ShadowMapController
{
public:
	ShadowMapController() = default;

	void Start(Rendering::DepthStencilBuffer& dsBufferShadow);
	void OnCanvasResized(Rendering::DepthStencilBuffer& dsBufferShadow);

private:
	static Rendering::Texture MakeTexture(Rendering::DepthStencilBuffer& dsBufferShadow);
	static Float2 CalcImageSize(const float maxSide = 200.f);

	NewUi::Extender* m_pExtender;
	NewUi::Image* m_pImage;
};
}
