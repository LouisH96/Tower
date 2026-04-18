#pragma once

#include <Ui/Elements/AllElements.h>

namespace TowerGame
{
struct Style
{
	static const Float3 ColorLight;
	static const Float3 ColorMedium;
	static const Float3 ColorDark;

	static SimpleParent* Border(float innerMargin = 5);

	static void CreateButton(SimpleParent* pParent, std::string& text, Button::Function function, Button::FunctionArg arg);

};

}