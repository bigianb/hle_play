#include "BgdaDrawColourSpriteBlock.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawColourSpriteBlock::BgdaDrawColourSpriteBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm)
{
}

unsigned int BgdaDrawColourSpriteBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	uint32 isInterlaced = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xa4d0);

	uint32 xpos = m_context.m_State.nGPR[CMIPS::A0].nV0;
	uint32 ypos = m_context.m_State.nGPR[CMIPS::A1].nV0;
	
	uint32 xpos2 = m_context.m_State.nGPR[CMIPS::A2].nV0;
	uint32 ypos2 = m_context.m_State.nGPR[CMIPS::A3].nV0;

	uint32 vertexRGBA = m_context.m_State.nGPR[CMIPS::T1].nV0;
	
	gs->DrawSprite(xpos, ypos, xpos2-xpos, ypos2-ypos, vertexRGBA, nullptr, isInterlaced != 0, 0x44);

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 1000;
}

