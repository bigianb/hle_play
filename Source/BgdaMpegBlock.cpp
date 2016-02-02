#include "BgdaMpegBlock.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "gs/GSH_Hle.h"
#include "gs/GSH_HleSoftware.h"

BgdaMpegBlock::BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm)
{
	std::cout << "Created Mpeg standard block from " << start << " to " << end << std::endl;
}

unsigned int BgdaMpegBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());
	
	//s0= mpegDecoderInfo struct
	// 8(s0) = frame count
	// $c054(gp) = rgb32 frame buffer. 16x16 blocks arranged column major
	// $5d80(s7) = width
	// 4(s0) = height
	// 0(sp) & 1 == odd.
	// if odd y = 0x210 else 0x10

	// This is the address of the variable which is of type void*, need to de-reference it and get the pointer.
	uint32* prgb32 = (uint32*)HleVMUtils::getGPOffsetPointer(m_context, 0xc054);
	uint8* rgb32 = HleVMUtils::getPointer(m_context, prgb32[0]);

	uint32 frameCount = HleVMUtils::readInt32Indirect(m_context, CMIPS::S0, 8);

	uint32 width = HleVMUtils::readInt32Indirect(m_context, CMIPS::S7, 0x5d80);
	uint32 height = HleVMUtils::readInt32Indirect(m_context, CMIPS::S0, 4);

	uint32 odd = HleVMUtils::readInt32Indirect(m_context, CMIPS::SP, 0);

	gs->transferBlockedImage(0x10, width / 16, height / 16, (uint32*)rgb32, 0, 0x0a, 0, odd ? 0x210 : 0x10);

	m_context.m_State.nPC = 0x00155024;
	return 5000;
}

