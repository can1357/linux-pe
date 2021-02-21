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
#include <xmmintrin.h>
#include <algorithm>
#include <map>
#include "../../img_common.hpp"
#include "../data_directories.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    // Unwind opcodes.
    //
    enum class unwind_opcode : uint16_t // : 4
    {
        push_nonvol =               0x0,   // info == register number
        alloc_large =               0x1,   // no info, alloc size in next 2 slots
        alloc_small =               0x2,   // info == size of allocation / 8 - 1
        set_frame =                 0x3,   // no info, FP = RSP + UNWIND_INFO.FPRegOffset*16
        save_nonvol =               0x4,   // info == register number, offset in next slot
        save_nonvol_far =           0x5,   // info == register number, offset in next 2 slots
        epilog =                    0x6,   // (?)
        spare_code =                0x7,   // (?)
        save_xmm128 =               0x8,   // info == XMM reg number, offset in next slot
        save_xmm128_far =           0x9,   // info == XMM reg number, offset in next 2 slots
        push_machframe =            0xa,   // info == 0: no error-code, 1: error-code
    };

    // Unwind register identifiers.
    //
    enum class unwind_register_id : uint8_t // : 4
    {
        amd64_rax =                 0,           // GP
        amd64_rcx =                 1,
        amd64_rdx =                 2,
        amd64_rbx =                 3,
        amd64_rsp =                 4,
        amd64_rbp =                 5,
        amd64_rsi =                 6,
        amd64_rdi =                 7,
        amd64_r8 =                  8,
        amd64_r9 =                  9,
        amd64_r10 =                 10,
        amd64_r11 =                 11,
        amd64_r12 =                 12,
        amd64_r13 =                 13,
        amd64_r14 =                 14,
        amd64_r15 =                 15,
        amd64_eflags =              16,          // Fake GP entries used by our helper.
        amd64_rip =                 17,          // 
        amd64_seg_ss =              18,          // 
        amd64_seg_cs =              19,          //

        amd64_xmm0 =                24 + 0,      // XMM, has artifical offset added.
        amd64_xmm1 =                24 + 1,      //
        amd64_xmm2 =                24 + 2,      //
        amd64_xmm3 =                24 + 3,      //
        amd64_xmm4 =                24 + 4,      //
        amd64_xmm5 =                24 + 5,      //
        amd64_xmm6 =                24 + 6,      //
        amd64_xmm7 =                24 + 7,      //
        amd64_xmm8 =                24 + 8,      //
        amd64_xmm9 =                24 + 9,      //
        amd64_xmm10 =               24 + 10,     //
        amd64_xmm11 =               24 + 11,     //
        amd64_xmm12 =               24 + 12,     //
        amd64_xmm13 =               24 + 13,     //
        amd64_xmm14 =               24 + 14,     //
        amd64_xmm15 =               24 + 15,     //

        nat_amd64_xmm0 =            0,           // XMM, original range found in the native structure.
        nat_amd64_xmm1 =            1,           //
        nat_amd64_xmm2 =            2,           //
        nat_amd64_xmm3 =            3,           //
        nat_amd64_xmm4 =            4,           //
        nat_amd64_xmm5 =            5,           //
        nat_amd64_xmm6 =            6,           //
        nat_amd64_xmm7 =            7,           //
        nat_amd64_xmm8 =            8,           //
        nat_amd64_xmm9 =            9,           //
        nat_amd64_xmm10 =           10,          //
        nat_amd64_xmm11 =           11,          //
        nat_amd64_xmm12 =           12,          //
        nat_amd64_xmm13 =           13,          //
        nat_amd64_xmm14 =           14,          //
        nat_amd64_xmm15 =           15,          //
    };

    // Unwind code and info descriptors.
    //
    struct runtime_function_t;
    struct unwind_code_t
    {
        union
        {
            struct
            {
                uint16_t            code_offset    : 8;
                unwind_opcode       unwind_op      : 4;
                uint16_t            op_info        : 4;
            };
        };
    };
    struct unwind_info_t
    {
        uint8_t                     version        : 3;

        uint8_t                     ex_handler     : 1;
        uint8_t                     term_handler   : 1;
        uint8_t                     chained        : 1;
        uint8_t                     reserved_flags : 2;

        uint8_t                     size_prologue;
        uint8_t                     num_uw_codes;
        
        unwind_register_id          frame_register : 4;
        uint8_t                     frame_offset   : 4;
        
        unwind_code_t               unwind_code[ VAR_LEN ]; // # = align( num_uw_codes, 2 )

        // Getter for the scaled frame offset.
        //
        int64_t get_frame_offset() const { return ( int64_t ) ( ( ( uint32_t ) frame_offset ) * 16 ); }

        // Followed by rva of language specific information:
        //
        void* get_language_specific_data() { return &unwind_code[ ( num_uw_codes + 1 ) & ~1 ]; }
        const void* get_language_specific_data() const { return const_cast< unwind_info_t* >( this )->get_language_specific_data(); }

        uint32_t& exception_handler_rva() { return *( uint32_t* ) get_language_specific_data(); }
        const uint32_t& exception_handler_rva() const { return const_cast< unwind_info_t* >( this )->exception_handler_rva(); }

        runtime_function_t& chained_function_entry() { return *( runtime_function_t* ) get_language_specific_data(); }
        const runtime_function_t& chained_function_entry() const { return const_cast< unwind_info_t* >( this )->chained_function_entry(); }

        // Followed by optional exception data.
        //
        void* exception_specific_data() { return &exception_handler_rva() + 1; }
        const void* exception_specific_data() const { return const_cast< unwind_info_t* >( this )->exception_specific_data(); }
    };

    // High level descriptors of the opcodes.
    //
    struct amd64_unwind_code_t : unwind_code_t
    {
        // Returns the number of entries occupied by this opcode.
        //
        //size_t get_size() const = 0;

        // Applies or reverts this record's described side effect.
        //
        struct state_t
        {
            using reg_resolver_t =  void*(*)( void* ctx, unwind_register_id reg );
            using rmemcpy_t =       bool(*)( void* ctx, void* dst, uint64_t src, size_t n );
            using wmemcpy_t =       bool(*)( void* ctx, uint64_t dst, const void* src, size_t n );
            
			uint8_t                 frame_offset = 0;      // Information from the function entry.
            win::unwind_register_id frame_register = {};   //
            void*                   context =     nullptr; // User-defined context.
            reg_resolver_t          resolve_reg = nullptr; // Should get a pointer to the value of the register specified in the second argument.
            rmemcpy_t               rmemcpy =     nullptr; // Safe memory operations, if not set will use current process.
            wmemcpy_t               wmemcpy =     nullptr; //

            // Implement wrappers for some common operations.
            //
            uint64_t& gp( unwind_register_id gp_reg ) const { return *( uint64_t* ) resolve_reg( context, gp_reg ); }
            __m128& xmm( unwind_register_id xmm_reg ) const { return *( __m128* ) resolve_reg( context, xmm_reg ); }
            uint16_t& ss() const { return *( uint16_t* ) resolve_reg( context, unwind_register_id::amd64_seg_ss ); }
            uint16_t& cs() const { return *( uint16_t* ) resolve_reg( context, unwind_register_id::amd64_seg_cs ); }
            uint32_t& flags() const { return *( uint32_t* ) resolve_reg( context, unwind_register_id::amd64_eflags ); }

            uint64_t& sp() const { return gp( unwind_register_id::amd64_rsp ); }
            uint64_t& ip() const { return gp( unwind_register_id::amd64_rip ); }
            uint64_t& frame() const { return gp( frame_register ); }

            template<typename T>
            bool read( T& out, uint64_t address ) const
            {
                if ( rmemcpy ) return rmemcpy( context, &out, address, sizeof( T ) );
                if ( !address ) return false;
                memcpy( &out, ( const void* ) address, sizeof( T ) );
                return true;
            }
            template<typename T>
            bool write( uint64_t address, const T& data ) const
            {
                if ( wmemcpy ) return wmemcpy( context, address, &data, sizeof( T ) );
                if ( !address ) return false;
                memcpy( ( void* ) address, &data, sizeof( T ) );
                return true;
            }
        };
        //bool rewind( const state_t& state ) const = 0;
        //bool unwind( const state_t& state ) const = 0;
    };
    using amd64_unwind_state_t = amd64_unwind_code_t::state_t;
    struct amd64_unwind_set_frame_t : amd64_unwind_code_t
    {
        // Implement the interface.
        //
        size_t get_size() const { return 1; }
        bool rewind( const state_t& state ) const
        {
            state.frame() = state.sp() + ( size_t( state.frame_offset ) * 16 );
            return true;
        }
        bool unwind( const state_t& state ) const
        {
            state.sp() = state.frame() - ( size_t( state.frame_offset ) * 16 );
            return true;
        }
    };
    struct amd64_unwind_alloc_t : amd64_unwind_code_t
    {
        // Implement helpers.
        //
        size_t get_allocation_size() const 
        {  
            if ( unwind_op == unwind_opcode::alloc_small )
                return uint64_t( op_info ) * 8 + 8;
            if ( op_info ) return *( uint32_t* ) ( this + 1 );
            else           return 8 * ( uint64_t ) *( uint16_t* ) ( this + 1 );
        }

        // Implement the interface.
        //
        size_t get_size() const 
        {
            if ( unwind_op == unwind_opcode::alloc_small )
                return 1;
            return op_info ? 3 : 2; 
        }
        bool rewind( const state_t& state ) const
        {
            state.sp() -= get_allocation_size();
            return true;
        }
        bool unwind( const state_t& state ) const
        {
            state.sp() += get_allocation_size();
            return true;
        }
    };
    struct amd64_unwind_push_t : amd64_unwind_code_t
    {
        // Implement helpers.
        //
        unwind_register_id get_register() const { return ( unwind_register_id ) op_info; }

        // Implement the interface.
        //
        size_t get_size() const { return 1; }
        bool rewind( const state_t& state ) const
        {
            if ( !state.write( state.sp() - 8, state.gp( get_register() ) ) )
                return false;
            state.sp() -= 8;
            return true;
        }
        bool unwind( const state_t& state ) const
        {
            if ( !state.read( state.gp( get_register() ), state.sp() ) )
                return false;
            state.sp() += 8;
            return true;
        }
    };
    struct amd64_unwind_save_gp_t : amd64_unwind_code_t
    {
        // Implement helpers.
        //
        int64_t get_sp_offset() const
        {
            if ( unwind_op == unwind_opcode::save_nonvol_far )
                return ( int64_t ) ( *( uint32_t* ) ( this + 1 ) );
            else
                return ( int64_t ) ( 8 * ( uint64_t ) *( uint16_t* ) ( this + 1 ) );
        }
        unwind_register_id get_register() const { return ( unwind_register_id ) op_info; }

        // Implement the interface.
        //
        size_t get_size() const { return unwind_op == unwind_opcode::save_nonvol_far ? 3 : 2; }
        bool rewind( const state_t& state ) const
        {
            return state.write( state.sp() + get_sp_offset(), state.gp( get_register() ) );
        }
        bool unwind( const state_t& state ) const
        {
            return state.read( state.gp( get_register() ), state.sp() + get_sp_offset() );
        }
    };
    struct amd64_unwind_save_xmm_t : amd64_unwind_code_t
    {
        // Implement helpers.
        //
        int64_t get_sp_offset() const
        {
            if ( unwind_op == unwind_opcode::save_xmm128_far )
                return ( int64_t ) ( *( uint32_t* ) ( this + 1 ) );
            else
                return ( int64_t ) ( 16 * ( uint64_t ) *( uint16_t* ) ( this + 1 ) );
        }
        unwind_register_id get_register() const { return ( unwind_register_id ) ( size_t( unwind_register_id::amd64_xmm0 ) + op_info ); }

        // Implement the interface.
        //
        size_t get_size() const { return unwind_op == unwind_opcode::save_xmm128_far ? 3 : 2; }
        bool rewind( const state_t& state ) const
        {
            return state.write( state.sp() + get_sp_offset(), state.xmm( get_register() ) );
        }
        bool unwind( const state_t& state ) const
        {
            return state.read( state.xmm( get_register() ), state.sp() + get_sp_offset() );
        }
    };
    struct amd64_unwind_iframe_t : amd64_unwind_code_t
    {
        // Implement helpers.
        //
        bool has_exception_code() const { return op_info; }

        // Implement the interface.
        //
        size_t get_size() const { return 1; }
        bool rewind( [[maybe_unused]] const state_t& state ) const
        {
            return false;
        }
        bool unwind( const state_t& state ) const
        {
            size_t offset = 0;
            if ( has_exception_code() ) offset += 8;

            bool success =
                state.read( state.ip(), state.sp() + offset + 8 * 0 ) &&
                state.read( state.cs(), state.sp() + offset + 8 * 1 ) &&
                state.read( state.flags(), state.sp() + offset + 8 * 2 ) &&
                state.read( state.sp(), state.sp() + offset + 8 * 3 ) &&
                state.read( state.ss(), state.sp() + offset + 8 * 4 );
            if ( !success ) return false;
            state.sp() += offset + 8 * 4;
            return true;
        }
    };
    struct amd64_unwind_call_t
    {
        // Implement special unwind helper.
        //
        bool unwind( const amd64_unwind_code_t::state_t& state ) const
        {
            if ( !state.read( state.ip(), state.sp() ) )
                return false;
            state.sp() += 8;
            return true;
        }
    };
    struct amd64_unwind_nop_t : amd64_unwind_code_t
    {
        size_t get_size() const { return 1; }
        bool rewind( [[maybe_unused]] const state_t& state ) const { return false; }
        bool unwind( [[maybe_unused]] const state_t& state ) const { return true; }
    };


    template<typename T>
    static bool visit_amd64_unwind( const unwind_code_t& code, T&& visitor )
    {
        switch ( code.unwind_op )
        {
            // push r64
            case unwind_opcode::push_nonvol:     visitor( ( const amd64_unwind_push_t* ) &code );       break;
            // sub rsp, N
            case unwind_opcode::alloc_large:
            case unwind_opcode::alloc_small:     visitor( ( const amd64_unwind_alloc_t* ) &code );      break;
            // <reg> <= rsp + n
            case unwind_opcode::set_frame:       visitor( ( const amd64_unwind_set_frame_t* ) &code );  break;
            // mov [rsp/frame+N], gpreg
            case unwind_opcode::save_nonvol:
            case unwind_opcode::save_nonvol_far: visitor( ( const amd64_unwind_save_gp_t* ) &code );    break;
            // mov?ps [rsp/frame+N], xmmreg
            case unwind_opcode::save_xmm128:
            case unwind_opcode::save_xmm128_far: visitor( ( const amd64_unwind_save_xmm_t* ) &code );   break;
            // sw/hw int
            case unwind_opcode::push_machframe:  visitor( ( const amd64_unwind_iframe_t* ) &code );     break;
            // silently ignored in w10 2004
            case unwind_opcode::spare_code:
            case unwind_opcode::epilog:          visitor( ( const amd64_unwind_nop_t* ) &code );        break;
            // invalid, raises STATUS_BAD_FUNCTION_TABLE
            default:                             return false;
        }
        return true;
    }

    // Very commonly used language-specific data, C scope table.
    //
    struct c_scope_table_entry_t
    {
        uint32_t                    rva_begin;
        uint32_t                    rva_end;
        uint32_t                    rva_handler;
        uint32_t                    rva_target;
    };
    struct c_scope_table_t
    {
        uint32_t                    num_entries;       
        c_scope_table_entry_t       entries[ VAR_LEN ];
    };

    // Function table and the directory itself.
    //
    struct runtime_function_t
    {
        uint32_t                    rva_begin;
        uint32_t                    rva_end;
        union
        {
            uint32_t                unwind_info;
            uint32_t                rva_unwind_info;
        };
    };
    struct exception_directory_t
    {
        // Length of this array is determined by the size of the directory
        //
        runtime_function_t           functions[ VAR_LEN ];
    };
    template<bool x64> struct directory_type<directory_id::directory_entry_exception, x64, void> { using type = exception_directory_t; };

    // Wrapper around exception directory.
    //
    struct exception_directory
    {
        // std::map like traits.
        //
        struct key_compare
        {
            constexpr bool operator()( const runtime_function_t& a, const runtime_function_t& b ) const noexcept
            {
                return a.rva_begin < b.rva_begin;
            }
        };
        struct key_compare_end
        {
            constexpr bool operator()( const runtime_function_t& a, const runtime_function_t& b ) const noexcept
            {
                return a.rva_end < b.rva_end;
            }
        };
        using key_type =               uint32_t;
        using mapped_type =            runtime_function_t;
        using value_type =             runtime_function_t;
        using size_type =              size_t;
        using difference_type =        ptrdiff_t;
        using iterator =               const runtime_function_t*;
        using const_iterator =         const runtime_function_t*;
        using pointer =                const runtime_function_t*;
        using const_pointer =          const runtime_function_t*;
        using reference =              const runtime_function_t&;
        using const_reference =        const runtime_function_t&;
        using reverse_iterator =       std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // The range it's viewing.
        //
        const runtime_function_t* table;
        size_t length;

        // Constructed with a pointer to the data and the raw length in bytes.
        //
        exception_directory( const void* data, size_t length )
            : table( ( runtime_function_t* ) data ), length( length / sizeof( runtime_function_t ) ) {}

        // Make it iterable.
        //
        iterator begin() const { return table; }
        iterator end() const { return table + length; }
        reverse_iterator rbegin() const { return reverse_iterator( end() ); }
        reverse_iterator rend() const { return reverse_iterator( begin() ); }

        // Basic properties.
        //
        size_t size() const { return length; }
        bool empty() const { return length == 0; }

        // Binary-search lookup.
        //
        iterator lower_bound( uint32_t rva ) const
        {
            return std::lower_bound( begin(), end(), runtime_function_t{ .rva_end = rva }, key_compare_end{} );
        }
        iterator upper_bound( uint32_t rva ) const
        {
            return std::upper_bound( begin(), end(), runtime_function_t{ .rva_begin = rva }, key_compare{} );
        }
        std::pair<iterator, iterator> equal_range( uint32_t rva ) const
        {
            iterator low = lower_bound( rva );
            iterator high = std::upper_bound( low, end(), runtime_function_t{ .rva_end = rva }, key_compare_end{} );
            return { low, high };
        }

        // Finds a function using binary search.
        //
        iterator find_overlapping( uint32_t rva ) const
        {
            iterator it = lower_bound( rva );
            if ( it == end() || ( it->rva_begin <= rva && rva < it->rva_end ) )
                return it;
            else
                return end();
        }
        iterator find( uint32_t rva ) const
        {
            auto it = lower_bound( rva );
            if ( it != end() && it->rva_begin != rva )
                it = end();
            return it;
        }
        bool contains( uint32_t rva ) const
        {
            return find( rva ) != end();
        }
    };
};
#pragma pack(pop)