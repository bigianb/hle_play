#pragma once

#include "../GSH_HleOgl.h"
#include "win32/Window.h"
#include "ui_win32/SettingsDialogProvider.h"

class CGSH_HleOglWin32 : public CGSH_HleOgl, public CSettingsDialogProvider
{
public:
									CGSH_HleOglWin32(Framework::Win32::CWindow*);
	virtual							~CGSH_HleOglWin32();

	static FactoryFunction			GetFactoryFunction(Framework::Win32::CWindow*);

	virtual void					InitializeImpl();
	virtual void					ReleaseImpl();

	CSettingsDialogProvider*		GetSettingsDialogProvider();

	Framework::Win32::CModalWindow* CreateSettingsDialog(HWND);
	void							OnSettingsDialogDestroyed();

protected:
	virtual void					PresentBackbuffer();

private:
	static CGSHandler*				GSHandlerFactory(Framework::Win32::CWindow*);

	Framework::Win32::CWindow*		m_outputWnd = nullptr;

	HGLRC							m_context = nullptr;
	HDC								m_dc = nullptr;
	static PIXELFORMATDESCRIPTOR	m_pfd;
};
