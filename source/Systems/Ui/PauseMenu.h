#pragma once

#include <Ui/Elements/AllElements.h>

namespace TowerGame
{
class PauseMenu
{
public:
	PauseMenu();
	~PauseMenu();

	void Toggle();
	void SetOpen(bool setOpen);
	void Open();
	void Close();

private:
	bool m_IsPaused{ false };

	Ui::Elem* m_pRoot{};

	enum class Buttons {
		CONTINUE, EXIT,
		END
	};

	static void BtnClicked(unsigned iBtn);
};
}