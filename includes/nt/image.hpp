// Copyright (c) 2020 Can Boluk
// All rights reserved.   
//    
// Redistribution and use in source and binary forms, with or without   
// modification, are permitted provided that the following conditions are met: 
//    
// 1. Redistributions of source code must retain the above copyright notice,   
//    this list of conditions and the following disclaimer.   
// 2. Redistributions in binary form must reproduce the above copyright   
//    notice, this list of conditions and the following disclaimer in the   
//    documentation and/or other materials provided with the distribution.   
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.   
//    
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE   
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR   
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF   
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN   
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)   
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  
// POSSIBILITY OF SUCH DAMAGE.        
//
#pragma once
#include "../img_common.hpp"
#include "nt_headers.hpp"
#include "directories/dir_debug.hpp"
#include "directories/dir_exceptions.hpp"
#include "directories/dir_export.hpp"
#include "directories/dir_iat.hpp"
#include "directories/dir_import.hpp"
#include "directories/dir_relocs.hpp"
#include "directories/dir_tls.hpp"
#include "directories/dir_load_config.hpp"
#include "directories/dir_resource.hpp"
#include "directories/dir_security.hpp"
#include "directories/dir_delay_load.hpp"

namespace win
{
    // Image wrapper
    //
    template<bool x64 = default_architecture>
    struct image_t
    {
        dos_header_t                dos_header;
        
        // Basic getters.
        //
        inline dos_header_t* get_dos_headers() { return &dos_header; }
        inline const dos_header_t* get_dos_headers() const { return &dos_header; }
        inline file_header_t* get_file_header() { return dos_header.get_file_header(); }
        inline const file_header_t* get_file_header() const { return dos_header.get_file_header(); }
        inline nt_headers_t<x64>* get_nt_headers() { return dos_header.get_nt_headers<x64>(); }
        inline const nt_headers_t<x64>* get_nt_headers() const { return dos_header.get_nt_headers<x64>(); }

        // Calculation of optional header checksum.
        //
        inline uint32_t compute_checksum( size_t file_len ) const
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
            return presult + ( uint32_t ) file_len;
        }
        inline void update_checksum( size_t file_len )
        {
            get_nt_headers()->optional_header.checksum = compute_checksum( file_len );
        }

        // Directory getter
        //
        inline data_directory_t* get_directory( directory_id id )
        {
            auto nt_hdrs = get_nt_headers();
            if ( nt_hdrs->optional_header.num_data_directories <= id ) return nullptr;
            data_directory_t* dir = &nt_hdrs->optional_header.data_directories.entries[ id ];
            return dir->present() ? dir : nullptr;
        }
        inline const data_directory_t* get_directory( directory_id id ) const { return const_cast< image_t* >( this )->get_directory( id ); }

        // RVA to section mapping
        //
        inline section_header_t* rva_to_section( uint32_t rva )
        {
            auto nt_hdrs = get_nt_headers();
            for ( size_t i = 0; i != nt_hdrs->file_header.num_sections; i++ )
            {
                auto section = nt_hdrs->get_section( i );
                if ( section->virtual_address <= rva && rva < ( section->virtual_address + section->virtual_size ) )
                    return section;
            }
            return nullptr;
        }
        inline const section_header_t* rva_to_section( uint32_t rva ) const { return const_cast< image_t* >( this )->rva_to_section( rva ); }

        // RVA to raw offset mapping
        //
        template<typename T = void>
        inline T* rva_to_ptr( uint32_t rva )
        {
            auto scn = rva_to_section( rva );
            if ( !scn ) return nullptr;

            size_t offset = rva - scn->virtual_address;
            if ( offset >= scn->size_raw_data ) return nullptr;

            return ( T* ) ( ( uint8_t* ) &dos_header + scn->ptr_raw_data + offset );
        }
        template<typename T = void>
        inline const T* rva_to_ptr( uint32_t rva ) const { return const_cast< image_t* >( this )->template rva_to_ptr<const T>( rva ); }
    };
    using image_x64_t = image_t<true>;
    using image_x86_t = image_t<false>;
};