#pragma once

#include "BasicBlock.h"

class CPS2VM;
class Texture;
class CGHSHle;
class FntDecoder;
class BgdaContext;

/**
Displays the back buffer and then clears it.
*/
class BgdaDisplayBackBufferBlock : public CBasicBlock
{
public:
	BgdaDisplayBackBufferBlock(BgdaContext& bgdaContext, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);
	virtual ~BgdaDisplayBackBufferBlock();

	unsigned int					Execute();
	//void							Compile() {}

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;
};
