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

// Contains COFF directory types.
//
#pragma pack(push, 1)
namespace win
{
    // Special section indices.
    //
    enum coff_special_section_id_t : int16_t
    {
        coff_symbol_undefined =   0,                           // External symbol
        coff_symbol_absolute =    -1,                          // Absolute symbol, non-relocatable value.
        coff_symbol_debug =       -2,                          // Misc. debugging info, not within a section.
    };

    // Storage class.
    //
    enum class coff_storage_class_t : uint8_t
    {
        none =                    0,                           // None.
        auto_variable =           1,                           // Automatic variable.
        public_symbol =           2,                           // Public symbol, global.
        private_symbol =          3,                           // Private symbol, static.
        register_variable =       4,                           // Register variable.
        external_definition =     5,                           // External definition.
        label =                   6,                           // Label.
        undefined_label =         7,                           // Undefined label.
        struct_member =           8,                           // Struct field.
        function_argument =       9,                           // Function argument.
        struct_tag =              10,                          // Struct tag.
        union_member =            11,                          // Union field.
        union_tag =               12,                          // Union tag.
        type_definition =         13,                          // Type definition.
        undefined_static =        14,                          // Undefined static.
        enum_tag =                15,                          // Enumerator tag.
        enum_member =             16,                          // Enumerator entry.
        register_parameter =      17,                          // Register parameter.
        bitfield =                18,                          // Marks a bitfield.
        auto_argument =           19,                          // Automatic argument.
        end_of_block =            20,                          // Marks the end of a block.
        block_delimitor =         100,                         // Marks the beginning or the end of a block.
        function_delimitor =      101,                         // Marks the beginning or the end of a function
        struct_end =              102,                         // Marks the end of a structure definition.
        file_name =               103,                         // File name as symbol.
        line_number =             104,                         // a) Line number as symbol.
        section =                 104,                         // b) Section entry.
        alias_entry =             105,                         // a) Alias to another entry.
        weak_external =           105,                         // b) Weak external.
        hidden_ext_symbol =       106,                         // Hidden external symbol.
        clr_token =               107,                         // CLR token.
        phys_end_of_function =    255,                         // Marks physical end of a function.
    };

    // Type identifiers.
    //
    enum class coff_base_type_t : uint16_t
    {
        t_none =                  0,
        t_void =                  1,
        t_char =                  2,
        t_short =                 3,
        t_int =                   4,
        t_long =                  5,
        t_float =                 6,
        t_double =                7,
        t_struct =                8,
        t_union =                 9,
        t_enum =                  10,
        t_enum_mem =              11,
        t_uchar =                 12,
        t_ushort =                13,
        t_uint =                  14,
        t_ulong =                 15,
    };
    enum class coff_derived_type_t : uint16_t
    {
        none =                    0,                           // Not derived.
        pointer =                 1,                           // Pointer to base type.
        function =                2,                           // Function returning base type.
        c_array =                 3,                           // Array of base type.
    };
    enum class coff_rel_type_t : uint16_t
    {
        // AMD64:
        //
        coff_rel_amd64_absolute = 0x0000,                      // The relocation is ignored.
        coff_rel_amd64_addr64 =   0x0001,                      // The 64-bit VA of the relocation target.
        coff_rel_amd64_addr32 =   0x0002,                      // The 32-bit VA of the relocation target.
        coff_rel_amd64_addr32nb = 0x0003,                      // The 32-bit address without an image base (RVA).
        coff_rel_amd64_rel32 =    0x0004,                      // The 32-bit relative address from the byte following the relocation.
        coff_rel_amd64_rel32_1 =  0x0005,                      // The 32-bit address relative to byte distance 1 from the relocation.
        coff_rel_amd64_rel32_2 =  0x0006,                      // The 32-bit address relative to byte distance 2 from the relocation.
        coff_rel_amd64_rel32_3 =  0x0007,                      // The 32-bit address relative to byte distance 3 from the relocation.
        coff_rel_amd64_rel32_4 =  0x0008,                      // The 32-bit address relative to byte distance 4 from the relocation.
        coff_rel_amd64_rel32_5 =  0x0009,                      // The 32-bit address relative to byte distance 5 from the relocation.
        coff_rel_amd64_section =  0x000A,                      // The 16-bit section index of the section that contains the target. This is used to support debugging information.
        coff_rel_amd64_secrel =   0x000B,                      // The 32-bit offset of the target from the beginning of its section. This is used to support debugging information and static thread local storage.
        coff_rel_amd64_secrel7 =  0x000C,                      // A 7-bit unsigned offset from the base of the section that contains the target.
        coff_rel_amd64_token =    0x000D,                      // CLR tokens.
        coff_rel_amd64_srel32 =   0x000E,                      // A 32-bit signed span-dependent value emitted into the object.
        coff_rel_amd64_pair =     0x000F,                      // A pair that must immediately follow every span-dependent value.
        coff_rel_amd64_sspan32 =  0x0010,                      // A 32-bit signed span-dependent value that is applied at link time.

        // I386:
        //
        coff_rel_i386_absolute =  0x0000,                      // The relocation is ignored.
        coff_rel_i386_dir16 =     0x0001,                      // Not supported.
        coff_rel_i386_rel16 =     0x0002,                      // Not supported.
        coff_rel_i386_dir32 =     0x0006,                      // The target's 32-bit VA.
        coff_rel_i386_dir32nb =   0x0007,                      // The target's 32-bit RVA.
        coff_rel_i386_seg12 =     0x0009,                      // Not supported.
        coff_rel_i386_section =   0x000A,                      // The 16-bit section index of the section that contains the target. This is used to support debugging information.
        coff_rel_i386_secrel =    0x000B,                      // The 32-bit offset of the target from the beginning of its section. This is used to support debugging information and static thread local storage.
        coff_rel_i386_token =     0x000C,                      // The CLR token.
        coff_rel_i386_secrel7 =   0x000D,                      // A 7-bit offset from the base of the section that contains the target.
        coff_rel_i386_rel32 =     0x0014,                      // The 32-bit relative displacement to the target. This supports the x86 relative branch and call instructions.
    };

    // Relocation entry.
    //
    struct coff_reloc_t
    {
        uint32_t                  virtual_address;             // Virtual address of the relocated data.
        uint32_t                  symbol_index;                // Symbol index.
        coff_rel_type_t           type;                        // Type of the relocation applied.
    };

    // Line number entry.
    //
    struct coff_line_number_t
    {
        union
        {
            uint32_t              symbol_index;                // If first entry at line #0, symbol index to the function.
            uint32_t              ptr_raw_data;                // Else, raw offset from the beginning of the directory that this data relates to.
        };
        uint16_t                  line_number;                 // Line number.
    };

    // Symbol table entry.
    //
    struct coff_symbol_t
    {
        union
        {
            char                 short_name[ LEN_SHORT_STR ];  // Name as inlined string.
            struct
            {
                uint32_t         is_short;                     // If non-zero, name is inline'd into short_name, else has a long name.
                uint32_t         long_name_offset;             // Offset into string table.
            };
        };
        int32_t                  value;                        // Value associated with the symbol, interp. depends on the type, usually address of the entry.
        int16_t                  section_index;                // If <= 0, a speccial descriptor else scn+1.

        coff_base_type_t         base_type    : 4;             // Base and derived type describing the symbol.
        coff_derived_type_t      derived_type : 12;            //

        coff_storage_class_t     storage_class;                // Storage class as described above.
        uint8_t                  num_auxiliary;                // Auxiliary data following this symbol.
    };
    static_assert( sizeof( coff_symbol_t ) == 18, "Invalid enum bitfield." );

    // String table.
    //
    struct coff_string_table_t
    {
        uint32_t                 size;
        char                     raw_data[ VAR_LEN ];
    };
};
#pragma pack(pop)