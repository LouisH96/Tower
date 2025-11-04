#include "pch.h"
#include "TowerApp.h"

#include <Debug/Rendering/DebugRenderer.h>
#include <Framework/Resources.h>
#include <Generation/Shapes/ArrowGenerator.h>
#include <Rendering/Renderers/R_LambertCam_Tex_Tran_Inst.h>
#include <Rendering/Renderers/Texture2DRenderer.h>

#include "App/Win32/Window.h"

using namespace TowerGame;

TowerApp* TowerApp::pApp{ nullptr };

TowerApp::TowerApp()
{
	pApp = this;
	m_Systems.Init();
}

TowerApp::~TowerApp()
{
}

void TowerApp::OnCanvasResized(const App::ResizedEvent& event)
{
	m_Renderer.OnCanvasResized(event);
}

float angle{ 0 };
bool rotate{ false };
void TowerApp::EarlyUpdate()
{
	m_Systems.EarlyUpdate();

	if (KEYBOARD.IsPressed(KEYBOARD.ScanCodeToVk(KeyCode::R)))
	{
		rotate = !rotate;
		Logger::Print("Rotate", rotate);
	}
	if (rotate)
	{

		if (KEYBOARD.IsPressed(KEYBOARD.ScanCodeToVk(KeyCode::ArrowLeft)))
		{
			angle -= 10 * Constants::TO_RAD;
			Logger::Print("Angle", angle * Constants::TO_DEG);
		}
		else if (KEYBOARD.IsPressed(KEYBOARD.ScanCodeToVk(KeyCode::ArrowRight)))
		{
			angle += 10 * Constants::TO_RAD;
			Logger::Print("Angle", angle * Constants::TO_DEG);
		}
		const float rotateAngle = (5.f * .99f) * Constants::TO_RAD;
		const ShadowSystem2& shadow{ m_Systems.Shadows2 };

		const Float3 fixForward{ shadow.GetPrevForward() };
		const Float3 fixRight{ Float3{0,1,0}.Cross(fixForward).Normalized() };
		const Float3 fixUp{ fixForward.Cross(fixRight).Normalized() };

		const float newZ{ std::cosf(rotateAngle) };
		float newY{ std::sinf(angle) * std::sinf(rotateAngle) };
		float newX{ std::cosf(angle) * std::sinf(rotateAngle) };

		const Float3 newForward{ fixForward * newZ + fixRight * newX + fixUp * newY };
		const Float3 newRight{ Float3{0,1,0}.Cross(newForward).Normalized() };
		const Float3 newUp{ newForward.Cross(newRight).Normalized() };

		CAMERA.SetForward(newForward);
		CAMERA.SetRight(newRight);
		CAMERA.SetUp(newUp);

		//angle += 90 * Constants::TO_RAD * DELTA_TIME;
	}
}

void TowerApp::Update()
{
	if (Globals::pKeyboard->IsDown_('\x1b'))
	{
		Globals::pWindow->SetIsDestroyed();
		return;
	}

	m_Systems.Update();
}

void TowerApp::PreRender()
{
	m_Renderer.PreRender();
}

void TowerApp::Render()
{
	m_Renderer.Render();
}

void TowerApp::RenderUi()
{
}
