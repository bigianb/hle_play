#pragma once

#include "BasicBlock.h"

class CPS2VM;
class Texture;
class CGHSHle;
class FntDecoder;
class BgdaContext;

/**
Code for setting text colour.
*/
class BgdaSetTextColourBlock : public CBasicBlock
{
public:
	BgdaSetTextColourBlock(BgdaContext& bgdaContext, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);
	virtual ~BgdaSetTextColourBlock();

	unsigned int					Execute();
	//void							Compile() {}

private:
	CPS2VM&		m_vm;
	BgdaContext& bgdaContext;
};
