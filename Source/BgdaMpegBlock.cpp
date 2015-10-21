#include "BgdaMpegBlock.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "GSH_HleOgl.h"

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaMpegBlock::BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm)
{
	std::cout << "Created Mpeg standard block from " << start << " to " << end << std::endl;
}

unsigned int BgdaMpegBlock::Execute()
{
	CGSH_HleOgl* gs = (CGSH_HleOgl*)m_vm.GetGSHandler();
	
	//s0= mpegDecoderInfo struct
	// 8(s0) = frame count
	// $c054(gp) = rgb32 frame buffer. 16x16 blocks arranged column major
	// $5d80(s7) = width
	// 4(s0) = height
	// 0(sp) & 1 == odd.
	// if odd y = 0x210 else 0x10

	uint8* rgb32 = HleVMUtils::getGPOffsetPointer(m_context, 0xc054);
	uint32 frameCount = HleVMUtils::readInt32Indirect(m_context, CMIPS::S0, 8);

	uint32 width = HleVMUtils::readInt32Indirect(m_context, CMIPS::S7, 0x5d80);
	uint32 height = HleVMUtils::readInt32Indirect(m_context, CMIPS::S0, 4);

	uint32 odd = HleVMUtils::readInt32Indirect(m_context, CMIPS::SP, 0);

	// TODO: transfer the RBG data to the GS

	m_context.m_State.nPC = 0x00155024;
	return 1000;
}

