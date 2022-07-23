/// @file

#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"

TEST(MemAllocTest, Memory, 1000)
{
	EngineInitializer engInitGuard;

	int nAllocSize = 128; // request for 128 bytes

	void *pData = Mem_Malloc(nAllocSize);

	printf("Alloc with Mem_Malloc\n"
	       "Requested %d bytes\n"
	       "Allocated data pointer: %\n",
	       nAllocSize, pData);

	Mem_Free(pData);
};

TEST(ZeroMemAllocTest, Memory, 1000)
{
	EngineInitializer engInitGuard;

	int nAllocSize = 128; // request for 128 bytes

	void *pData = Mem_ZeroMalloc(nAllocSize);

	printf("Alloc with Mem_ZeroMalloc\n"
	       "Requested %d bytes\n"
	       "Allocated data pointer: %\n",
	       nAllocSize, pData);

	Mem_Free(pData);
};

TEST(CAllocTest, Memory, 1000)
{
	EngineInitializer engInitGuard;

	int nAllocSize = 128; // request for 128 bytes

	void *pData = Mem_Calloc(0, nAllocSize);

	printf("Alloc with Mem_Calloc\n"
	       "Requested %d bytes\n"
	       "Allocated data pointer: %\n",
	       nAllocSize, pData);

	Mem_Free(pData);
};