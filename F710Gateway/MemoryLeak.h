#ifndef __MEMORY_KEAK_H__
#define __MEMORY_KEAK_H__

#include <cstdlib>	// •K—v

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new  ::new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif

#endif /* __MEMORY_KEAK_H__ */