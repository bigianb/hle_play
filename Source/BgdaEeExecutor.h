#pragma once

#include "ee/EeExecutor.h"

/* An executor specialised for Baldurs Gate Dark Alliance. */
class BgdaEeExecutor : public CEeExecutor
{
public:
						BgdaEeExecutor(CMIPS& context, uint8* ram) : CEeExecutor(context, ram){}
	BasicBlockPtr		BlockFactory(CMIPS&, uint32, uint32) override;

};
