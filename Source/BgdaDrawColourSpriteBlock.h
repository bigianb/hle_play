#pragma once

#include "BasicBlock.h"

class CPS2VM;

/**
  Code for drawing a sprite which is a single coloured rectangle.
*/
class BgdaDrawColourSpriteBlock : public CBasicBlock
{
public:
	BgdaDrawColourSpriteBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);

	unsigned int					Execute();
	void							Compile() {}

private:
	CPS2VM&		m_vm;
};
