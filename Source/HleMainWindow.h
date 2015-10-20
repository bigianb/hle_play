#pragma once

#include "ui_win32/MainWindow.h"

class HleMainWindow : public CMainWindow
{
public:
							HleMainWindow(CPS2VM& ps2vm) : CMainWindow(ps2vm) {}
protected:
	void					CreateGSHandler() override;
};