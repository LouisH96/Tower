#include "pch.h"
#include "Style.h"

#include <Ui/Elements/AllElements.h>

using namespace Ui;

namespace TowerGame
{
const Float3 Style::ColorLight{ Float3::Color(223,201,172) };
const Float3 Style::ColorMedium{ Float3::Color(202,175,137) };
const Float3 Style::ColorDark{ Float3::Color(123,106,79) };

SimpleParent* Style::Border(float innerMargin)
{
	Ui::Border* pBorder{ new Ui::Border(UiSystem::BORDER_THICKNESS, UiSystem::BORDER_THICKNESS * 3) };
	pBorder->SetBorderColor(ColorMedium);
	pBorder->SetBackgroundColor(ColorDark);

	return pBorder;
}

void Style::CreateButton(SimpleParent* pParent, std::string& text, Button::Function function, Button::FunctionArg arg)
{
	Button* pButton{ new Button(text, function, arg) };
	pButton->SetBorderColor(Style::ColorMedium);
	pButton->SetBgColor(Style::ColorDark);
	pButton->SetInteractColor(Style::ColorLight);
	pParent->AddChild(pButton);
}
}
