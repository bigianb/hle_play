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
	fontTexture = nullptr;
}

BgdaDrawTextBlock::~BgdaDrawTextBlock()
{
	delete fontTexture;
}

unsigned int BgdaDrawTextBlock::Execute()
{
	CGHSHle* gs = dynamic_cast<CGHSHle*>(m_vm.GetGSHandler());

	uint32 isInterlaced = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xa4d0);

	uint32 xpos = m_context.m_State.nGPR[CMIPS::A0].nV0;
	uint32 ypos = m_context.m_State.nGPR[CMIPS::A1].nV0;
	uint16* pCharsOrGlyphs = reinterpret_cast<uint16*>(HleVMUtils::getOffsetPointer(m_context, CMIPS::A2, 0));

	int length = 0;
	uint16* p = pCharsOrGlyphs;
	while ((*p & ~0xf800) != 0) {
		++p; ++length;
	}

	CLog::GetInstance().Print(LOG_NAME, "BgdaDrawTextBlock(%d, %d, length=%d)\n", xpos, ypos, length);

	if (length > 0) {
		uint32 fontPS2Addr = HleVMUtils::readInt32Indirect(m_context, CMIPS::GP, 0xbcf4);
		uint8* pFont = HleVMUtils::getPointer(m_context, fontPS2Addr);
		setActiveFont(pFont, fontPS2Addr);
		FntDecoder fntDecoder;
		fntDecoder.charsToGlyphsInplace(pActiveFont, pCharsOrGlyphs, length, fontPS2Addr);

		drawGlyphs(gs, xpos, ypos, fntDecoder, pCharsOrGlyphs, length, isInterlaced);
	}
	m_context.m_State.nPC = m_context.m_State.nGPR[CMIPS::RA].nV0;
	return 10;
}

void BgdaDrawTextBlock::drawGlyphs(CGHSHle* gs, int xpos, int ypos, FntDecoder& fntDecoder, uint16* pGlyphs, uint32 length, bool isInterlaced)
{
	for (int glyphNum = 0; glyphNum < length; ++glyphNum)
	{
		uint16 glyphCode = pGlyphs[glyphNum];
		GlyphInfo& glyphInfo = fntDecoder.lookupGlyph(pActiveFont, glyphCode, pActiveFontPS2Address);

		gs->setAlphaBlendFunction(0x44);
		gs->setTexture32(fontTexture->data, fontTexture->dataLength, fontTexture->widthPixels, fontTexture->heightPixels, isInterlaced);
		gs->drawSprite(xpos, ypos, glyphInfo.x0, glyphInfo.y0, glyphInfo.x1 - glyphInfo.x0, glyphInfo.y1 - glyphInfo.y0, bgdaContext.currentTextColour, true);

		xpos += glyphInfo.width;
	}
}

void BgdaDrawTextBlock::setActiveFont(uint8* font, int fontPs2Address)
{
	if (font == pActiveFont) {
		return;
	}
	delete fontTexture;
	TexDecoder decoder;
	int textureAddress = DataUtil::getLEInt(font, 0x10);
	uint8* pTexture = HleVMUtils::getPointer(m_context, textureAddress);
	fontTexture = decoder.decode(pTexture, textureAddress);
	pActiveFont = font;
	pActiveFontPS2Address = fontPs2Address;
}

