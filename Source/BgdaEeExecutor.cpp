#include "BgdaEeExecutor.h"

#include "BgdaMpegBlock.h"

BgdaEeExecutor::BasicBlockPtr BgdaEeExecutor::BlockFactory(CMIPS& context, uint32 start, uint32 end)
{
	if (start >= 0x00203ab0 && start < 0x00203c44){
		return std::make_shared<BgdaMpegBlock>(context, start, end);
	}
	return CMipsExecutor::BlockFactory(context, start, end);
}

