#pragma once

#include "BasicBlock.h"

class CPS2VM;

/**
  This code transfers the results of the mpeg frame decode to the display buffer of the GS.
  The picture returned by the mpeg decoded is composed of 16x16 pixel blocks in RGBA32 format.
  The blocks are arranged in column major format. This needs changing to a linear row major format
  during the transfer.
*/
class BgdaMpegBlock : public CBasicBlock
{
public:
	BgdaMpegBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);

	unsigned int					Execute();
	void							Compile() {}

private:
	CPS2VM&		m_vm;
};
