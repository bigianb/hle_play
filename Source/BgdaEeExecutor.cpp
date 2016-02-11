#include "BgdaEeExecutor.h"

#include "NopBlock.h"
#include "BgdaMpegBlock.h"
#include "BgdaDrawSpriteBlock.h"
#include "BgdaDrawColourSpriteBlock.h"
#include "BgdaBeginTextBlock.h"
#include "BgdaDrawTextBlock.h"

BgdaEeExecutor::BasicBlockPtr BgdaEeExecutor::BlockFactory(CMIPS& context, uint32 start, uint32 end)
{
	if (start == 0x00154f6c) {
		return std::make_shared<BgdaMpegBlock>(context, start, end, m_vm);
	}
	if (start > 0x00154f6c && start < 0x00155024){
		return std::make_shared<NopBlock>(context, start, end);
	}
	if (start == 0x140eb0) {
		return std::make_shared<BgdaDrawSpriteBlock>(context, start, end, m_vm);
	}
	if (start == 0x1416a0){
		return std::make_shared<BgdaDrawColourSpriteBlock>(context, start, end, m_vm);
	}
	if (start == 0x143908) {
		return std::make_shared<BgdaBeginTextBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x143c40) {
		// Flush text
//		return std::make_shared<NopBlock>(context, start, end);
	}
	if (start == 0x144328) {
		return std::make_shared<BgdaDrawTextBlock>(bgdaContext, context, start, end, m_vm);
	}

	return CMipsExecutor::BlockFactory(context, start, end);
}
