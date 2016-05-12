#include "BgdaDebugBlock.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"

#include "Log.h"
#define LOG_NAME	("bgda_debug")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDebugBlock::BgdaDebugBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{

}

unsigned int BgdaDebugBlock::Execute()
{
	// address is 185a68
	// $be04(gp)  is the start of the DMA
	// $0050(sp) is the end of the DMA

	// address is 00186794
	// $be04(gp)  is the start of the DMA
	// $0050(sp) is the end of the DMA
	// $006c(t8) is texture data

	uint32 pc = m_context.m_State.nPC;

	uint8* dmaStart = HleVMUtils::getOffsetPointer(m_context, CMIPS::GP, 0xbe04);
	uint8* dmaEnd = HleVMUtils::getOffsetPointer(m_context, CMIPS::SP, 0x50);
	uint8* dmaLenPtr = HleVMUtils::getOffsetPointer(m_context, CMIPS::SP, 0x54);

    int cycles = CBasicBlock::Execute();
    return cycles;
}

