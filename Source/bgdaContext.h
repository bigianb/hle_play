#pragma once

#include "Types.h"
#include "BgdaDmaQueue.h"

/**
  Stores shared context required by the various blocks.
*/
class BgdaContext
{
public:
	uint32 currentTextColour;

	BgdaDmaQueue dmaQueue;

};