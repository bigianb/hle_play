#include "BgdaDrawObjectBlock.h"
#include "BgdaMatrixRoutines.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"
#include "snowlib/TexDecoder.h"
#include "snowlib/VifDecoder.h"
#include "Log.h"
#include <d3dx9math.h>
#define LOG_NAME	("bgda_model")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawObjectBlock::BgdaDrawObjectBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

class BgdaDrawObjectBlockItem : public BgdaDmaItem
{
public:
	CMIPS& context;
	uint8* texData;
	std::vector<Mesh*>* meshList;
	float xform[16];

	BgdaDrawObjectBlockItem(CMIPS& contextIn, uint8* texDataIn, std::vector<Mesh*>* meshListIn, float* xformIn) :
		context(contextIn), texData(texDataIn), meshList(meshListIn)
	{
		memcpy(xform, xformIn, 4 * 16);
	}

	~BgdaDrawObjectBlockItem()
	{
		if (meshList != nullptr) {
			for (Mesh* p : *meshList) {
				delete p;
			}
			delete meshList;
		}
	}


	void execute(CGHSHle* gs)
	{
		int16* pTexData16 = (int16*)texData;
		uint16 texWidth = pTexData16[0];
		uint16 texHeight = pTexData16[1];
		uint32 gsStartPtr = *(uint32*)(texData + 0x10);

		if (texWidth > 0xffff || texHeight > 0xffff || gsStartPtr > 0x0fffffff) {
			// This is probably due to the game loading a new level before clearing the DMA.
			CLog::GetInstance().Print(LOG_NAME, "Found corrupt texture data");
			return;
		}
		gs->drawModel(texWidth, texHeight, HleVMUtils::getPointer(context, gsStartPtr), meshList, xform);

	}
};

unsigned int BgdaDrawObjectBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	/*
	a0 = 
	a1 = 
	a2 = 
	*/
	uint32 arg0 = m_context.m_State.nGPR[CMIPS::A0].nV0;
	uint8* pArg1 = HleVMUtils::getOffsetPointer(m_context, CMIPS::A1, 0);
	uint8* arg2 = HleVMUtils::getOffsetPointer(m_context, CMIPS::A2, 0);
	
	uint32 ps2PtrModelName = pArg1[0];
	uint8* pModelName = HleVMUtils::getPointer(m_context, ps2PtrModelName);



	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

