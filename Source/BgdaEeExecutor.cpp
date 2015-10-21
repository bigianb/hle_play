#include "BgdaEeExecutor.h"

#include "NopBlock.h"
#include "BgdaMpegBlock.h"

BgdaEeExecutor::BasicBlockPtr BgdaEeExecutor::BlockFactory(CMIPS& context, uint32 start, uint32 end)
{
	if (start == 0x00154f6c) {
		return std::make_shared<BgdaMpegBlock>(context, start, end, m_vm);
	}
	if (start > 0x00154f6c && start < 0x00155024){
		return std::make_shared<NopBlock>(context, start, end);
	}
	return CMipsExecutor::BlockFactory(context, start, end);
}

