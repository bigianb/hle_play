#pragma once

#include "BasicBlock.h"

/**
    A collection of useful functions for dealing with the vm.
*/
namespace HleVMUtils
{
	static inline
	uint8* getPointer(CMIPS& context, uint32 addr)
	{
		addr = context.m_pAddrTranslator(&context, addr);
		const CMemoryMap::MEMORYMAPELEMENT* e = context.m_pMemoryMap->GetReadMap(addr);
		if (e == nullptr) {
			return nullptr;
		}
		uint8* p = reinterpret_cast<uint8*>(e->pPointer) + (addr - e->nStart);
		return p;
	}

	static inline
	uint8* getGPOffsetPointer(CMIPS& context, int offset)
	{
		uint32 addr = context.m_State.nGPR[CMIPS::GP].nV0 + (int16)offset;
		return getPointer(context, addr);
	}

	static inline
	uint8* getOffsetPointer(CMIPS& context, CMIPS::REGISTER reg, int offset)
	{
		uint32 addr = context.m_State.nGPR[reg].nV0 + (int16)offset;
		return addr == 0 ? 0 : getPointer(context, addr);
	}

	static inline
	uint32 readInt32Indirect(CMIPS& context, CMIPS::REGISTER reg, int offset)
	{
		uint8* p = getOffsetPointer(context, reg, offset);
		// Could just cast if we are running on a Little Endian machine
		uint32 val = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
		return val;
	}
}
