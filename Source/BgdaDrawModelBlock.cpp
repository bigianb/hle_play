#include "BgdaDrawModelBlock.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"

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
	

	BgdaDrawModelBlockItem(CMIPS& contextIn) :
		context(contextIn)
	{

	}


	void execute(CGHSHle* gs)
	{
		
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
	uint8* pVifData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	uint8* pTexData = HleVMUtils::getOffsetPointer(m_context, CMIPS::A1, 0);
	uint32 slot = m_context.m_State.nGPR[CMIPS::A2].nV0;

	uint32 arg3 = m_context.m_State.nGPR[CMIPS::A3].nV0;

	float * pMatrix= (float* )HleVMUtils::getOffsetPointer(m_context, CMIPS::T0, 0);

	uint8* pAnimData = HleVMUtils::getOffsetPointer(m_context, CMIPS::T1, 0);
	uint32 showSubmeshMask = m_context.m_State.nGPR[CMIPS::T2].nV0;
	uint32 arg7 = m_context.m_State.nGPR[CMIPS::T3].nV0;

	CLog::GetInstance().Print(LOG_NAME, "transform\n");
	for (int row = 0; row < 4; ++row) {
		CLog::GetInstance().Print(LOG_NAME, "%f, %f, %f, %f\n", pMatrix[row], pMatrix[row + 4], pMatrix[row+8], pMatrix[row+12]);
	}

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

