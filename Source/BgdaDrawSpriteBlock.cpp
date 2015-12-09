#include "BgdaDrawSpriteBlock.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawSpriteBlock::BgdaDrawSpriteBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm)
{
}

unsigned int BgdaDrawSpriteBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	uint32 isInterlaced = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xa4d0);

	uint8* pTexData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	uint32 xpos = m_context.m_State.nGPR[CMIPS::A1].nV0;
	uint32 ypos = m_context.m_State.nGPR[CMIPS::A2].nV0;
	
//	uint32 arg2 = m_context.m_State.nGPR[CMIPS::A3].nV0;
//	uint32 arg3 = m_context.m_State.nGPR[CMIPS::T0].nV0;

	uint32 vertexRGBA = m_context.m_State.nGPR[CMIPS::T1].nV0;

	int16* pTexData16 = (int16*)pTexData;
	uint16 texWidth = pTexData16[0];
	uint16 texHeight = pTexData16[1];

	// GS packet data starts at [pTexData+0x10]

	uint32 gsStartPtr = *(uint32*)(pTexData + 0x10);
	
	gs->DrawSprite(xpos, ypos, texWidth, texHeight, vertexRGBA, HleVMUtils::getPointer(m_context, gsStartPtr), isInterlaced != 0);

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 1000;
}

