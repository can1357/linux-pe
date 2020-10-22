#pragma once
#include "common.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
	static constexpr uint32_t NUM_DATA_DIRECTORIES =	16;

	// Directory indices
	//
	enum directory_id
	{
		directory_entry_export =						0,				// Export Directory
		directory_entry_import =						1,				// Import Directory
		directory_entry_resource =						2,				// Resource Directory
		directory_entry_exception =						3,				// Exception Directory
		directory_entry_security =						4,				// Security Directory
		directory_entry_basereloc =						5,				// Base Relocation Table
		directory_entry_debug =							6,				// Debug Directory
		directory_entry_copyright =						7,				// (X86 usage)
		directory_entry_architecture =					7,				// Architecture Specific Data
		directory_entry_globalptr =						8,				// RVA of GP
		directory_entry_tls =							9,				// TLS Directory
		directory_entry_load_config =					10,				// Load Configuration Directory
		directory_entry_bound_import =					11,				// Bound Import Directory in headers
		directory_entry_iat =							12,				// Import Address Table
		directory_entry_delay_import =					13,				// Delay Load Import Descriptors
		directory_entry_com_descriptor =				14,				// COM Runtime descriptor
		directory_reserved0 =							15,				// -
	};

	// Generic directory descriptors
	//
	struct data_directory_t
	{
		uint32_t					rva;
		uint32_t					size;
		inline bool present() const { return size; }
	};
	struct raw_data_directory_t
	{
		uint32_t					ptr_raw_data;
		uint32_t					size;
		inline bool present() const { return size; }
	};

	// Data directories
	//
	struct data_directories_x86_t
	{
		union
		{
			struct
			{
				data_directory_t	  export_directory;
				data_directory_t	  import_directory;
				data_directory_t	  resource_directory;
				data_directory_t	  exception_directory;
				raw_data_directory_t  security_directory;  // File offset instead of virtual address!
				data_directory_t      basereloc_directory;
				data_directory_t      debug_directory;
				data_directory_t      copyright_directory;
				data_directory_t      globalptr_directory;
				data_directory_t      tls_directory;
				data_directory_t      load_config_directory;
				data_directory_t      bound_import_directory;
				data_directory_t      iat_directory;
				data_directory_t      delay_import_directory;
				data_directory_t      com_descriptor_directory;
				data_directory_t      _reserved0;
			};
			data_directory_t		  entries[ NUM_DATA_DIRECTORIES ];
		};
	};
	struct data_directories_x64_t
	{
		union
		{
			struct
			{
				data_directory_t      export_directory;
				data_directory_t      import_directory;
				data_directory_t      resource_directory;
				data_directory_t      exception_directory;
				raw_data_directory_t  security_directory;  // File offset instead of virtual address!!
				data_directory_t      basereloc_directory;
				data_directory_t      debug_directory;
				data_directory_t      architecture_directory;
				data_directory_t      globalptr_directory;
				data_directory_t      tls_directory;
				data_directory_t      load_config_directory;
				data_directory_t      bound_import_directory;
				data_directory_t      iat_directory;
				data_directory_t      delay_import_directory;
				data_directory_t      com_descriptor_directory;
				data_directory_t      _reserved0;
			};
			data_directory_t		entries[ NUM_DATA_DIRECTORIES ];
		};
	};
	template<bool x64 = default_architecture>
	using data_directories_t = std::conditional_t<x64, data_directories_x64_t, data_directories_x86_t>;
};
#pragma pack(pop)