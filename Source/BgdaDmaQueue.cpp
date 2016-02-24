#include "BgdaDmaQueue.h"

void BgdaDmaQueue::flush(CGHSHle* gs)
{
	for (auto& slot : slots){
		for (BgdaDmaItem* item : slot) {
			item->execute(gs);
			delete item;
		}
		slot.clear();
	}
}
