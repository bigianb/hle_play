#pragma once

#include "ee/EeExecutor.h"

#include "bgdaContext.h"

class CPS2VM;

/* An executor specialised for Baldurs Gate Dark Alliance. */
class BgdaEeExecutor : public CEeExecutor
{
public:
	BgdaEeExecutor(CMIPS& context, uint8* ram, CPS2VM& vm) : CEeExecutor(context, ram), m_vm(vm){}
	BasicBlockPtr		BlockFactory(CMIPS&, uint32, uint32) override;

private:
	CPS2VM& m_vm;

	BgdaContext bgdaContext;

};
