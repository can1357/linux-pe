#pragma once
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    union delay_load_attributes_t
    {
        uint32_t flags;
        struct
        {
            uint32_t rva_based : 1;
            uint32_t reserved  : 31;
        };
    };

    struct delay_load_directory_t
    {
        delay_load_attributes_t          attributes;
        uint32_t                         dll_name_rva;
        uint32_t                         module_handle_rva;
        uint32_t                         import_address_table_rva;
        uint32_t                         import_name_table_rva;
        uint32_t                         bound_import_address_table_rva;
        uint32_t                         unload_information_table_rva;
        uint32_t                         time_date_stamp;
    };
};
#pragma pack(pop)