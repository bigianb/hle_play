#include "BgdaKickoffDmaBlock.h"
#include "bgdaContext.h"
#include "gs/GSH_Hle.h"
#include "PS2VM.h"

#include "Log.h"
#define LOG_NAME	("bgda")

BgdaKickoffDmaBlock::BgdaKickoffDmaBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) :
	CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

BgdaKickoffDmaBlock::~BgdaKickoffDmaBlock()
{
}

unsigned int BgdaKickoffDmaBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());
	bgdaContext.dmaQueue.flush(gs);
	return CBasicBlock::Execute();
}
