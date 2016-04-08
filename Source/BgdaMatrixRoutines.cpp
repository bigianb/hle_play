#include "BgdaMatrixRoutines.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"

#include "Log.h"
#define LOG_NAME	("bgda_model")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaSetupProjMatrixBlock::BgdaSetupProjMatrixBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}


unsigned int BgdaSetupProjMatrixBlock::Execute()
{
	/*
	a0 = pDest
	we're going to use our own projection matrix, so squash the built in one to
	the identity matrix.
	*/
	float* pDest = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			*pDest++ = row == col ? 1.0 : 0.0;
		}
	}

	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

