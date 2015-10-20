#pragma once

#include "BasicBlock.h"

/**
    A NopBlock (no-operation block) doesn't do anything and is used for stubbing out code that should not be executed in an HLE context.
*/
class NopBlock : public CBasicBlock
{
public:
	NopBlock(CMIPS& context, uint32 start, uint32 end) : CBasicBlock(context, start, end){}

	unsigned int					Execute() { return 1; }
	void							Compile() {}
};