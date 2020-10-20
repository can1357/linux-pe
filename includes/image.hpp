#pragma once
#include "nt_headers.hpp"

#include "dir_debug.hpp"
#include "dir_exceptions.hpp"
#include "dir_export.hpp"
#include "dir_iat.hpp"
#include "dir_import.hpp"
#include "dir_relocs.hpp"
#include "dir_tls.hpp"
#include "dir_load_config.hpp"
#include "dir_resource.hpp"

// TODO:
// - Implement security directory
// - Implement parsing helpers
namespace win
{
	// Image wrapper
	//
	template<bool x64 = IS_DEF_AMD64>
	struct image_t
	{
		dos_header_t				dos_header;
		
		inline dos_header_t* get_dos_headers() { return &dos_header; }
		inline const dos_header_t* get_dos_headers() const { return &dos_header; }
		inline nt_headers_t<x64>* get_nt_headers() { return dos_header.get_nt_headers<x64>(); }
		inline const nt_headers_t<x64>* get_nt_headers() const { return dos_header.get_nt_headers<x64>(); }

		inline uint32_t compute_checksum( uint32_t file_len ) const
		{
			// Sum over each word.
			//
			uint32_t chksum = 0;
			const uint16_t* wdata = ( const uint16_t* ) this;
			for ( size_t n = 0; n != file_len / 2; n++ )
			{
				uint32_t sum = wdata[ n ] + chksum;
				chksum = ( uint16_t ) sum + ( sum >> 16 );
			}

			// If there's a byte left append it.
			//
			uint16_t presult = chksum + ( chksum >> 16 );
			if ( file_len & 1 )
				presult += *( ( ( const char* ) this ) + file_len - 1 );

			// Adjust for the previous .checkum field (=0)
			//
			uint16_t* adjust_sum = ( uint16_t* ) &get_nt_headers()->optional_header.checksum;
			for ( size_t i = 0; i != 2; i++ )
			{
				presult -= presult < adjust_sum[ i ];
				presult -= adjust_sum[ i ];
			}
			return presult + file_len;
		}
		inline void update_checksum( uint32_t file_len )
		{
			get_nt_headers()->optional_header.checksum = compute_checksum( file_len );
		}

		inline data_directory_t* get_directory( directory_id id )
		{
			auto nt_hdrs = get_nt_headers();
			if ( nt_hdrs->optional_header.num_data_directories <= id ) return nullptr;
			data_directory_t* dir = &nt_hdrs->optional_header.data_directories.entries[ id ];
			return dir->present() ? dir : nullptr;
		}
		inline const data_directory_t* get_directory( directory_id id ) const { const_cast< image_t* >( this )->get_directory( id ); }

		inline section_header_t* rva_to_section( uint32_t rva )
		{
			auto nt_hdrs = get_nt_headers();
			for ( int i = 0; i < nt_hdrs->file_header.num_sections; i++ )
			{
				auto section = nt_hdrs->get_section( i );
				if ( section->virtual_address <= rva && rva < ( section->virtual_address + section->virtual_size ) )
					return section;
			}
			return nullptr;
		}
		inline const section_header_t* rva_to_section( uint32_t rva ) const { const_cast< image_t* >( this )->rva_to_section( rva ); }

		template<typename T = void>
		inline T* rva_to_ptr( uint32_t rva )
		{
			auto nt_hdrs = get_nt_headers();
			if ( !rva || nt_hdrs->optional_header.size_image <= rva ) return nullptr;
			
			uint8_t* output = rva + ( uint8_t* ) &dos_header;
			for ( int i = 0; i < nt_hdrs->file_header.num_sections; i++ )
			{
				auto section = nt_hdrs->get_section( i );
				if ( section->virtual_address <= rva && rva < ( section->virtual_address + section->virtual_size ) )
				{
					output = output - section->virtual_address + section->ptr_raw_data;
					break;
				}
			}

			return ( T* ) output;
		}
		template<typename T = void>
		inline const T* rva_to_ptr( uint32_t rva ) const { const_cast< image_t* >( this )->template rva_to_ptr<T>( rva ); }
	};
	using image_x64_t = image_t<true>;
	using image_x86_t = image_t<false>;
};