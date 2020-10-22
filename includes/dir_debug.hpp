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
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    enum class debug_directory_type_id : uint32_t
    {
        unknown =       0x00000000,
        coff =          0x00000001,
        codeview =      0x00000002,
        fpo =           0x00000003,
        misc =          0x00000004,
        exception =     0x00000005,
        fixup =         0x00000006,
        omap_to_src =   0x00000007,
        omap_from_src = 0x00000008,
        borland =       0x00000009,
        reserved10 =    0x0000000A,
        clsid =         0x0000000B,
        vc_feature =    0x0000000C,
        pogo =          0x0000000D,
        iltcg =         0x0000000E,
        mpx =           0x0000000F,
        repro =         0x00000010,
    };

    struct debug_directory_entry_t
    {
        uint32_t                    characteristics;
        uint32_t					timedate_stamp;
        ex_version_t                version;
        debug_directory_type_id     type;
        uint32_t                    size_raw_data;
        uint32_t                    rva_raw_data;
        uint32_t                    ptr_raw_data;
    };

    struct debug_directory_t
    {
        debug_directory_entry_t     entries[ VAR_LEN ];
    };
};
#pragma pack(pop)