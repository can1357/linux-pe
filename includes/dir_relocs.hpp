#pragma once
#include "nt_headers.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    // Not "enum class" to ease the casting to uint16_t:4
    enum reloc_type_id 
    {
        rel_based_absolute =          0,
        rel_based_high =              1,
        rel_based_low =               2,
        rel_based_high_low =          3,
        rel_based_high_adj =          4,
        rel_based_ia64_imm64 =        9,
        rel_based_dir64 =             10,
    };

    struct reloc_entry_t
    {
        uint16_t                    offset  : 12;
        uint16_t                    type    : 4;
    };

    struct reloc_directory_t
    {
        uint32_t                    rva;
        uint32_t                    size_block;

        inline reloc_entry_t* get_entries() { return ( reloc_entry_t* ) ( this + 1 ); }
        inline uint32_t num_entries() { return ( size_block - sizeof( reloc_directory_t ) ) / sizeof( reloc_entry_t ); }
    };
};
#pragma pack(pop)