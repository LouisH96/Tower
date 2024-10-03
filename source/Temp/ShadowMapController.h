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

private:
	static Rendering::Texture MakeTexture(ShadowRenderer& shadowMapRenderer);
};
}
