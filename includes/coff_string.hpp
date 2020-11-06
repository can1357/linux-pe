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
#include <string_view>
#include "common.hpp"

#pragma pack(push, COFF_STRUCT_PACKING)
namespace coff
{
    // String table.
    //
    struct string_table_t
    {
        uint32_t                 size;
        char                     raw_data[ VAR_LEN ];
    };

    // External reference to string table.
    //
    union string_t
    {
        char                     short_name[ LEN_SHORT_STR ];  // Name as inlined string.
        struct
        {
            uint32_t             is_short;                     // If non-zero, name is inline'd into short_name, else has a long name.
            uint32_t             long_name_offset;             // Offset into string table.
        };

        // Comparison with a short string.
        //
        template<size_t N> requires( N <= ( LEN_SHORT_STR + 1 ) )
        int short_cmp( const char( &str )[ N ] ) const
        {
            if ( N == ( LEN_SHORT_STR + 1 ) )
                return memcmp( short_name, str, LEN_SHORT_STR );
            else
                return memcmp( short_name, str, N );
        }

        // Convert to string view given the string table.
        //
        std::string_view to_string( const string_table_t* tbl ) const
        {
            if ( is_short )
            {
                if ( short_name[ LEN_SHORT_STR - 1 ] ) return { short_name, LEN_SHORT_STR };
                return short_name;
            }
            else
            {
                if ( tbl ) return ( ( const char* ) tbl ) + long_name_offset;
                return {};
            }
        }
    };
};
#pragma pack(pop)