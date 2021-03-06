#pragma once

#include "gs/GSHandler.h"
#include "GSH_Hle.h"
#include "win32/Window.h"
#include "win32/ComPtr.h"
#include "ui_win32/OutputWnd.h"
#include <d3d9.h>
#include <d3dx9.h>

class Mesh;

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
	void	transferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y) override;

	void	drawSprite(int xpos, int ypos, int width, int height, uint32 vertexRGBA, uint8* texGsPacketData, bool interlaced, uint64 alphaReg) override;

	/**
	Sets the active texture given a linear block of 32 bit pixels.
	*/
	virtual void setTexture32(unsigned char* data, int dataLength, int width, int height, bool interlaced);

	/**
	Draws a sprite using the current texture.
	*/
	virtual void drawSprite(int xpos, int ypos, int u0, int v0, int width, int height, uint32 vertexRGBA, bool useTexture);

	virtual void setAlphaBlendFunction(uint64 alphaReg);

	/**
	Sets the scissor rectangle and enables scissoring.
	*/
	virtual void setScissorRect(uint32 scissorX0, uint32 scissorY0, uint32 scissorX1, uint32 scissorY1);

	/**
	Disables scissoring.
	*/
	virtual void disableScissor();

	virtual void drawModel(int texWidth, int texHeight, uint8* texGsPacketData, std::vector<Mesh*>* meshList, float* xform);

	virtual void displayBackBufferAndClear();

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
	typedef Framework::Win32::CComPtr<IDirect3DVertexDeclaration9> VertexDeclarationPtr;

	static CGSHandler*              GSHandlerFactory(Framework::Win32::CWindow*);
    
	void							ProcessHostToLocalTransfer() override;
	void							ProcessLocalToHostTransfer() override;
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
	void SetWorldMatrix(int nWidth, int nHeight);
	
	// Used only to know whether we need to reload a texture.
	unsigned char* currentTextureSourcePointer;
	int currentTextureHeight;
	int currentTextureWidth;
	TexturePtr currentTexture;

	EffectPtr						CreateEffectFromResource(const TCHAR*);

	EffectPtr						m_mainFx;
	COutputWnd*						m_outputWnd;
	Direct3DPtr						m_d3d;
	DevicePtr						m_device;
	VertexBufferPtr					m_quadVb;
	VertexDeclarationPtr			m_quadVertexDecl;
	bool							m_sceneBegun;
	D3DXMATRIX						m_worldViewMatrix;
	TexturePtr blockedTex;
};


