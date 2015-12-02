#include "GSH_HleSoftware.h"
#include "PtrMacro.h"
#include <d3dx9math.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// Enable debugging in release mode
#pragma optimize( "", off )

struct CUSTOMVERTEX
{
	float x, y, z;
	DWORD color;
	float u, v;
};

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define FB_WIDTH_PIX 640
#define FB_HEIGHT_PIX 400

CGSH_HleSoftware::CGSH_HleSoftware(Framework::Win32::CWindow* outputWindow) :
	m_outputWnd(dynamic_cast<COutputWnd*>(outputWindow)),
m_nWidth(0),
m_nHeight(0)
{

}

CGSH_HleSoftware::~CGSH_HleSoftware()
{
    
}

void CGSH_HleSoftware::InitializeImpl()
{
	m_d3d = Direct3DPtr(Direct3DCreate9(D3D_SDK_VERSION));
	CreateDevice();

	m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	PresentBackbuffer();
}

void CGSH_HleSoftware::ReleaseImpl()
{

}

CGSHandler::FactoryFunction CGSH_HleSoftware::GetFactoryFunction(Framework::Win32::CWindow* pOutputWnd)
{
    return std::bind(&CGSH_HleSoftware::GSHandlerFactory, pOutputWnd);
}

CGSHandler* CGSH_HleSoftware::GSHandlerFactory(Framework::Win32::CWindow* pOutputWnd)
{
    return new CGSH_HleSoftware(pOutputWnd);
}

void CGSH_HleSoftware::CreateDevice()
{
	auto presentParams = CreatePresentParams();
	HRESULT result = S_OK;
	result = m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_outputWnd->m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&presentParams,
		&m_device);
	assert(SUCCEEDED(result));

	OnDeviceReset();

	m_sceneBegun = false;
	BeginScene();
}

D3DPRESENT_PARAMETERS CGSH_HleSoftware::CreatePresentParams()
{
	RECT clientRect = m_outputWnd->GetClientRect();
	unsigned int outputWidth = clientRect.right;
	unsigned int outputHeight = clientRect.bottom;

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_outputWnd->m_hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = outputWidth;
	d3dpp.BackBufferHeight = outputHeight;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	return d3dpp;
}

void CGSH_HleSoftware::OnDeviceReset()
{
	HRESULT result = S_OK;

#ifdef _WIREFRAME
	m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
#endif
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	result = m_device->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, CUSTOMFVF, D3DPOOL_DEFAULT, &m_quadVb, NULL);
	assert(SUCCEEDED(result));
}

void CGSH_HleSoftware::OnDeviceResetting()
{

}


void CGSH_HleSoftware::BeginScene()
{
	if (!m_sceneBegun)
	{
#ifdef _WIREFRAME
		m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
#endif
		HRESULT result = m_device->BeginScene();
		assert(result == S_OK);
		m_sceneBegun = true;
	}
}

void CGSH_HleSoftware::EndScene()
{
	if (m_sceneBegun)
	{
		HRESULT result = m_device->EndScene();
		assert(result == S_OK);
		m_sceneBegun = false;
	}
}

void CGSH_HleSoftware::PresentBackbuffer()
{
	if (!m_device.IsEmpty())
	{
		HRESULT result = S_OK;

		EndScene();
		if (TestDevice())
		{
			result = m_device->Present(NULL, NULL, NULL, NULL);
			assert(SUCCEEDED(result));
		}
		BeginScene();
	}
}

bool CGSH_HleSoftware::TestDevice()
{
	HRESULT coopLevelResult = m_device->TestCooperativeLevel();
	if (FAILED(coopLevelResult))
	{
		if (coopLevelResult == D3DERR_DEVICELOST)
		{
			return false;
		}
		else if (coopLevelResult == D3DERR_DEVICENOTRESET)
		{
			OnDeviceResetting();
			auto presentParams = CreatePresentParams();
			HRESULT result = m_device->Reset(&presentParams);
			if (FAILED(result))
			{
				assert(0);
				return false;
			}
			OnDeviceReset();
		}
		else
		{
			assert(0);
		}
	}

	return true;
}

void CGSH_HleSoftware::FlipImpl()
{
	displayFrameBuffer();
	PresentBackbuffer();
	CGSHandler::FlipImpl();
}

void CGSH_HleSoftware::displayFrameBuffer()
{
	

	SetReadCircuitMatrix(FB_WIDTH_PIX, FB_HEIGHT_PIX);
}

void CGSH_HleSoftware::ProcessImageTransfer()
{
	auto bltBuf = make_convertible<BITBLTBUF>(m_nReg[GS_REG_BITBLTBUF]);
	uint32 transferAddress = bltBuf.GetDstPtr();

	if (m_trxCtx.nDirty)
	{
		auto trxReg = make_convertible<TRXREG>(m_nReg[GS_REG_TRXREG]);
		auto trxPos = make_convertible<TRXPOS>(m_nReg[GS_REG_TRXPOS]);

	}
}

void CGSH_HleSoftware::ProcessClutTransfer(uint32 csa, uint32)
{

}

void CGSH_HleSoftware::ProcessLocalToLocalTransfer()
{
	auto bltBuf = make_convertible<BITBLTBUF>(m_nReg[GS_REG_BITBLTBUF]);
	
}

void CGSH_HleSoftware::ReadFramebuffer(uint32, uint32, void*)
{

}

void CGSH_HleSoftware::SetReadCircuitMatrix(int nWidth, int nHeight)
{
	//Setup projection matrix
	{
		D3DXMATRIX projMatrix;
		D3DXMatrixOrthoLH(&projMatrix, static_cast<FLOAT>(nWidth), static_cast<FLOAT>(nHeight), 1.0f, 0.0f);
		m_device->SetTransform(D3DTS_PROJECTION, &projMatrix);
	}

	//Setup view matrix
	{
		D3DXMATRIX viewMatrix;
		D3DXMatrixLookAtLH(&viewMatrix,
			&D3DXVECTOR3(0.0f, 0.0f, 1.0f),		// eye
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// at (look along -ve z axis)
			&D3DXVECTOR3(0.0f, -1.0f, 0.0f));	// up (+ve y is down)

		m_device->SetTransform(D3DTS_VIEW, &viewMatrix);
	}

	//Setup world matrix to make (0,0) the top left corner rather than the centre
	{
		D3DXMATRIX worldMatrix;
		D3DXMatrixTranslation(&worldMatrix, -static_cast<FLOAT>(nWidth) / 2.0f, -static_cast<FLOAT>(nHeight) / 2.0f, 0);
		m_device->SetTransform(D3DTS_WORLD, &worldMatrix);
	}
}

void CGSH_HleSoftware::TransferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y)
{
	TexturePtr tex;
	int pixw = blockSize * widthInBlocks;
	int pixh = blockSize * heightInBlocks;
	D3DXCreateTexture(m_device, pixw, pixh, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &tex);
	m_device->SetTexture(0, tex);

	DWORD color0 = D3DCOLOR_ARGB(0,0,0,0);
	DWORD color1 = D3DCOLOR_ARGB(0,0,0,0);

	float nU1 = 0;
	float nU2 = 1.0;
	float nV1 = 0.0;
	float nV2 = 1.0;

	float nZ = 0.0;
	float nX1 = 0.0;
	float nY1 = 0.0;
	float nX2 = pixw;
	float nY2 = pixh;

	CUSTOMVERTEX vertices[] =
	{
		{ nX1,	nY2,	nZ,		color0,		nU1,	nV2 },
		{ nX1,	nY1,	nZ,		color0,		nU1,	nV1 },
		{ nX2,	nY2,	nZ,		color1,		nU2,	nV2 },
		{ nX2,	nY1,	nZ,		color1,		nU2,	nV1 },
	};

	HRESULT result;
	uint8* buffer = NULL;
	result = m_quadVb->Lock(0, sizeof(CUSTOMVERTEX) * 4, reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);
	assert(SUCCEEDED(result));
	{
		memcpy(buffer, vertices, sizeof(vertices));
	}
	result = m_quadVb->Unlock();
	assert(SUCCEEDED(result));

	// select which vertex format we are using
	result = m_device->SetFVF(CUSTOMFVF);
	assert(SUCCEEDED(result));

	// select the vertex buffer to display
	result = m_device->SetStreamSource(0, m_quadVb, 0, sizeof(CUSTOMVERTEX));
	assert(SUCCEEDED(result));

	// copy the vertex buffer to the back buffer
	result = m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	assert(SUCCEEDED(result));
}


