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
#include <vector>
#include <iterator>
#include <string_view>
#include <cstring>
#include <stdlib.h>
#include <type_traits>
#include "../img_common.hpp"

#pragma pack(push, COFF_STRUCT_PACKING)
namespace ar
{
	// Magic constants.
	//
	constexpr uint64_t format_magic =     0x0A3E686372613C21; // "!<arch>\n"
	constexpr uint16_t entry_terminator = 0x0A60;             // "`\n"

	// Stupid string integers.
	//
	template<size_t B, size_t N>
	struct string_integer
	{
		char string[ N ];

		// Integer to string.
		//
		string_integer( uint64_t integer )
		{
			static constexpr char dictionary[] = "0123456789ABCDEF";
			char* it = std::end( string );
			if ( !integer ) *--it = '0';
			while ( integer )
			{
				*--it = dictionary[ integer % B ];
				integer /= B;
				if ( it == std::begin( string ) )
					throw std::overflow_error( "Integer does not fit." );
			}

			size_t len = std::end( string ) - it;
			memmove( string, it, len );
			memset( string + len, ' ', N - len );
		}
		string_integer( const string_integer& o ) = default;

		// String to integer.
		//
		uint64_t get() const
		{
			char* it = ( char* ) string;
			while ( it != std::end( string ) && *it && *it != ' ' )
				++it;
			return strtoull( string, &it, B );
		}
		operator uint64_t() const { return get(); }
	};

	// File entry.
	//
	struct entry_t
	{
		union
		{
			char                        identifier[ 16 ];
			struct
			{
				char                    _pad;
				string_integer<10, 15>  long_string_offset;
			};
		};
		string_integer<10, 12>          modify_timestamp;
		string_integer<10, 6>           owner_id;
		string_integer<10, 6>           group_id;
		string_integer<8,  8>           mode;
		string_integer<10, 10>          length;
		uint16_t                        terminator;

		// Data getter.
		//
		uint8_t* data() { return ( uint8_t* ) ( this + 1 ); };
		const uint8_t* data() const { return ( const uint8_t* ) ( this + 1 ); };
		uint8_t* begin() { return data(); };
		const uint8_t* begin() const { return data(); };
		uint8_t* end() { return data() + length; };
		const uint8_t* end() const { return data() + length; };

		// Forward to next.
		//
		entry_t* next() { return ( entry_t* ) ( data() + ( ( length + 1 ) & ~1 ) ); }
		const entry_t* next() const { return ( const entry_t* ) ( data() + ( ( length + 1 ) & ~1 ) ); }

		// System-V extension properties.
		//
		bool is_symbol_table() const { return identifier[ 0 ] == '/' && identifier[ 1 ] == ' '; }
		bool is_string_table() const { return identifier[ 0 ] == '/' && identifier[ 1 ] == '/' && identifier[ 2 ] == ' '; }
		bool has_long_name() const { return identifier[ 0 ] == '/' && '0' <= identifier[ 1 ] && identifier[ 1 ] <= '9'; }

		// Convert identifier to string view.
		//
		std::string_view to_string( const entry_t* string_table = nullptr ) const
		{
			const char* begin = std::begin( identifier );
			const char* end = std::end( identifier );
			char terminator = '//';

			if ( has_long_name() )
			{
				begin = ( const char* ) string_table->begin() + long_string_offset;
				end = ( const char* ) string_table->end();
				terminator = '\n';
				if ( begin >= end ) return {};
			}

			auto it = begin;
			while ( it != end && *it != terminator ) it++;
			return { begin, ( size_t ) ( it - begin ) };
		}
	};

	// Archive header.
	//
	struct header_t
	{
		uint64_t               magic;
		entry_t                first_entry;
	};

	// Archive view wrapping the AR format.
	//
	template<bool constant>
	struct view
	{
		// Typedefs.
		//
		using archive_type = std::conditional_t<constant, const header_t, header_t>;
		using entry_type =   std::conditional_t<constant, const entry_t, entry_t>;

		// Declare iterator.
		//
		struct iterator
		{
			// Declare iterator traits
			//
			using iterator_category =   std::forward_iterator_tag;
			using difference_type =     int64_t;
			using reference =           std::pair<std::string_view, entry_type&>;
			using pointer =             const void*;

			// Stores current location, the limit and the string table.
			//
			entry_type*                 str_table;
			entry_type*                 at;
			const void*                 limit;

			// Implement forward iteration and basic comparison.
			//
			iterator& operator++() { at = at->next(); return *this; }
			iterator operator++( int ) { auto s = *this; operator++(); return s; }
			bool operator==( const iterator& other ) const 
			{
				if ( !at )       return !other.at || other.at >= other.limit || other.at->terminator != entry_terminator;
				if ( !other.at ) return !at || at >= limit || at->terminator != entry_terminator;
				else             return at == other.at;
			}
			bool operator<( const iterator& other ) const
			{
				if ( !at )       return false;
				if ( !other.at ) return !operator==( other );
				else             return at < other.at;
			}
			bool operator!=( const iterator& other ) const { return !operator==( other ); }

			// Implement iterator interface.
			//
			inline reference operator*() const { return reference{ at->to_string( str_table ), *at }; }
			inline entry_type* operator->() const { return at; }
		};
		using const_iterator = iterator;

		// Holds the archive header and the region limit.
		//
		archive_type*            archive;
		const void*              limit;

		// Holds special tables discovered during construction.
		//
		entry_type*              string_table = nullptr;
		std::vector<entry_type*> symbol_tables = {};
		entry_type*              first_entry = nullptr;

		// Constructed by pointer and the region size.
		//
		view( const void* _archive, size_t size ) : archive( ( archive_type* ) _archive ), limit( ( uint8_t* ) _archive + size )
		{
			// Validate magic.
			//
			if ( archive->magic != format_magic )
				limit = &archive->first_entry;

			// Resolve special entries and assign the first non-special entry.
			//
			for ( entry_type* it = &archive->first_entry; it < limit; it = it->next() )
			{
				if ( it->is_string_table() && !string_table )
				{
					string_table = it;
				}
				else if ( it->is_symbol_table() )
				{
					symbol_tables.emplace_back( it );
				}
				else
				{
					first_entry = it;
					break;
				}
			}
		}

		// Make iterable.
		//
		iterator begin( bool long_strings = true ) const { return { string_table, first_entry, limit }; }
		iterator end() const { return { nullptr, nullptr, nullptr }; }
	};
	template<typename T> view( T*, size_t ) -> view<std::is_const_v<T>>;
};
#pragma pack(pop)