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
	BgdaDrawSpriteBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm, bool hasScissor);

	void setScissor(int x0, int y0, int x1, int y1);

	unsigned int					Execute();
	void							Compile() {}

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;

	bool hasScissor;
};
