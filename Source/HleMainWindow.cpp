#include "HleMainWindow.h"
#include "ui_win32/GSH_HleOglWin32.h"

void HleMainWindow::CreateGSHandler()
{
	m_virtualMachine.CreateGSHandler(CGSH_HleOglWin32::GetFactoryFunction(m_outputWnd));
}
