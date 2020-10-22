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
#include <type_traits>
#include <string_view>
#include "common.hpp"
#include "data_directories.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    enum resource_id : uint16_t
    {
        cursor =                    1,
        bitmap =                    2,
        icon =                      3,
        menu =                      4,
        dialog =                    5,
        string =                    6,
        font_dir =                  7,
        font =                      8,
        accelerator =               9,
        rcdata =                    10,
        message_table =             11,
        group_cursor =              12,
        group_icon =                14,
        version =                   16,
        dlg_include =               17,
        plug_play =                 19,
        vxd =                       20,
        ani_cursor =                21,
        ani_icon =                  22,
        html =                      23,
        manifest =                  24,
    };

    template<typename C = wchar_t>
    struct resource_directory_string_t
    {
        uint16_t                    length;
        C                           name[ VAR_LEN ];
    };

    struct resource_directory_data_t
    {
        uint32_t                    offset_to_data;
        uint32_t                    size;
        uint32_t                    code_page;
        uint32_t                    _pad0;
    };

    struct resource_directory_entry_t
    {
        union
        {
            struct
            {
                uint32_t            offset_name     : 31;
                uint32_t            is_named        : 1;
            };
            uint16_t                identifier;
        };
        uint32_t                    offset          : 31;
        uint32_t                    is_directory    : 1;
    };

    struct resource_directory_desc_t
    {
        uint32_t                    characteristics;
        uint32_t                    timedate_stamp;
        ex_version_t                version;
        uint16_t                    num_named_entries;
        uint16_t                    num_id_entries;
        resource_directory_entry_t  entries[ VAR_LEN ];

        inline size_t num_entries() const { return num_named_entries + num_id_entries; }
    };

    // Contains { Type -> Name -> Lang } directory, nested
    struct resource_directory_t
    {

        resource_directory_desc_t   type_directory;

        template<typename T> inline T* resolve_offset( uint32_t offset ) { return ( T* ) ( ( char* ) this + offset ); }
        template<typename T> inline const T* resolve_offset( uint32_t offset ) const { return ( T* ) ( ( char* ) this + offset ); }

        inline resource_directory_data_t* resolve_data( const resource_directory_entry_t& entry ) { return !entry.is_directory ? resolve_offset<resource_directory_data_t>( entry.offset ) : nullptr; }
        inline resource_directory_desc_t* resolve_directory( const resource_directory_entry_t& entry ) { return entry.is_directory ? resolve_offset<resource_directory_desc_t>( entry.offset ) : nullptr; }
        inline resource_directory_data_t* resolve_data( const resource_directory_entry_t& entry ) const { return const_cast< resource_directory_t* >( this )->resolve_data( entry ); }
        inline resource_directory_desc_t* resolve_directory( const resource_directory_entry_t& entry ) const { return const_cast< resource_directory_t* >( this )->resolve_directory( entry ); }

        template<typename C = wchar_t> inline resource_directory_string_t<C>* resolve_name_entry( const resource_directory_entry_t& entry ) { return entry.is_named ? resolve_offset<resource_directory_string_t<C>>( entry.offset_name ) : nullptr; }
        template<typename C = wchar_t> inline const resource_directory_string_t<C>* resolve_name_entry( const resource_directory_entry_t& entry ) const { return const_cast< resource_directory_t* >( this )->template resolve_name_entry<C>( entry ); }

        template<typename C = wchar_t>
        inline std::basic_string_view<C> resolve_name( const resource_directory_entry_t& entry ) const
        {
            if ( auto* str = resolve_name_entry<C>( entry ) )
                return { ( const C* ) str->name, str->length / sizeof( C ) };
            else
                return ( const C* ) L"";
        }
    };

    template<bool x64> struct directory_type<directory_id::directory_entry_resource, x64, void> { using type = resource_directory_t; };
};
#pragma pack(pop)