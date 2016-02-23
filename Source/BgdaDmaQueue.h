#pragma once

#include <list>

class CGHSHle;

// Snowblind uses a set of queues for the DMA.
// There are 8 slots which are sent to the GS/VIF in order.
// Each slots holds a list of items.
// When drawing a scene, the code will generally prepend to the lists.

class BgdaDmaItem
{
	virtual void execute(CGHSHle* gs) = 0;
};

class BgdaDmaQueue
{
	std::list<BgdaDmaItem> slots[8];

public:
	void prependItem(int slot, BgdaDmaItem& item)
	{
		slots[slot].push_front(item);
	}

	void flush(CGHSHle* gs);
};

