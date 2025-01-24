#pragma once
#include "Rendering/Renderers/RendererFactory.h"
#include "Rendering/Renderers/R_LambertCam_Tex_Transform.h"
#include "Rendering/Renderers/R_LambertLight_Col.h"
#include "Rendering/Renderers/TextureRenderer.h"
#include "Rendering/Structs/ConstantBufferTypes.h"
#include "Rendering/Structs/VertexTypes.h"
#include <Renderer\TerrainRenderer.h>

namespace MyEngine
{
namespace Rendering
{
class R_LambertCam_Tex_Tran_Inst;
class FpsDisplay;
class Texture2DRenderer;
}
}

namespace TowerGame
{
class ArrowSystem;

class RenderSystems
{
public:
	using SimpleRenderer = Rendering::RendererFactory::SimpleRenderer;
	
	static TowerGame::TerrainRenderer& GetTerrainRenderer() { return *m_pStatic->pTerrainRenderer; }
	static SimpleRenderer& GetSimpleRenderer() { return *m_pStatic->pSimpleRenderer; }

	RenderSystems();

	TowerGame::TerrainRenderer* pTerrainRenderer{};
	SimpleRenderer* pSimpleRenderer{};

private:
	static const RenderSystems* m_pStatic;
};
}
