#pragma once

#include "BasicBlock.h"

class CPS2VM;
class Texture;
class CGHSHle;
class FntDecoder;
class BgdaContext;

/**
Code for starting a DMA transfer text.
*/
class BgdaKickoffDmaBlock : public CBasicBlock
{
public:
	BgdaKickoffDmaBlock(BgdaContext& bgdaContext, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);
	virtual ~BgdaKickoffDmaBlock();

	unsigned int	Execute();

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;
};
