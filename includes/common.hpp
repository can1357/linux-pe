#pragma once
#include <stdint.h>
#include <type_traits>
#include <immintrin.h>

#define WIN_STRUCT_PACKING				4

// If your compiler does not support zero-len arrays, define VAR_LEN as 1 before including linuxpe.
//
#ifndef VAR_LEN
	#pragma warning(disable:4200)
	#define VAR_LEN
#endif

namespace win
{
	// Default image architecture.
	//
	static constexpr bool default_architecture = sizeof( void* ) == 8;

	// NT versioning
	//
	union version_t
	{
		uint16_t					identifier;
		struct
		{
			uint8_t					major;
			uint8_t					minor;
		};
	};
	union ex_version_t
	{
		uint32_t					identifier;
		struct
		{
			uint16_t				major;
			uint16_t				minor;
		};
	};
};