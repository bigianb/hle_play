#include "BgdaMpegBlock.h"
#include "iostream"

BgdaMpegBlock::BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end) : CBasicBlock(context, start, end)
{
	std::cout << "Created Mpeg standard block from " << start << " to " << end << std::endl;
}

unsigned int BgdaMpegBlock::Execute()
{
	return CBasicBlock::Execute();
}