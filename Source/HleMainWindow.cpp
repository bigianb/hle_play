#include "HleMainWindow.h"
#include "gs/GSH_HleSoftware.h"

void HleMainWindow::CreateGSHandler()
{
	m_virtualMachine.CreateGSHandler(CGSH_HleSoftware::GetFactoryFunction(m_outputWnd));
}
