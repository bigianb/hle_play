#pragma once

#include "BasicBlock.h"

class CPS2VM;
class BgdaContext;

/**
  Code for drawing a sprite.
*/
class BgdaDrawSpriteBlock : public CBasicBlock
{
public:
	BgdaDrawSpriteBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);

	unsigned int					Execute();
	void							Compile() {}

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;
};
