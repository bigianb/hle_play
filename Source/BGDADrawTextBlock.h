#pragma once

#include "BasicBlock.h"

class CPS2VM;
class Texture;
class CGHSHle;
class FntDecoder;

/**
  Code for drawing text.
*/
class BgdaDrawTextBlock : public CBasicBlock
{
public:
	BgdaDrawTextBlock(CMIPS& context, uint32 start, uint32 end, CPS2VM& vm);
	virtual ~BgdaDrawTextBlock();

	unsigned int					Execute();
	void							Compile() {}

private:
	CPS2VM&		m_vm;

	void setActiveFont(uint8* font, int pActiveFontPS2Address);
	uint8* pActiveFont;
	int pActiveFontPS2Address;
	Texture* fontTexture;

	void drawGlyphs(CGHSHle* gs, int xpos, int ypos, FntDecoder& fntDecoder, uint16* pGlyphs, uint32 length, bool isInterlaced);

};
