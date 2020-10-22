#pragma once
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
	// Magic numbers
	//
	static constexpr uint32_t LEN_SECTION_NAME =		8;

	// Section characteristics
	//
	union section_characteristics_t
	{
		uint32_t flags;
		struct
		{
			uint32_t _pad0								: 5;
			uint32_t cnt_code							: 1;			// Section contains code.
			uint32_t cnt_init_data						: 1;			// Section contains initialized data.
			uint32_t cnt_uninit_data					: 1;			// Section contains uninitialized data.
			uint32_t _pad1								: 1;
			uint32_t lnk_info							: 1;			// Section contains comments or some other type of information.
			uint32_t _pad2								: 1;
			uint32_t lnk_remove							: 1;			// Section contents will not become part of image.
			uint32_t lnk_comdat							: 1;			// Section contents comdat.
			uint32_t _pad3								: 1;
			uint32_t no_defer_spec_exc					: 1;			// Reset speculative exceptions handling bits in the TLB entries for this section.
			uint32_t mem_far							: 1;
			uint32_t _pad4								: 1;
			uint32_t mem_purgeable						: 1;
			uint32_t mem_locked							: 1;
			uint32_t mem_preload						: 1;
			uint32_t alignment							: 4;			// Alignment calculated as: n ? 1 << ( n - 1 ) : 16 
			uint32_t lnk_nreloc_ovfl					: 1;			// Section contains extended relocations.
			uint32_t mem_discardable					: 1;			// Section can be discarded.
			uint32_t mem_not_cached						: 1;			// Section is not cachable.
			uint32_t mem_not_paged						: 1;			// Section is not pageable.
			uint32_t mem_shared							: 1;			// Section is shareable.
			uint32_t mem_execute						: 1;			// Section is executable.
			uint32_t mem_read							: 1;			// Section is readable.
			uint32_t mem_write							: 1;			// Section is writeable.
		};
		inline uint32_t get_alignment() const { return alignment ? 1 << ( alignment - 1 ) : 0x10; }
	};

	// Section header
	//
	struct section_header_t
	{
		char						name[ LEN_SECTION_NAME ];
		
		union
		{
			uint32_t				physical_address;
			uint32_t				virtual_size;
		};
		uint32_t					virtual_address;
		
		uint32_t					size_raw_data;
		uint32_t					ptr_raw_data;
		
		uint32_t					ptr_relocs;
		uint32_t					ptr_line_numbers;
		uint16_t					num_relocs;
		uint16_t					num_line_numbers;
		
		section_characteristics_t	characteristics;
	};
};
#pragma pack(pop)