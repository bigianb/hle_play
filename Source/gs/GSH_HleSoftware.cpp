#include "GSH_HleSoftware.h"
#include "PtrMacro.h"
#include <d3dx9math.h>

#include "snowlib/TexDecoder.h"
#include "snowlib/Texture.h"
#include "snowlib/Mesh.h"

#include "../ui_win32/hle_resource.h"
#include "Log.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// Enable debugging in release mode
#pragma optimize( "", off )

#define LOG_NAME	("gs_bgda")


struct CUSTOMVERTEX
{
	float x, y, z;
	DWORD color;
	float u, v;
};

#define FB_WIDTH_PIX 640
#define FB_HEIGHT_PIX 512

CGSH_HleSoftware::CGSH_HleSoftware(Framework::Win32::CWindow* outputWindow) :
	m_outputWnd(dynamic_cast<COutputWnd*>(outputWindow))
{
	currentTextureSourcePointer = nullptr;
}

CGSH_HleSoftware::~CGSH_HleSoftware()
{
    
}

void CGSH_HleSoftware::InitializeImpl()
{
	m_d3d = Direct3DPtr(Direct3DCreate9(D3D_SDK_VERSION));
	CreateDevice();

	m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
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

	m_mainFx = CreateEffectFromResource(MAKEINTRESOURCE(IDR_HLEMAIN_SHADER));

	OnDeviceReset();

	m_sceneBegun = false;
	BeginScene();
}

D3DPRESENT_PARAMETERS CGSH_HleSoftware::CreatePresentParams()
{
	RECT clientRect = m_outputWnd->GetClientRect();
	unsigned int outputWidth = FB_WIDTH_PIX; // clientRect.right;
	unsigned int outputHeight = FB_HEIGHT_PIX;// clientRect.bottom;

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

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, D3DZB_TRUE);

	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	result = m_device->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_quadVb, nullptr);
	assert(SUCCEEDED(result));

	{
		std::vector<D3DVERTEXELEMENT9> vertexElements;

		{
			D3DVERTEXELEMENT9 element = {};
			element.Offset = offsetof(CUSTOMVERTEX, x);
			element.Type = D3DDECLTYPE_FLOAT3;
			element.Usage = D3DDECLUSAGE_POSITION;
			vertexElements.push_back(element);
		}

		{
			D3DVERTEXELEMENT9 element = {};
			element.Offset = offsetof(CUSTOMVERTEX, color);
			element.Type = D3DDECLTYPE_D3DCOLOR;
			element.Usage = D3DDECLUSAGE_COLOR;
			vertexElements.push_back(element);
		}

		{
			D3DVERTEXELEMENT9 element = {};
			element.Offset = offsetof(CUSTOMVERTEX, u);
			element.Type = D3DDECLTYPE_FLOAT2;
			element.Usage = D3DDECLUSAGE_TEXCOORD;
			vertexElements.push_back(element);
		}

		{
			D3DVERTEXELEMENT9 element = D3DDECL_END();
			vertexElements.push_back(element);
		}

		result = m_device->CreateVertexDeclaration(vertexElements.data(), &m_quadVertexDecl);
		assert(SUCCEEDED(result));
	}
}

void CGSH_HleSoftware::OnDeviceResetting()
{

}


void CGSH_HleSoftware::BeginScene()
{
	if (!m_sceneBegun)
	{
		CLog::GetInstance().Print(LOG_NAME, "\n---------------------\n\nBeginScene()\n");
		HRESULT result = m_device->BeginScene();
		assert(result == S_OK);
		m_sceneBegun = true;
	}
}

void CGSH_HleSoftware::EndScene()
{
	if (m_sceneBegun)
	{
		CLog::GetInstance().Print(LOG_NAME, "EndScene()\n");
		HRESULT result = m_device->EndScene();
		assert(result == S_OK);
		m_sceneBegun = false;
	}
}

void CGSH_HleSoftware::PresentBackbuffer()
{
	if (!m_device.IsEmpty())
	{
		CLog::GetInstance().Print(LOG_NAME, "PresentBackbuffer()\n");
		HRESULT result = S_OK;

		EndScene();
		if (TestDevice())
		{
			result = m_device->Present(NULL, NULL, NULL, NULL);
			assert(SUCCEEDED(result));
		}
//		m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

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
	CLog::GetInstance().Print(LOG_NAME, "FlipImpl()\n");
	displayFrameBuffer();
	//PresentBackbuffer();

	CGSHandler::FlipImpl();
}

void CGSH_HleSoftware::displayBackBufferAndClear()
{
	PresentBackbuffer();
	m_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0x0, 0x0, 0x0), 1.0f, 0);
}

void CGSH_HleSoftware::displayFrameBuffer()
{
	SetWorldMatrix(FB_WIDTH_PIX, FB_HEIGHT_PIX);
}


void CGSH_HleSoftware::ProcessHostToLocalTransfer()
{

}

void CGSH_HleSoftware::ProcessLocalToHostTransfer()
{

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

void CGSH_HleSoftware::SetWorldMatrix(int nWidth, int nHeight)
{

	//Setup projection matrix
	
	D3DXMATRIX projMatrix;
	D3DXMatrixOrthoLH(&projMatrix, static_cast<FLOAT>(nWidth), static_cast<FLOAT>(nHeight), 1.0f, -1000.0f);

	//Setup view matrix
	
	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix,
		&D3DXVECTOR3(0.0f, 0.0f, 1.0f),		// eye
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// at (look along -ve z axis)
		&D3DXVECTOR3(0.0f, -1.0f, 0.0f));	// up (+ve y is down)

	//Setup world matrix to make (0,0) the top left corner rather than the centre
	
	D3DXMATRIX worldMatrix;
	D3DXMatrixTranslation(&worldMatrix, -static_cast<FLOAT>(nWidth) / 2.0f, -static_cast<FLOAT>(nHeight) / 2.0f, 0);
	
	m_worldViewMatrix = worldMatrix * viewMatrix * projMatrix;
}

void CGSH_HleSoftware::setScissorRect(uint32 scissorX0, uint32 scissorY0, uint32 scissorX1, uint32 scissorY1)
{
	RECT scissorRect = {};
	scissorRect.left = scissorX0;
	scissorRect.top = scissorY0;
	scissorRect.right = scissorX1;
	scissorRect.bottom = scissorY1;
	m_device->SetScissorRect(&scissorRect);
	m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
}

void CGSH_HleSoftware::disableScissor()
{
	m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

void CGSH_HleSoftware::transferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y)
{
	CLog::GetInstance().Print(LOG_NAME, "transferBlockedImage()\n");
	HRESULT result;

	setAlphaBlendFunction(0);

	int pixw = blockSize * widthInBlocks;
	int pixh = blockSize * heightInBlocks;
	if (blockedTex.IsEmpty()) {
		D3DXMATRIX textureMatrix;
		D3DXMatrixIdentity(&textureMatrix);
		D3DXMatrixScaling(&textureMatrix, 1, 1, 1);
		m_device->SetTransform(D3DTS_TEXTURE0, &textureMatrix);

		result = m_device->CreateTexture(pixw, pixh, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &blockedTex, NULL);
	}
	D3DLOCKED_RECT rect;
	result = blockedTex->LockRect(0, &rect, NULL, 0);

	uint32* pDst = reinterpret_cast<uint32*>(rect.pBits);
	uint32* pSrc = pRGBA;
	unsigned int nDstPitch = rect.Pitch / 4;

	for (int i = 0; i < widthInBlocks; i++)
	{
		for (int j = 0; j < heightInBlocks; j++)
		{
			uint32* blockDestPtr = pDst + nDstPitch * j * blockSize + i * blockSize;
			for (int blocky = 0; blocky < blockSize; ++blocky) {
				for (int blockx = 0; blockx < blockSize; ++blockx) {
					uint32 abgr = *pSrc++;
					uint32 argb = 0xFF000000 | (abgr & 0x0000FF00) | ((abgr & 0xFF) << 16) | ((abgr >> 16) & 0xFF);
					blockDestPtr[blockx] = argb;
				}
				blockDestPtr += nDstPitch;
			}
		}
	}

	result = blockedTex->UnlockRect(0);

	DWORD color = D3DCOLOR_ARGB(0x80, 0x80, 0x80, 0x80);

	float nU1 = 0.0;
	float nU2 = 1.0;
	float nV1 = 0.0;
	float nV2 = 1.0;

	float nZ = 0.0;
	float nX1 = 0.0;
	float nY1 = 0.0;
	float nX2 = (float)pixw;
	float nY2 = (float)pixh;

	CUSTOMVERTEX vertices[] =
	{
		{ nX1,	nY2,	nZ,		color,		nU1,	nV2 },
		{ nX1,	nY1,	nZ,		color,		nU1,	nV1 },
		{ nX2,	nY2,	nZ,		color,		nU2,	nV2 },
		{ nX2,	nY1,	nZ,		color,		nU2,	nV1 },
	};

	uint8* buffer = NULL;
	result = m_quadVb->Lock(0, sizeof(CUSTOMVERTEX) * 4, reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);
	assert(SUCCEEDED(result));
	{
		memcpy(buffer, vertices, sizeof(vertices));
	}
	result = m_quadVb->Unlock();
	assert(SUCCEEDED(result));

	// select which vertex format we are using
	result = m_device->SetVertexDeclaration(m_quadVertexDecl);
	assert(SUCCEEDED(result));

	// select the vertex buffer to display
	result = m_device->SetStreamSource(0, m_quadVb, 0, sizeof(CUSTOMVERTEX));
	assert(SUCCEEDED(result));

	D3DXHANDLE isModelProjParameter = m_mainFx->GetParameterByName(NULL, "g_modelProj");
	m_mainFx->SetBool(isModelProjParameter, FALSE);

	D3DXHANDLE useTextureParameter = m_mainFx->GetParameterByName(NULL, "g_useTexture");
	m_mainFx->SetBool(useTextureParameter, TRUE);

	D3DXHANDLE textureParameter = m_mainFx->GetParameterByName(NULL, "g_MeshTexture");
	m_mainFx->SetTexture(textureParameter, blockedTex);

	D3DXHANDLE worldMatrixParameter = m_mainFx->GetParameterByName(NULL, "g_WorldViewProj");
	m_mainFx->SetMatrix(worldMatrixParameter, &m_worldViewMatrix);

	m_mainFx->CommitChanges();

	UINT passCount = 0;
	m_mainFx->Begin(&passCount, D3DXFX_DONOTSAVESTATE);
	for (unsigned int i = 0; i < passCount; i++)
	{
		m_mainFx->BeginPass(i);

		m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		m_mainFx->EndPass();
	}
	m_mainFx->End();
}

Texture* getBGDATexture(int width, int height, uint8* texGsPacketData)
{
	TexDecoder decoder;
	Texture* tex = decoder.decode(width, height, texGsPacketData, 0);
	return tex;
}


void CGSH_HleSoftware::setTexture32(unsigned char* data, int dataLength, int width, int height, bool interlaced)
{
	CLog::GetInstance().Print(LOG_NAME, "setTexture32(width=%d, height=%d)\n", width, height);
	if (data == nullptr) {
		currentTextureSourcePointer = nullptr;
		return;
	}
	if (data == currentTextureSourcePointer && width == currentTextureWidth && height == currentTextureHeight) {
		CLog::GetInstance().Print(LOG_NAME, "    found in cache\n");
		return;
	}
	currentTextureSourcePointer = data;
	currentTextureWidth = width;
	currentTextureHeight = height;

	D3DXMATRIX textureMatrix;
	D3DXMatrixIdentity(&textureMatrix);
	D3DXMatrixScaling(&textureMatrix, 1, 1, 1);
	m_device->SetTransform(D3DTS_TEXTURE0, &textureMatrix);

	currentTexture.Reset();
	HRESULT result = m_device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &currentTexture, NULL);

	D3DLOCKED_RECT rect;
	result = currentTexture->LockRect(0, &rect, NULL, 0);
	uint8* pDst = (uint8*)rect.pBits;

	int mid = height / 2;
	for (int y = 0; y < height; ++y) {
		int ysrc = y;
		if (interlaced) {
			if ((y & 0x01) == 0x01) {
				ysrc = mid + y / 2;
			}
			else {
				ysrc = y / 2;
			}
		}
		uint8* p = data + ysrc * width * 4;
		uint8* pDRow = pDst;
		for (int x = 0; x < width; ++x) {
			pDRow[0] = p[2];
			pDRow[1] = p[1];
			pDRow[2] = p[0];
			pDRow[3] = p[3];
			p += 4;
			pDRow += 4;
		}
		pDst += rect.Pitch;
	}

	result = currentTexture->UnlockRect(0);
}

void CGSH_HleSoftware::drawSprite(int xpos, int ypos, int width, int height, uint32 vertexRGBA, uint8* texGsPacketData, bool interlaced, uint64 alphaReg)
{
	CLog::GetInstance().Print(LOG_NAME, "drawSprite(x=%d, y=%d, w=%d, h=%d, alphareg=0x%x)\n", xpos, ypos, width, height, alphaReg);
	HRESULT result;
	
	setAlphaBlendFunction(alphaReg);
	
	if (texGsPacketData != nullptr) {
		if (texGsPacketData != currentTextureSourcePointer){
			Texture* rawTexture = getBGDATexture(width, height, texGsPacketData);
			setTexture32(rawTexture->data, rawTexture->dataLength, rawTexture->widthPixels, height, interlaced);
			currentTextureSourcePointer = texGsPacketData;
			delete rawTexture; rawTexture = nullptr;
		}
	}
	drawSprite(xpos, ypos, 0, 0, width, height, vertexRGBA, texGsPacketData != nullptr);
}

void CGSH_HleSoftware::drawSprite(int xpos, int ypos, int u0, int v0, int width, int height, uint32 vertexRGBA, bool useTexture)
{
	CLog::GetInstance().Print(LOG_NAME, "drawSprite(vertexRGBA=0x%08x, useTexture=%s)\n", vertexRGBA, useTexture?"true":"false");
	uint32 color = (vertexRGBA & 0xFF00FF00) | ((vertexRGBA & 0xFF) << 16) | ((vertexRGBA >> 16) & 0xFF);

	float nU1 = 0.0;
	float nU2 = 1.0;
	float nV1 = 0.0;
	float nV2 = 1.0;

	if (useTexture)
	{
		nU1 = u0 / (float)currentTextureWidth;
		nV1 = v0 / (float)currentTextureHeight;
		nU2 = (u0 + width) / (float)currentTextureWidth;
		nV2 = (v0 + height) / (float)currentTextureHeight;
	}

	float nZ = 0.0;
	float nX1 = xpos;
	float nY1 = ypos;
	float nX2 = xpos + width;
	float nY2 = ypos + height;

	CUSTOMVERTEX vertices[] =
	{
		{ nX1,	nY2,	nZ,		color,		nU1,	nV2 },
		{ nX1,	nY1,	nZ,		color,		nU1,	nV1 },
		{ nX2,	nY2,	nZ,		color,		nU2,	nV2 },
		{ nX2,	nY1,	nZ,		color,		nU2,	nV1 },
	};

	uint8* buffer = NULL;
	HRESULT result = m_quadVb->Lock(0, sizeof(CUSTOMVERTEX) * 4, reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);
	assert(SUCCEEDED(result));
	{
		memcpy(buffer, vertices, sizeof(vertices));
	}
	result = m_quadVb->Unlock();
	assert(SUCCEEDED(result));

	// select which vertex format we are using
	result = m_device->SetVertexDeclaration(m_quadVertexDecl);
	assert(SUCCEEDED(result));

	// select the vertex buffer to display
	result = m_device->SetStreamSource(0, m_quadVb, 0, sizeof(CUSTOMVERTEX));
	assert(SUCCEEDED(result));

	D3DXHANDLE isModelProjParameter = m_mainFx->GetParameterByName(NULL, "g_modelProj");
	m_mainFx->SetBool(isModelProjParameter, FALSE);

	D3DXHANDLE useTextureParameter = m_mainFx->GetParameterByName(NULL, "g_useTexture");
	if (useTexture) {
		m_mainFx->SetBool(useTextureParameter, TRUE);
		D3DXHANDLE textureParameter = m_mainFx->GetParameterByName(NULL, "g_MeshTexture");
		m_mainFx->SetTexture(textureParameter, currentTexture);
		
	} else {
		m_mainFx->SetBool(useTextureParameter, FALSE);
	}
	D3DXHANDLE worldMatrixParameter = m_mainFx->GetParameterByName(NULL, "g_WorldViewProj");
	m_mainFx->SetMatrix(worldMatrixParameter, &m_worldViewMatrix);

	m_mainFx->CommitChanges();

	UINT passCount = 0;
	m_mainFx->Begin(&passCount, D3DXFX_DONOTSAVESTATE);
	for (unsigned int i = 0; i < passCount; i++)
	{
		m_mainFx->BeginPass(i);

		m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		m_mainFx->EndPass();
	}
	m_mainFx->End();

}

void CGSH_HleSoftware::drawModel(int texWidth, int texHeight, uint8* texGsPacketData, std::vector<Mesh*>* meshList, float* xform)
{
	if (texGsPacketData != nullptr) {
		if (texGsPacketData != currentTextureSourcePointer) {
			Texture* rawTexture = getBGDATexture(texWidth, texHeight, texGsPacketData);
			setTexture32(rawTexture->data, rawTexture->dataLength, rawTexture->widthPixels, texHeight, false);
			currentTextureSourcePointer = texGsPacketData;
			delete rawTexture; rawTexture = nullptr;
		}
	}

	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	disableScissor();

	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//Setup view matrix
	D3DXMATRIX viewMatrix(xform);
	D3DXMATRIX worldViewMatrix = viewMatrix;

	for (Mesh* mesh : *meshList) {
		if (mesh->numVertices == 0) {
			continue;
		}
		int numTris = mesh->triangleIndices.size() / 3;
		VertexBufferPtr vertexBuf;
		HRESULT result = m_device->CreateVertexBuffer(numTris * 3 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vertexBuf, nullptr);

		uint8* buffer = NULL;
		result = vertexBuf->Lock(0, numTris * 3 * sizeof(CUSTOMVERTEX), reinterpret_cast<void**>(&buffer), D3DLOCK_DISCARD);
		assert(SUCCEEDED(result));
		
		CUSTOMVERTEX* vertex = (CUSTOMVERTEX*)buffer;
		for (int idx : mesh->triangleIndices) {
			FloatVector* position = mesh->positions + idx;
			FloatPoint* uvCoord = mesh->uvCoords + idx;
			vertex->x = position->x;
			vertex->y = position->y;
			vertex->z = position->z;
			vertex->color = 0x80808080;
			vertex->u = uvCoord->x / (float)texWidth;
			vertex->v = uvCoord->y / (float)texHeight;

			++vertex;
		}
		result = vertexBuf->Unlock();
		assert(SUCCEEDED(result));
		// select which vertex format we are using
		result = m_device->SetVertexDeclaration(m_quadVertexDecl);
		assert(SUCCEEDED(result));

		// select the vertex buffer to display
		result = m_device->SetStreamSource(0, vertexBuf, 0, sizeof(CUSTOMVERTEX));
		assert(SUCCEEDED(result));

		D3DXHANDLE useTextureParameter = m_mainFx->GetParameterByName(NULL, "g_useTexture");
		m_mainFx->SetBool(useTextureParameter, TRUE);
		
		D3DXHANDLE isModelProjParameter = m_mainFx->GetParameterByName(NULL, "g_modelProj");
		m_mainFx->SetBool(isModelProjParameter, TRUE);
		
		D3DXHANDLE textureParameter = m_mainFx->GetParameterByName(NULL, "g_MeshTexture");
		m_mainFx->SetTexture(textureParameter, currentTexture);

		D3DXHANDLE worldMatrixParameter = m_mainFx->GetParameterByName(NULL, "g_WorldViewProj");
		m_mainFx->SetMatrix(worldMatrixParameter, &worldViewMatrix);

		m_mainFx->CommitChanges();

		UINT passCount = 0;
		m_mainFx->Begin(&passCount, D3DXFX_DONOTSAVESTATE);
		for (unsigned int pass = 0; pass < passCount; pass++)
		{
			m_mainFx->BeginPass(pass);

			m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numTris);

			m_mainFx->EndPass();
		}
		m_mainFx->End();
	}
}

void CGSH_HleSoftware::setAlphaBlendFunction(uint64 alphaReg)
{
	CLog::GetInstance().Print(LOG_NAME, "setAlphaBlendFunction(0x%x)\n", alphaReg);
	auto alpha = make_convertible<ALPHA>(alphaReg);

	// Cv = (A - B) * C >> 7 + D

	m_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);

	if ((alpha.nA == 0) && (alpha.nB == 1) && (alpha.nC == 0) && (alpha.nD == 1))
	{
		// 0x44 d = cd (frame buffer), c=as, b=Cd, a = Cs
		// Cv = (Cs - Cd) * As >> 7 + Cd = Cs*As + Cd*(1 - As)
		// 
		m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else
	{
		//Default blending
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	}

	// hack
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_device->SetRenderState(D3DRS_ALPHAREF, 1);
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

}

CGSH_HleSoftware::EffectPtr CGSH_HleSoftware::CreateEffectFromResource(const TCHAR* resourceName)
{
	HRESULT result = S_OK;

	HRSRC shaderResourceInfo = FindResource(GetModuleHandle(nullptr), resourceName, _T("TEXTFILE"));
	assert(shaderResourceInfo != nullptr);

	HGLOBAL shaderResourceHandle = LoadResource(GetModuleHandle(nullptr), shaderResourceInfo);
	DWORD shaderResourceSize = SizeofResource(GetModuleHandle(nullptr), shaderResourceInfo);

	const char* shaderData = reinterpret_cast<const char*>(LockResource(shaderResourceHandle));

	EffectPtr effect;
	Framework::Win32::CComPtr<ID3DXBuffer> errors;
	result = D3DXCreateEffect(m_device, shaderData, shaderResourceSize, nullptr, nullptr, 0, nullptr, &effect, &errors);
	if (!errors.IsEmpty())
	{
		std::string errorText(reinterpret_cast<const char*>(errors->GetBufferPointer()), reinterpret_cast<const char*>(errors->GetBufferPointer()) + errors->GetBufferSize());
		OutputDebugStringA("Failed to compile shader:\r\n");
		OutputDebugStringA(errorText.c_str());
	}
	assert(SUCCEEDED(result));

	UnlockResource(shaderResourceHandle);

	return effect;
}
