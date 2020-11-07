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
#include "../../img_common.hpp"
#include "../data_directories.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    // Unwind information.
    //
    enum class unwind_opcode : uint16_t // : 4
    {
        push_nonvol =     0x0, /* info == register number */
        alloc_large =     0x1, /* no info, alloc size in next 2 slots */
        alloc_small =     0x2, /* info == size of allocation / 8 - 1 */
        set_fpreg =       0x3, /* no info, FP = RSP + UNWIND_INFO.FPRegOffset*16 */
        save_nonvol =     0x4, /* info == register number, offset in next slot */
        save_nonvol_far = 0x5, /* info == register number, offset in next 2 slots */
        epilog =          0x6,
        spare_code =      0x7,
        save_xmm128 =     0x8, /* info == XMM reg number, offset in next slot */
        save_xmm128_far = 0x9, /* info == XMM reg number, offset in next 2 slots */
        push_machframe =  0xa, /* info == 0: no error-code, 1: error-code */
    };
    struct unwind_code_t
    {
        union
        {
            struct
            {
                uint16_t            code_offset    : 8;
                unwind_opcode       unwind_op      : 4;
                uint16_t            op_info        : 4;
            };                      
            struct                  
            {                       
                uint16_t            frame_offset;
            };                      
            struct                  
            {                       
                uint16_t            offset_lo      : 8;
                uint16_t            unwind_op      : 4;
                uint16_t            offset_hi      : 4;
            } epilogue_code;
        };
    };
    struct unwind_info_t
    {
        uint8_t                     version        : 3;
        uint8_t                     flags          : 5;
        uint8_t                     size_prolog;
        uint8_t                     num_uw_codes;
        uint8_t                     frame_register : 4;
        uint8_t                     frame_offset   : 4;
        unwind_code_t               unwind_code[ VAR_LEN ]; // # = align( num_uw_codes, 2 )

        // Followed by rva of language specific information:
        //
        uint32_t& exception_handler_rva() { return *( uint32_t* ) &unwind_code[ ( num_uw_codes + 1 ) & ~1 ]; }
        uint32_t& function_entry_rva() { return exception_handler_rva(); }
        const uint32_t& exception_handler_rva() const { return const_cast< unwind_info_t* >( this )->exception_handler_rva(); }
        const uint32_t& function_entry_rva() const { return exception_handler_rva(); }

        // Followed by optional exception data.
        //
        void* exception_data() { return &exception_handler_rva() + 1; }
        const void* exception_data() const { return &exception_handler_rva() + 1; }
    };

    // Function table.
    //
    struct runtime_function_t
    {
        uint32_t                    rva_begin;
        uint32_t                    rva_end;
        union
        {
            uint32_t                unwind_info;
            uint32_t                rva_unwind_info;
        };
    };
    struct exception_directory_t
    {
        // Length of this array is determined by the size of the directory
        //
        runtime_function_t           functions[ VAR_LEN ];
    };

    template<bool x64> struct directory_type<directory_id::directory_entry_exception, x64, void> { using type = exception_directory_t; };
};
#pragma pack(pop)