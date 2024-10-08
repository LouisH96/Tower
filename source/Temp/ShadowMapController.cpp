#include "pch.h"
#include "ShadowMapController.h"

#include <Applied\NewUi\Elements\AnchorParent.h>
#include <Applied\NewUi\Elements\Box.h>
#include <Applied\NewUi\Elements\Margin.h>
#include <Applied\NewUi\NewUiSystem.h>
#include <Character\Character.h>
#include <DataStructures\Adders\ArrayAdder.h>
#include <Generation\RectGenerator.h>
#include <Renderer\ShadowRenderer.h>
#include <Rendering\Canvas.h>
#include <Rendering\State\DepthStencilBuffer.h>
#include <Rendering\State\Mesh.h>
#include <Rendering\State\Texture.h>
#include <Services\GameplaySystems.h>
#include <Services\RenderSystems.h>
#include <TowerApp.h>

#include <d3d11.h>

using namespace TowerGame;
using namespace Rendering;

void ShadowMapController::Start(ShadowRenderer& shadowMapRenderer)
{
	//Ui
	using namespace NewUi;
	NEW_UI.BeforeEdit();

	AnchorParent* pAnchor{ new AnchorParent() };
	NEW_UI.AddChild(pAnchor);

	Margin* pMargin{ new Margin(5.f) };
	pAnchor->AddChild(pMargin, { 1,1 });

	Extender* pExtender{ new Extender(SizeDef{ SizeDef::Mode::Pixels, {200,200} }) };
	pMargin->AddChild(pExtender);

	const unsigned textureId{ NEW_UI.GetImageRenderer().AddTexture(
		Texture{shadowMapRenderer.GetDepthStencilBuffer().MakeShaderResourceView()}) };
	NewUi::Image* pImage{ new NewUi::Image(textureId) };
	pExtender->AddChild(pImage);

	NEW_UI.AfterEdit();
}

