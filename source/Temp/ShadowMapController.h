#pragma once

namespace TowerGame
{
class ShadowRenderer;

class ShadowMapController
{
public:
	ShadowMapController();

	void Start(ShadowRenderer& shadowMapRenderer);

private:
	void InitDemoQuad(Rendering::Texture* pTexture);
	static Rendering::Texture* MakeTexture(ShadowRenderer& shadowMapRenderer);
};
}
