#pragma once
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    enum class reloc_type_id : uint16_t
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
        reloc_type_id               type    : 4;
    };
    static_assert( sizeof( reloc_entry_t ) == 2, "Enum bitfield is not supported." );

    struct reloc_block_t
    {
        uint32_t                    base_rva;
        uint32_t                    size_block;
        reloc_entry_t               entries[ VAR_LEN ];

        inline reloc_block_t* get_next() { return ( reloc_block_t* ) ( ( char* ) this + this->size_block ); }
        inline const reloc_block_t* get_next() const { return const_cast< reloc_block_t* >( this )->get_next(); }
        inline size_t num_entries() const { return ( reloc_entry_t* ) get_next() - &entries[ 0 ]; }
    };

    struct reloc_directory_t
    {
        reloc_block_t               first_block;
    };
};
#pragma pack(pop)