#ifndef _RENDERERSETTINGSWND_H_
#define _RENDERERSETTINGSWND_H_

#include "win32/ModalWindow.h"
#include "layout/VerticalLayout.h"
#include "win32/ListView.h"
#include "win32/Button.h"
#include "win32/ComboBox.h"

class CGHSHle;

class HleRendererSettingsWnd : public Framework::Win32::CModalWindow
{
public:
									HleRendererSettingsWnd(HWND, CGHSHle*);
	virtual							~HleRendererSettingsWnd();

protected:
	long							OnCommand(unsigned short, unsigned short, HWND);

private:
	void							RefreshLayout();
	void							CreateExtListColumns();
	void							UpdateExtList();
	void							Save();

	bool							ProcessCheckBoxMessage(HWND, Framework::Win32::CButton*, bool*);

	Framework::FlatLayoutPtr		m_pLayout;
	Framework::Win32::CListView*	m_pExtList;
	Framework::Win32::CButton*		m_pLineCheck;
	Framework::Win32::CButton*		m_pForceBilinearCheck;
	Framework::Win32::CButton*		m_pOk;
	Framework::Win32::CButton*		m_pCancel;

	bool							m_nLinesAsQuads;
	bool							m_nForceBilinearTextures;
};

#endif
