#include "BgdaDrawSpriteBlock.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"

#include "Log.h"
#define LOG_NAME	("bgda")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawSpriteBlock::BgdaDrawSpriteBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm, bool hasScissorIn) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
	hasScissor = hasScissorIn;
}

class BgdaDrawSpriteDmaItem : public BgdaDmaItem
{
public:
	CMIPS& context;
	bool isInterlaced;
	uint8* texData;
	uint32 xpos;
	uint32 ypos;
	uint32 vertexRGBA;
	bool hasScissor;
	int scissorX0, scissorY0, scissorX1, scissorY1;

	BgdaDrawSpriteDmaItem(CMIPS& contextIn, uint8* texDataIn, uint32 xposIn, uint32 yposIn, uint32 vertexRGBAIn, bool isInterlacedIn) :
		context(contextIn), texData(texDataIn), xpos(xposIn), ypos(yposIn), vertexRGBA(vertexRGBAIn), isInterlaced(isInterlacedIn)
	{

	}

	void setScissor(int x0, int y0, int x1, int y1)
	{
		hasScissor = true;
		scissorX0 = x0;
		scissorY0 = y0;
		scissorX1 = x1;
		scissorY1 = y1;
	}

	void execute(CGHSHle* gs)
	{
		int16* pTexData16 = (int16*)texData;
		uint16 texWidth = pTexData16[0];
		uint16 texHeight = pTexData16[1];

		// GS packet data starts at [texData+0x10]

		uint32 gsStartPtr = *(uint32*)(texData + 0x10);
		
		CLog::GetInstance().Print(LOG_NAME, "BgdaDrawSpriteBlock(%d, %d, %d, %d, 0x%.8x)\n", xpos, ypos, texWidth, texHeight, vertexRGBA);

		if (hasScissor) {
			gs->setScissorRect(scissorX0, scissorY0, scissorX1, scissorY1);
		}
		gs->drawSprite(xpos, ypos, texWidth, texHeight, vertexRGBA, HleVMUtils::getPointer(context, gsStartPtr), isInterlaced, 0x44);
		if (hasScissor) {
			gs->disableScissor();
		}
	}
};

unsigned int BgdaDrawSpriteBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	uint32 isInterlaced = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xa4d0);

	uint8* pTexData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	uint32 xpos = m_context.m_State.nGPR[CMIPS::A1].nV0;
	uint32 ypos = m_context.m_State.nGPR[CMIPS::A2].nV0;
	uint32 slot = m_context.m_State.nGPR[CMIPS::A3].nV0;

	uint32 vertexRGBA = m_context.m_State.nGPR[CMIPS::T1].nV0;

	BgdaDrawSpriteDmaItem* item = new BgdaDrawSpriteDmaItem(m_context, pTexData, xpos, ypos, vertexRGBA, isInterlaced != 0);
	if (hasScissor) {
		uint32 sc_x0 = m_context.m_State.nGPR[CMIPS::T2].nV0;
		uint32 sc_y0 = m_context.m_State.nGPR[CMIPS::T3].nV0;
		uint32* pSP = (uint32*)HleVMUtils::getOffsetPointer(m_context, CMIPS::SP, 0);
		uint32 sc_x1 = pSP[0];
		uint32 sc_y1 = pSP[2];
		item->setScissor(sc_x0, sc_y0, sc_x1, sc_y1);
	}
	
	bgdaContext.dmaQueue.prependItem(slot, item);

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

