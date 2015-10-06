#include "ui_win32/MainWindow.h"

#include "BgdaEeExecutor.h"

#ifdef VTUNE_ENABLED
#include <jitprofiling.h>
#endif

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	CPS2VM virtualMachine;
	CMainWindow MainWindow(virtualMachine);

	virtualMachine.m_ee->SetExecutor(std::make_unique<BgdaEeExecutor>(virtualMachine.m_ee->m_EE, virtualMachine.m_ee->m_ram));

	MainWindow.Loop();
#ifdef VTUNE_ENABLED
	iJIT_NotifyEvent(iJVM_EVENT_TYPE_SHUTDOWN, NULL);
#endif
	return 0;
}
