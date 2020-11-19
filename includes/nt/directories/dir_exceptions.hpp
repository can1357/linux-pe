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
#include "../../img_common.hpp"
#include "../data_directories.hpp"

#pragma pack(push, WIN_STRUCT_PACKING)
namespace win
{
    // Unwind opcodes.
    //
    enum class unwind_opcode : uint16_t // : 4
    {
        push_nonvol =     0x0, /* info == register number */
        alloc_large =     0x1, /* no info, alloc size in next 2 slots */
        alloc_small =     0x2, /* info == size of allocation / 8 - 1 */
        set_frame =       0x3, /* no info, FP = RSP + UNWIND_INFO.FPRegOffset*16 */
        save_nonvol =     0x4, /* info == register number, offset in next slot */
        save_nonvol_far = 0x5, /* info == register number, offset in next 2 slots */
        epilog =          0x6,
        spare_code =      0x7,
        save_xmm128 =     0x8, /* info == XMM reg number, offset in next slot */
        save_xmm128_far = 0x9, /* info == XMM reg number, offset in next 2 slots */
        push_machframe =  0xa, /* info == 0: no error-code, 1: error-code */
    };

    // Unwind register identifiers.
    //
    enum class unwind_register_id : uint8_t // : 4
    {
        amd64_rax =       0,      // GP
        amd64_rcx =       1,
        amd64_rdx =       2,
        amd64_rbx =       3,
        amd64_rsp =       4,
        amd64_rbp =       5,
        amd64_rsi =       6,
        amd64_rdi =       7,
        amd64_r8 =        8,
        amd64_r9 =        9,
        amd64_r10 =       10,
        amd64_r11 =       11,
        amd64_r12 =       12,
        amd64_r13 =       13,
        amd64_r14 =       14,
        amd64_r15 =       15,
        amd64_eflags =    16,     // Fake GP entries used by our helper.
        amd64_rip =       17,     // 
        amd64_seg_ss =    18,     // 
        amd64_seg_cs =    19,     // 
        
        amd64_xmm0 =      0,      // XMM
        amd64_xmm1 =      1,
        amd64_xmm2 =      2,
        amd64_xmm3 =      3,
        amd64_xmm4 =      4,
        amd64_xmm5 =      5,
        amd64_xmm6 =      6,
        amd64_xmm7 =      7,
        amd64_xmm8 =      8,
        amd64_xmm9 =      9,
        amd64_xmm10 =     10,
        amd64_xmm11 =     11,
        amd64_xmm12 =     12,
        amd64_xmm13 =     13,
        amd64_xmm14 =     14,
        amd64_xmm15 =     15,
    };

    // Unwind code and info descriptors.
    //
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

        uint8_t                     size_prolog;
        uint8_t                     num_uw_codes;
        
        unwind_register_id          frame_register : 4;
        uint8_t                     frame_offset   : 4;
        
        unwind_code_t               unwind_code[ VAR_LEN ]; // # = align( num_uw_codes, 2 )

        // Getter for the scaled frame offset.
        //
        size_t get_frame_offset() const { return frame_offset * 16; }

        // Followed by rva of language specific information:
        //
        uint32_t& exception_handler_rva() { return *( uint32_t* ) &unwind_code[ ( num_uw_codes + 1 ) & ~1 ]; }
        uint32_t& function_entry_rva() { return exception_handler_rva(); }
        const uint32_t& exception_handler_rva() const { return const_cast< unwind_info_t* >( this )->exception_handler_rva(); }
        const uint32_t& function_entry_rva() const { return exception_handler_rva(); }

        // Followed by optional exception data.
        //
        void* exception_data() { return &exception_handler_rva() + 1; }
        const void* exception_data() const { return &exception_handler_rva() + 1; }
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
            using xmm_resolver_t = void*(*)( void* ctx, unwind_register_id reg );
            using gp_resolver_t =  void*(*)( void* ctx, unwind_register_id reg );
            using rmemcpy_t =      bool(*)( void* dst, uint64_t src, size_t n );
            using wmemcpy_t =      bool(*)( uint64_t dst, const void* src, size_t n );

            const unwind_info_t*  owner =       nullptr; // Pointer to parent.
            void*                 context =     nullptr; // User-defined context.
            gp_resolver_t         resolve_gp =  nullptr; // Should get a reference to the GP register given in the second argument.
            xmm_resolver_t        resolve_xmm = nullptr; // Should get a reference to the XMM register given in the second argument.
            rmemcpy_t             rmemcpy =     nullptr; // Safe memory operations, if not set will use current process.
            wmemcpy_t             wmemcpy =     nullptr; //

            // Implement wrappers for some common operations.
            //
            uint64_t& gp( unwind_register_id gp_reg ) const { return *( uint64_t* ) resolve_gp( context, gp_reg ); }
            __m128& xmm( unwind_register_id xmm_reg ) const { return *( __m128* ) resolve_xmm( context, xmm_reg ); }
            uint16_t& ss() const { return *( uint16_t* ) resolve_gp( context, unwind_register_id::amd64_seg_ss ); }
            uint16_t& cs() const { return *( uint16_t* ) resolve_gp( context, unwind_register_id::amd64_seg_cs ); }
            uint32_t& flags() const { return *( uint32_t* ) resolve_gp( context, unwind_register_id::amd64_eflags ); }

            uint64_t& sp() const { return gp( unwind_register_id::amd64_rsp ); }
            uint64_t& ip() const { return gp( unwind_register_id::amd64_rip ); }
            uint64_t& frame() const { return gp( owner->frame_register ); }

            template<typename T>
            bool read( T& out, uint64_t address ) const
            {
                if ( rmemcpy ) return rmemcpy( &out, address, sizeof( T ) );
                memcpy( &out, ( const void* ) address, sizeof( T ) );
                return true;
            }
            template<typename T>
            bool write( uint64_t address, const T& data ) const
            {
                if ( wmemcpy ) return wmemcpy( address, &data, sizeof( T ) );
                memcpy( ( void* ) address, &data, sizeof( T ) );
                return true;
            }
        };
        //bool rewind( const state_t& state ) const = 0;
        //bool unwind( const state_t& state ) const = 0;
    };
    struct amd64_unwind_set_frame_t : amd64_unwind_code_t
    {
        // Implement the interface.
        //
        size_t get_size() const { return 1; }
        bool rewind( const state_t& state ) const
        {
            state.frame() = state.sp() + state.owner->get_frame_offset();
            return true;
        }
        bool unwind( const state_t& state ) const
        {
            state.sp() = state.frame() - state.owner->get_frame_offset();
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
                return op_info * 8 + 8;
            if ( op_info ) return *( uint32_t* ) ( this + 1 );
            else           return 8 * *( uint16_t* ) ( this + 1 );
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
        size_t get_sp_offset() const
        {
            if ( unwind_op == unwind_opcode::save_nonvol_far )
                return *( uint32_t* ) ( this + 1 );
            else
                return 8 * *( uint16_t* ) ( this + 1 );
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
        size_t get_sp_offset() const
        {
            if ( unwind_op == unwind_opcode::save_xmm128_far )
                return *( uint32_t* ) ( this + 1 );
            else
                return 16 * *( uint16_t* ) ( this + 1 );
        }
        unwind_register_id get_register() const { return ( unwind_register_id ) op_info; }

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
        bool rewind( const state_t& state ) const
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

    template<typename T>
    static bool visit_amd64_unwind( const unwind_code_t& code, T&& visitor )
    {
        switch ( ( unwind_opcode ) code.unwind_op )
        {
            case unwind_opcode::push_nonvol:     visitor( ( const amd64_unwind_push_t* ) &code );       break;
            case unwind_opcode::alloc_large:
            case unwind_opcode::alloc_small:     visitor( ( const amd64_unwind_alloc_t* ) &code );      break;
            case unwind_opcode::set_frame:       visitor( ( const amd64_unwind_set_frame_t* ) &code );  break;
            case unwind_opcode::save_nonvol:
            case unwind_opcode::save_nonvol_far: visitor( ( const amd64_unwind_save_gp_t* ) &code );    break;
            case unwind_opcode::save_xmm128:
            case unwind_opcode::save_xmm128_far: visitor( ( const amd64_unwind_save_xmm_t* ) &code );   break;
            case unwind_opcode::push_machframe:  visitor( ( const amd64_unwind_iframe_t* ) &code );     break;
            case unwind_opcode::spare_code:
            case unwind_opcode::epilog:          return false;
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
};
#pragma pack(pop)