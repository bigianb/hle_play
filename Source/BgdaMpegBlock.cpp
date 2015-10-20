#include "BgdaMpegBlock.h"
#include "iostream"
#include "PS2VM.h"

BgdaMpegBlock::BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) : CBasicBlock(context, start, end), m_vm(vm)
{
	std::cout << "Created Mpeg standard block from " << start << " to " << end << std::endl;
}

unsigned int BgdaMpegBlock::Execute()
{
	// TODO: need to get the GIF/GS here
	//m_vm.GetGSHandler()->ProcessImageTransfer
	return CBasicBlock::Execute();
}
