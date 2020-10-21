#pragma once
#include "nt_headers.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    enum class certificate_type_id : uint16_t
    {
        x509 =             0x0001,
        pkcs_signed_data = 0x0002,
        reserved_1 =       0x0003,
        ts_stack_signed =  0x0004,
    };

    struct security_directory_t
    {
        uint32_t             length;
        version_t            revision;
        certificate_type_id  certificate_type;
        uint8_t              raw_data[];
    };
};
#pragma pack(pop)