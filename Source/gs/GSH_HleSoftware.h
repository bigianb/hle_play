#pragma once

#include "gs/GSHandler.h"
#include "win32/Window.h"
#include "win32/ComPtr.h"
#include "ui_win32/OutputWnd.h"
#include <d3d9.h>

class CGSH_HleSoftware : public CGSHandler
{
public:
	CGSH_HleSoftware(Framework::Win32::CWindow*);
    virtual                         ~CGSH_HleSoftware();

    static FactoryFunction          GetFactoryFunction(Framework::Win32::CWindow*);

protected:
	//virtual void					ResetImpl() override;
	virtual void					InitializeImpl() override;
	virtual void					ReleaseImpl() override;
	virtual void					FlipImpl() override;

private:
   	static CGSHandler*              GSHandlerFactory(Framework::Win32::CWindow*);
    
	void							ProcessImageTransfer() override;
	void							ProcessClutTransfer(uint32, uint32) override;
	void							ProcessLocalToLocalTransfer() override;

	void							ReadFramebuffer(uint32, uint32, void*) override;

    unsigned int                    m_nWidth;
    unsigned int                    m_nHeight;

	void							BeginScene();
	void							EndScene();
	bool							TestDevice();
	void							CreateDevice();
	void							OnDeviceReset();
	void							OnDeviceResetting();
	D3DPRESENT_PARAMETERS			CreatePresentParams();
	void							PresentBackbuffer();

	TexturePtr m_framebufferTexture;
	void displayFrameBuffer();
	void SetReadCircuitMatrix(int nWidth, int nHeight);

	typedef Framework::Win32::CComPtr<IDirect3D9> Direct3DPtr;
	typedef Framework::Win32::CComPtr<IDirect3DDevice9> DevicePtr;

	COutputWnd*						m_outputWnd;
	Direct3DPtr						m_d3d;
	DevicePtr						m_device;
	bool							m_sceneBegun;
};


