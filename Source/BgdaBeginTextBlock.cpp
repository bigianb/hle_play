#include "BgdaBeginTextBlock.h"
#include "bgdaContext.h"
#include "HleVMUtils.h"

#include "Log.h"
#define LOG_NAME	("bgda")

BgdaBeginTextBlock::BgdaBeginTextBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) :
	CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

BgdaBeginTextBlock::~BgdaBeginTextBlock()
{
}

unsigned int BgdaBeginTextBlock::Execute()
{
	bgdaContext.currentTextColour = m_context.m_State.nGPR[CMIPS::A3].nV0;
	uint32 fontAddr = m_context.m_State.nGPR[CMIPS::A0].nV0;

	CLog::GetInstance().Print(LOG_NAME, "BgdaBeginTextBlock\n");

	uint32 preStoredFontAddr = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xbfc4);
	int cycles = CBasicBlock::Execute();

	uint32 storedFontAddr = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xbfc4);

	//m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return cycles;
}
