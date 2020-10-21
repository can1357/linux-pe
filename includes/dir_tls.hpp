#pragma once
#include "nt_headers.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    union tls_characteristics_t
    {
        uint32_t flags;
        struct
        {
            uint32_t _reserved0     : 20;
            uint32_t alignment      : 4;
            uint32_t _reserved1     : 8;
        };
    };

    struct tls_directory_x64_t
    {
        uint64_t                    address_raw_data_start;
        uint64_t                    address_raw_data_end;
        uint64_t                    address_index;
        uint64_t                    address_callbacks;
        uint32_t                    size_zero_fill;
        tls_characteristics_t       characteristics;
    };

    struct tls_directory_x86_t
    {
        uint32_t                    address_raw_data_start;
        uint32_t                    address_raw_data_end;
        uint32_t                    address_index;
        uint32_t                    address_callbacks;
        uint32_t                    size_zero_fill;
        tls_characteristics_t       characteristics;
    };

    template<bool x64 = IS_DEF_AMD64>
    using tls_directory_t = std::conditional_t<x64, tls_directory_x64_t, tls_directory_x86_t>;
};
#pragma pack(pop)