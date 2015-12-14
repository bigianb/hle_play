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
	virtual void	TransferBlockedImage(int blockSize, int widthInBlocks, int heightInBlocks, uint32* pRGBA, int dbp, int dbw, int x, int y) = 0;

	virtual void	DrawSprite(int xpos, int ypos, int width, int height, uint32 vertexRGBA, uint8* texGsPacketData, bool interlaced, uint64 alphaReg) = 0;
};
