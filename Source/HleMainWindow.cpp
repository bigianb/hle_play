#include "HleMainWindow.h"
#include "ui_win32/GSH_OpenGLWin32.h"

void HleMainWindow::CreateGSHandler()
{
	m_virtualMachine.CreateGSHandler(CGSH_OpenGLWin32::GetFactoryFunction(m_outputWnd));
}
