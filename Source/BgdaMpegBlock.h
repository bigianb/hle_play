#pragma once

#include "BasicBlock.h"

class BgdaMpegBlock : public CBasicBlock
{
public:
	BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end);

	unsigned int					Execute();
	// Once execute is working, we will want to stub out compile.
	//void							Compile();
};