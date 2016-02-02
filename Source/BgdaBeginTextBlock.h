#pragma once

#include "BasicBlock.h"

class CPS2VM;
class Texture;
class CGHSHle;
class FntDecoder;
class BgdaContext;

/**
Code for drawing text.
*/
class BgdaBeginTextBlock : public CBasicBlock
{
public:
	BgdaBeginTextBlock(BgdaContext& bgdaContext, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);
	virtual ~BgdaBeginTextBlock();

	unsigned int					Execute();
	//void							Compile() {}

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;
};
