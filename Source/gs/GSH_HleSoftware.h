#pragma once

#include "gs/GSHandler.h"
#include "GSH_Hle.h"
#include "win32/Window.h"
#include "win32/ComPtr.h"
#include "ui_win32/OutputWnd.h"
#include <d3d9.h>
#include <d3dx9.h>

class CGSH_HleSoftware : public CGSHandler, public CGHSHle
{
public:
	CGSH_HleSoftware(Framework::Win32::CWindow*);
    virtual                         ~CGSH_HleSoftware();

    static FactoryFunction          GetFactoryFunction(Framework::Win32::CWindow*);

	// ----------- Hle additions

	/**
	Transfers a macroblocked image as returned from sceMpegGetPicture.
	*/
	void	TransferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y) override;

	void	CGSH_HleSoftware::DrawSprite(int xpos, int ypos, int width, int height, uint32 vertexRGBA, uint8* texGsPacketData, bool interlaced, uint64 alphaReg) override;

	// ---------- end hle additions

protected:
	//virtual void					ResetImpl() override;
	virtual void					InitializeImpl() override;
	virtual void					ReleaseImpl() override;
	virtual void					FlipImpl() override;

private:
	typedef Framework::Win32::CComPtr<IDirect3D9> Direct3DPtr;
	typedef Framework::Win32::CComPtr<IDirect3DVertexBuffer9> VertexBufferPtr;
	typedef Framework::Win32::CComPtr<IDirect3DDevice9> DevicePtr;
	typedef Framework::Win32::CComPtr<IDirect3DTexture9> TexturePtr;
	typedef Framework::Win32::CComPtr<IDirect3DSurface9> SurfacePtr;
	typedef Framework::Win32::CComPtr<ID3DXEffect> EffectPtr;

	static CGSHandler*              GSHandlerFactory(Framework::Win32::CWindow*);
    
	void							ProcessImageTransfer() override;
	void							ProcessClutTransfer(uint32, uint32) override;
	void							ProcessLocalToLocalTransfer() override;

	void							ReadFramebuffer(uint32, uint32, void*) override;

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
	void SetupBlendingFunction(uint64 alphaReg);

	EffectPtr						CreateEffectFromResource(const TCHAR*);

	EffectPtr						m_mainFx;
	COutputWnd*						m_outputWnd;
	Direct3DPtr						m_d3d;
	DevicePtr						m_device;
	VertexBufferPtr					m_quadVb;
	bool							m_sceneBegun;

	TexturePtr blockedTex;
};


