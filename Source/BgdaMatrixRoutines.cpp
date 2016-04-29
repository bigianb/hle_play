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

// ------------------------------------

BgdaMatrixMulBlock::BgdaMatrixMulBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm), bgdaContext(bgdaContextIn)
{
}

void BgdaMatrixMulBlock::mul3x4by4x4(float* out, float* p3x4, float* p4x4)
{
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			float a = p4x4[row] * p3x4[col*3];
			float b = p4x4[row + 4] * p3x4[col*3+1];
			float c = p4x4[row + 8] * p3x4[col*3+2];
			float d = col == 3 ? p4x4[row + 12] : 0;
			out[row + col * 4] = a + b + c + d;
		}
	}
}

unsigned int BgdaMatrixMulBlock::Execute()
{
	uint32 A0_in = m_context.m_State.nGPR[CMIPS::A0].nV0;
	//float* pDest = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::A0, 0);
	//float* p3x4 = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::A1, 0);
	//float* p4x4 = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::A2, 0);

	float* pDest = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::S5, 0);
	float* p3x4 = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::S2, -4);
	float* p4x4 = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::T9, -0x20);

	float out[16];
	mul3x4by4x4(out, p3x4, p4x4);

	int cycles = CBasicBlock::Execute();

	uint32 V0_out = m_context.m_State.nGPR[CMIPS::V0].nV0;
	float* pRet = (float*)HleVMUtils::getOffsetPointer(m_context, CMIPS::V0, 0);

	bool error = false;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (out[row * 4 + col] != pDest[row * 4 + col]) {
				error = true;
			}
		}
	}

	return cycles;
}