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
		MakeTexture(shadowMapRenderer)) };
	NewUi::Image* pImage{ new NewUi::Image(textureId) };
	pExtender->AddChild(pImage);

	NEW_UI.AfterEdit();
}

Texture ShadowMapController::MakeTexture(ShadowRenderer& renderer)
{
	//Get
	DepthStencilBuffer& dsBuffer{ renderer.GetDepthStencilBuffer() };
	ID3D11DepthStencilView* pDsView{ dsBuffer.GetView() };

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	pDsView->GetDesc(&dsViewDesc);

	ID3D11Resource* dsResource;
	pDsView->GetResource(&dsResource);

	//Make ShaderResourceViewDesc
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
	resourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = 1;

	//Make ShaderResourceView
	ID3D11ShaderResourceView* pShaderResourceView;
	const HRESULT result{
		Globals::pGpu->GetDevice().CreateShaderResourceView(dsResource, &resourceViewDesc, &pShaderResourceView)
	};
	SAFE_RELEASE(dsResource);

	if (FAILED(result)) {
		Logger::PrintError("[ShadowMapController::MakeTexture]");
		SAFE_RELEASE(pShaderResourceView);
	}

	return Texture{ pShaderResourceView };
}

