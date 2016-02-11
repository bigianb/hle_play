#include "BgdaBeginTextBlock.h"
#include "bgdaContext.h"

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

	CLog::GetInstance().Print(LOG_NAME, "BgdaBeginTextBlock\n");


	return CBasicBlock::Execute();

	//m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	//return 10;
}
