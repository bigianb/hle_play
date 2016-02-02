#pragma once

#include "Types.h"

/**
 * Interface for GS Handlers which support Hle Additions.
*/
class CGHSHle
{
public:
	/**
	Transfers a macroblocked image as returned from sceMpegGetPicture.
	*/
	virtual void	transferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y) = 0;

	virtual void	drawSprite(int xpos, int ypos, int width, int height, uint32 vertexRGBA, uint8* texGsPacketData, bool interlaced, uint64 alphaReg) = 0;

	/**
	Sets the active texture given a linear block of 32 bit pixels.
	*/
	virtual void setTexture32(unsigned char* data, int dataLength, int width, int height, bool interlaced) = 0;

	/**
	Draws a sprite using the current texture.
	*/
	virtual void drawSprite(int xpos, int ypos, int u0, int v0, int width, int height, uint32 vertexRGBA, bool useTexture) = 0;

	/**
	Sets the alpha blend function.
	*/
	virtual void setAlphaBlendFunction(uint64 alphaReg) = 0;

};
