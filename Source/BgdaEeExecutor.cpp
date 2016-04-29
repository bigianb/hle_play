#include "BgdaEeExecutor.h"

#include "NopBlock.h"
#include "BgdaMpegBlock.h"
#include "BgdaDrawSpriteBlock.h"
#include "BgdaDrawColourSpriteBlock.h"
#include "BgdaBeginTextBlock.h"
#include "BgdaDrawTextBlock.h"
#include "BgdaKickoffDmaBlock.h"
#include "BgdaSetTextColourBlock.h"
#include "BgdaDrawModelBlock.h"
#include "BgdaMatrixRoutines.h"
#include "BgdaDisplayBackBufferBlock.h"

BgdaEeExecutor::BasicBlockPtr BgdaEeExecutor::BlockFactory(CMIPS& context, uint32 start, uint32 end)
{
	if (start == 0x12ee60) {
		return std::make_shared<BgdaMatrixMulBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x131b00) {
//		return std::make_shared<BgdaSetupProjMatrixBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x135c08) {
		return std::make_shared<BgdaKickoffDmaBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x13e618) {
		return std::make_shared<BgdaDrawModelBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x13ec48) {
		return std::make_shared<BgdaDrawModelBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x13f108) {
		return std::make_shared<BgdaDrawModelBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x140eb0) {
		return std::make_shared<BgdaDrawSpriteBlock>(bgdaContext, context, start, end, m_vm, false);
	}
	if (start == 0x141210) {
		return std::make_shared<BgdaDrawSpriteBlock>(bgdaContext, context, start, end, m_vm, true);
	}
	if (start == 0x1416a0){
		return std::make_shared<BgdaDrawColourSpriteBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x143908) {
		return std::make_shared<BgdaBeginTextBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x143aa0) {
		return std::make_shared<BgdaSetTextColourBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x143c40) {
		// Flush text
//		return std::make_shared<NopBlock>(context, start, end);
	}
	if (start == 0x144328) {
		return std::make_shared<BgdaDrawTextBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start == 0x00154f6c) {
		return std::make_shared<BgdaMpegBlock>(bgdaContext, context, start, end, m_vm);
	}
	if (start > 0x00154f6c && start < 0x00155024) {
		return std::make_shared<NopBlock>(context, start, end);
	}
	if (start == 0x00201a50) {
		return std::make_shared<BgdaDisplayBackBufferBlock>(bgdaContext, context, start, end, m_vm);
	}
	return CMipsExecutor::BlockFactory(context, start, end);
}
