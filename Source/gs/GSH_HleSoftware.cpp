#include "GSH_HleSoftware.h"
#include "PtrMacro.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

using namespace Framework;
using namespace std;
using namespace std::tr1;

CGSH_HleSoftware::CGSH_HleSoftware(Win32::CWindow* outputWindow) :
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

CGSHandler::FactoryFunction CGSH_HleSoftware::GetFactoryFunction(Win32::CWindow* pOutputWnd)
{
    return bind(&CGSH_HleSoftware::GSHandlerFactory, pOutputWnd);
}

CGSHandler* CGSH_HleSoftware::GSHandlerFactory(Win32::CWindow* pOutputWnd)
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
	PresentBackbuffer();
	CGSHandler::FlipImpl();
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

