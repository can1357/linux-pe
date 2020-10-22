#pragma once
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    struct image_named_import_t
    {
        uint16_t            hint;
        char                name[ 1 ];
    };

    #pragma pack(push, 8)
    struct image_thunk_data_x64_t
    {
        union
        {
            uint64_t        forwarder_string;
            uint64_t        function;
            uint64_t        address;                   // -> image_named_import_t
            struct
            {
                uint64_t    ordinal     : 16;
                uint64_t    _reserved0  : 47;
                uint64_t    is_ordinal  : 1;
            };
        };
    };
    #pragma pack(pop)

    struct image_thunk_data_x86_t
    {
        union
        {
            uint32_t        forwarder_string;
            uint32_t        function;
            uint32_t        address;                   // -> image_named_import_t
            struct
            {
                uint32_t    ordinal     : 16;
                uint32_t    _reserved0  : 15;
                uint32_t    is_ordinal  : 1;
            };
        };
    };

    struct bound_forwarder_ref_t
    {
        uint32_t                    timedate_stamp;
        uint16_t                    offset_module_name;
    };

    struct bound_import_descriptor_t
    {
        uint32_t                    timedate_stamp;
        uint16_t                    offset_module_name;
        uint16_t                    num_module_forwarder_refs;
    };

    template<bool x64 = default_architecture>
    using image_thunk_data_t = std::conditional_t<x64, image_thunk_data_x64_t, image_thunk_data_x86_t>;
};
#pragma pack(pop)