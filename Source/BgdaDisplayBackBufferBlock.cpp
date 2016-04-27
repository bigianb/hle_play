#include "BgdaDisplayBackBufferBlock.h"
#include "bgdaContext.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"
#include "PS2VM.h"

#include "Log.h"
#define LOG_NAME	("bgda")

BgdaDisplayBackBufferBlock::BgdaDisplayBackBufferBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) :
	CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

BgdaDisplayBackBufferBlock::~BgdaDisplayBackBufferBlock()
{
}

unsigned int BgdaDisplayBackBufferBlock::Execute()
{
	CLog::GetInstance().Print(LOG_NAME, "BgdaDisplayBackBUfferBlock\n");

	int cycles = CBasicBlock::Execute();

	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());
	bgdaContext.dmaQueue.flush(gs);

	gs->displayBackBufferAndClear();

	return cycles;
}
