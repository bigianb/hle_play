#include "GSH_HleOglWin32.h"
#include "HleRendererSettingsWnd.h"

PIXELFORMATDESCRIPTOR CGSH_HleOglWin32::m_pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	32,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0,
	32,
	0,
	0,
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
};

CGSH_HleOglWin32::CGSH_HleOglWin32(Framework::Win32::CWindow* outputWindow)
: m_outputWnd(outputWindow)
{

}

CGSH_HleOglWin32::~CGSH_HleOglWin32()
{

}

CGSHandler::FactoryFunction CGSH_HleOglWin32::GetFactoryFunction(Framework::Win32::CWindow* outputWindow)
{
	return std::bind(&CGSH_HleOglWin32::GSHandlerFactory, outputWindow);
}

void CGSH_HleOglWin32::InitializeImpl()
{
	m_dc = GetDC(m_outputWnd->m_hWnd);
	unsigned int pf = ChoosePixelFormat(m_dc, &m_pfd);
	SetPixelFormat(m_dc, pf, &m_pfd);
	m_context = wglCreateContext(m_dc);
	wglMakeCurrent(m_dc, m_context);

	auto result = glewInit();
	assert(result == GLEW_OK);

#ifdef GLES_COMPATIBILITY
	if(wglCreateContextAttribsARB != nullptr)
	{
		auto prevContext = m_context;

		static const int attributes[] = 
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_context = wglCreateContextAttribsARB(m_dc, nullptr, attributes);
		assert(m_context != nullptr);

		wglMakeCurrent(m_dc, m_context);
		auto deleteResult = wglDeleteContext(prevContext);
		assert(deleteResult == TRUE);
	}
#endif

	CGSH_HleOgl::InitializeImpl();
}

void CGSH_HleOglWin32::ReleaseImpl()
{
	CGSH_HleOgl::ReleaseImpl();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_context);
}

void CGSH_HleOglWin32::PresentBackbuffer()
{
	SwapBuffers(m_dc);
}

CSettingsDialogProvider* CGSH_HleOglWin32::GetSettingsDialogProvider()
{
	return this;
}

Framework::Win32::CModalWindow* CGSH_HleOglWin32::CreateSettingsDialog(HWND hParent)
{
	return new HleRendererSettingsWnd(hParent, this);
}

void CGSH_HleOglWin32::OnSettingsDialogDestroyed()
{
	LoadSettings();
	TexCache_Flush();
	PalCache_Flush();
}

CGSHandler* CGSH_HleOglWin32::GSHandlerFactory(Framework::Win32::CWindow* outputWindow)
{
	return new CGSH_HleOglWin32(outputWindow);
}
