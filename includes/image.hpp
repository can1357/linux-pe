#pragma once
#include "nt_headers.hpp"

#include "dir_debug.hpp"
#include "dir_exceptions.hpp"
#include "dir_export.hpp"
#include "dir_iat.hpp"
#include "dir_import.hpp"
#include "dir_relocs.hpp"
#include "dir_tls.hpp"

namespace win
{
	// Image wrapper
	//
	template<bool x64 = IS_DEF_AMD64>
	struct image_t
	{
		dos_header_t				dos_header;
		
		inline operator dos_header_t&() { return dos_header; }
		inline dos_header_t& get_dos_headers() { return &dos_header; }

		inline nt_headers_t<x64>* get_nt_headers() { return dos_header.get_nt_headers<x64>(); }

		inline uint32_t compete_checksum( uint32_t file_len )
		{
			// Calculate partial sum
			uint32_t psum_tmp = 0;
			uint16_t* raw_data = ( uint16_t* ) &dos_header;
			for ( uint32_t off = 0; off < ( file_len + 1 ) >> 1; off++ )
			{
				// Add uint16_t
				psum_tmp += raw_data[ off ];
				// If it overflows, increment by one
				psum_tmp = ( psum_tmp >> 16 ) + ( psum_tmp & 0xFFFF );
			}
			uint16_t partial_sum = psum_tmp;

			// Adjust for the previous .checkum field (=0)
			uint16_t* adjust_sum = ( uint16_t* ) &get_nt_headers()->optional_header.checksum;
			for ( int i = 0; i < 2; i++ )
			{
				// If it underflows, decrement by one
				partial_sum -= partial_sum < adjust_sum[ i ];
				// Substract uint16_t
				partial_sum -= adjust_sum[ i ];
			}

			// Return result
			return ( uint32_t ) partial_sum + file_len;
		}
	};
	using image_x64_t = image_t<true>;
	using image_x86_t = image_t<false>;
};