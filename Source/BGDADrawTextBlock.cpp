#include "BgdaDrawTextBlock.h"
#include "bgdaContext.h"
#include "iostream"
#include "PS2VM.h"
#include "HleVMUtils.h"
#include "snowlib/DataUtil.h"
#include "gs/GSH_Hle.h"
#include "snowlib/TexDecoder.h"
#include "snowlib/FntDecoder.h"
#include "snowlib/Texture.h"

#include "Log.h"
#define LOG_NAME	("bgda")

// Enable debugging in release mode
#pragma optimize( "", off )

BgdaDrawTextBlock::BgdaDrawTextBlock(BgdaContext& bgdaContextIn, CMIPS& context, uint32 start, uint32 end, CPS2VM& vm) :
	CBasicBlock(context, start, end), m_vm(vm), pActiveFont(nullptr), bgdaContext(bgdaContextIn)
{
}

BgdaDrawTextBlock::~BgdaDrawTextBlock()
{
}

class BgdaDrawTextBlockDmaItem : public BgdaDmaItem
{
public:
	BgdaContext& bgdaContext;
	CMIPS& context;
	bool isInterlaced;
	uint32 xpos;
	uint32 ypos;
	
	int length;
	uint16 glyphs[256];

	uint32 fontPS2Addr;
	uint32 colour;

	BgdaDrawTextBlockDmaItem(BgdaContext& bgdaContextIn, CMIPS& contextIn, uint16* pGlyphs, int len) : bgdaContext(bgdaContextIn), context(contextIn)
	{
		length = len > 256 ? 256 : len;
		memcpy(glyphs, pGlyphs, length * sizeof(uint16));
	}

	void execute(CGHSHle* gs)
	{
		FntDecoder fntDecoder;
		drawGlyphs(gs, xpos, ypos, fntDecoder, length, true /*isInterlaced*/);
	}

	// Not very efficient as it parses and uploads the font each time.
	void drawGlyphs(CGHSHle* gs, int xpos, int ypos, FntDecoder& fntDecoder, uint32 length, bool isInterlaced)
	{
		TexDecoder decoder;
		uint8* pFont = HleVMUtils::getPointer(context, fontPS2Addr);
		int textureAddress = DataUtil::getLEInt(pFont, 0x10);
		uint8* pTexture = HleVMUtils::getPointer(context, textureAddress);
		Texture* fontTexture = decoder.decode(pTexture, textureAddress);

		uint16 prevGlyphCode = 0;
		for (int glyphNum = 0; glyphNum < length; ++glyphNum)
		{
			uint16 glyphCode = glyphs[glyphNum];
			GlyphInfo& glyphInfo = fntDecoder.lookupGlyph(pFont, glyphCode, fontPS2Addr);

			xpos += fntDecoder.getKernPairAdjust(pFont, prevGlyphCode, glyphCode, fontPS2Addr);

			gs->setAlphaBlendFunction(0x44);
			gs->setTexture32(fontTexture->data, fontTexture->dataLength, fontTexture->widthPixels, fontTexture->logicalHeight, isInterlaced);
			gs->drawSprite(xpos, ypos + glyphInfo.yOffset, glyphInfo.x0, glyphInfo.y0, glyphInfo.x1 - glyphInfo.x0, glyphInfo.y1 - glyphInfo.y0, colour, true);

			xpos += fntDecoder.getCharAdvance(pFont, glyphCode, fontPS2Addr);

			prevGlyphCode = glyphCode;
		}
	}

};

unsigned int BgdaDrawTextBlock::Execute()
{
	uint32 isInterlaced = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xa4d0);

	uint32 xpos = m_context.m_State.nGPR[CMIPS::A0].nV0;
	uint32 ypos = m_context.m_State.nGPR[CMIPS::A1].nV0;
	uint16* pCharsOrGlyphs = reinterpret_cast<uint16*>(HleVMUtils::getOffsetPointer(m_context, CMIPS::A2, 0));

	int length = 0;
	uint16* p = pCharsOrGlyphs;
	while (*p != 0) {
		++p; ++length;
	}

	CLog::GetInstance().Print(LOG_NAME, "BgdaDrawTextBlock(%d, %d, length=%d)\n", xpos, ypos, length);

	if (length > 0) {
		uint32 fontPS2Addr = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xbfc4);
		BgdaDrawTextBlockDmaItem* item = new BgdaDrawTextBlockDmaItem(bgdaContext, m_context, pCharsOrGlyphs, length);
		item->isInterlaced = isInterlaced != 0;
		item->xpos = xpos;
		item->ypos = ypos;
		item->fontPS2Addr = fontPS2Addr;
		item->colour = bgdaContext.currentTextColour;

		bgdaContext.dmaQueue.prependItem(7, item);
	}
	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}




