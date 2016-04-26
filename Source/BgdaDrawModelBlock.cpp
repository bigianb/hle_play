#include "BgdaDrawModelBlock.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"
#include "snowlib/TexDecoder.h"
#include "snowlib/VifDecoder.h"
#include "Log.h"
#define LOG_NAME	("bgda_model")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawModelBlock::BgdaDrawModelBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

class BgdaDrawModelBlockItem : public BgdaDmaItem
{
public:
	CMIPS& context;
	uint8* texData;
	std::vector<Mesh*>* meshList;
	float xform[16];

	BgdaDrawModelBlockItem(CMIPS& contextIn, uint8* texDataIn, std::vector<Mesh*>* meshListIn, float* xformIn) :
		context(contextIn), texData(texDataIn), meshList(meshListIn)
	{
		memcpy(xform, xformIn, 4 * 16);
	}

	~BgdaDrawModelBlockItem()
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

unsigned int BgdaDrawModelBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	/*
	a0 = vif
	a1 = tex
	a2 = arg2 = dma slot
	a3 = arg3
	t0 = arg4 - 4x4 matrix
	t1 = arg5 - anim data
	t2 = arg6 - bit mask of meshes to draw
	t3 = arg7
	*/
	uint32 vifPs2Addr = m_context.m_State.nGPR[CMIPS::A0].nV0;
	uint8* pVifData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	uint8* pTexData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A1, 0);
	uint32 slot = m_context.m_State.nGPR[CMIPS::A2].nV0;

	uint32 arg3 = m_context.m_State.nGPR[CMIPS::A3].nV0;

	float * pMatrix= (float* )HleVMUtils::getOffsetPointer(m_context, CMIPS::T0, 0);

	uint8* pAnimData = HleVMUtils::getOffsetPointer(m_context, CMIPS::T1, 0);
	uint32 showSubmeshMask = m_context.m_State.nGPR[CMIPS::T2].nV0;
	uint32 arg7 = m_context.m_State.nGPR[CMIPS::T3].nV0;
	/*
	CLog::GetInstance().Print(LOG_NAME, "transform\n");
	for (int row = 0; row < 4; ++row) {
		CLog::GetInstance().Print(LOG_NAME, "%f, %f, %f, %f\n", pMatrix[row], pMatrix[row + 4], pMatrix[row+8], pMatrix[row+12]);
	}
	*/
	VifDecoder vifDecoder;
	std::vector<Mesh*>* meshList = vifDecoder.decode(pVifData, vifPs2Addr);

	BgdaDrawModelBlockItem* item = new BgdaDrawModelBlockItem(m_context, pTexData, meshList, pMatrix);
	bgdaContext.dmaQueue.prependItem(slot, item);

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

