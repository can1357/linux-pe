#pragma once
#include "nt_headers.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    struct load_config_ci_t
    {
        uint16_t                    flags;                              // Flags to indicate if CI information is available, etc.
        uint16_t                    catalog;                            // 0xFFFF means not available
        uint32_t                    rva_catalog;
        uint32_t                    _pad0;                              // Additional bitmask to be defined later
    };

    struct load_config_directory_x64_t
    {
        uint32_t                    size;
        uint32_t                    timedate_stamp;
        ex_version_t                version;
        uint32_t                    global_flags_clear;
        uint32_t                    global_flags_set;
        uint32_t                    critical_section_default_timeout;
        uint64_t                    decommit_free_block_threshold;
        uint64_t                    decommit_total_free_threshold;
        uint64_t                    lock_prefix_table;
        uint64_t                    maximum_allocation_size;
        uint64_t                    virtual_memory_threshold;
        uint64_t                    process_affinity_mask;
        uint32_t                    process_heap_flags;
        uint16_t                    csd_version;
        uint16_t                    dependent_load_flags;
        uint64_t                    edit_list;
        uint64_t                    security_cookie;
        uint64_t                    se_handler_table;
        uint64_t                    se_handler_count;
        uint64_t                    guard_cf_check_function_ptr;
        uint64_t                    guard_cf_dispatch_function_ptr;
        uint64_t                    guard_cf_function_table;
        uint64_t                    guard_cf_function_count;
        uint32_t                    guard_flags;
        load_config_ci_t            code_integrity;
        uint64_t                    guard_address_taken_iat_entry_table;
        uint64_t                    guard_address_taken_iat_entry_count;
        uint64_t                    guard_long_jump_target_table;
        uint64_t                    guard_long_jump_target_count;
        uint64_t                    dynamic_value_reloc_table;
        uint64_t                    chpe_metadata_ptr;
        uint64_t                    guard_rf_failure_routine;
        uint64_t                    guard_rf_failure_routine_function_ptr;
        uint32_t                    dynamic_value_reloc_table_offset;
        uint16_t                    dynamic_value_reloc_table_section;
        uint16_t                    _pad0;
        uint64_t                    guard_rf_verify_stack_ptr_function_ptr;
        uint32_t                    hotpatch_table_offset;
        uint32_t                    _pad1;
        uint64_t                    enclave_configuration_ptr;
        uint64_t                    volatile_metadata_ptr;
    };

    struct load_config_directory_x86_t
    {
        uint32_t                    size;
        uint32_t                    timedate_stamp;
        ex_version_t                version;
        uint32_t                    global_flags_clear;
        uint32_t                    global_flags_set;
        uint32_t                    critical_section_default_timeout;
        uint32_t                    decommit_free_block_threshold;
        uint32_t                    decommit_total_free_threshold;
        uint32_t                    lock_prefix_table;
        uint32_t                    maximum_allocation_size;
        uint32_t                    virtual_memory_threshold;
        uint32_t                    process_heap_flags;
        uint32_t                    process_affinity_mask;
        uint16_t                    csd_version;
        uint16_t                    _pad0;
        uint32_t                    edit_list;
        uint32_t                    security_cookie;
        uint32_t                    se_handler_table;
        uint32_t                    se_handler_count;
    };

    template<bool x64 = IS_DEF_AMD64,
        typename base_type = typename std::conditional<x64, load_config_directory_x64_t, load_config_directory_x86_t>::type>
        struct load_config_directory_t : base_type {};
    static_assert( sizeof( load_config_directory_t<false> ) == sizeof( load_config_directory_x86_t ) &&
                   sizeof( load_config_directory_t<true> ) == sizeof( load_config_directory_x64_t ),
                   "Empty structure influenced structure size." );
};
#pragma pack(pop)