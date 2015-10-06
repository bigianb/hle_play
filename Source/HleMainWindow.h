#pragma once

#include "ui_win32/MainWindow.h"

class HleMainWindow : CMainWindow
{
protected:
	void					CreateGSHandler() override;
};