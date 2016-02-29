#include "BgdaSetTextColourBlock.h"
#include "bgdaContext.h"

#include "Log.h"
#define LOG_NAME	("bgda")

BgdaSetTextColourBlock::BgdaSetTextColourBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) :
	CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

BgdaSetTextColourBlock::~BgdaSetTextColourBlock()
{
}

unsigned int BgdaSetTextColourBlock::Execute()
{
	bgdaContext.currentTextColour = m_context.m_State.nGPR[CMIPS::A0].nV0;

	CLog::GetInstance().Print(LOG_NAME, "BgdaSetTextColourBlock\n");
	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}
