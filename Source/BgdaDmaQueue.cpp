#include "BgdaDmaQueue.h"

void BgdaDmaQueue::flush(CGHSHle* gs)
{
	for (auto& slot : slots){
		for (auto& item : slot) {
			item.execute(gs);
		}
		slot.clear();
	}
}
