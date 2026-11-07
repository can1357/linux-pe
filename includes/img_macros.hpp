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

// Macros only: modules cannot export macros, so every header includes this file textually, even
// when its declarations come from a module.
//
// LINUXPE_MODULE:         Defined by the module interface units in modules/ while they include a header.
// LINUXPE_IMPORT_MODULES: Define it to turn every #include of a linux-pe header into an import of the
//                         matching module (e.g. "nt/image.hpp" -> import linuxpe.nt.image;), which makes
//                         #include and import usable within the same translation unit.
//
#ifdef LINUXPE_MODULE
    #undef LINUXPE_IMPORT_MODULES
    #define LINUXPE_EXPORT                export extern "C++" // Attach to the global module, so header and module users share entities.
    #define LINUXPE_INTERNAL              extern "C++"        // Same attachment, but not visible to importers.
#else
    #define LINUXPE_EXPORT
    #define LINUXPE_INTERNAL
#endif

#define WIN_STRUCT_PACKING                _Pragma("pack(push, 4)") // Structure packings of the variants.
#define COFF_STRUCT_PACKING               _Pragma("pack(push, 1)") //
#define LEN_SHORT_STR                     8 // Common short string length used in COFF and it's variants.

// If your compiler does not support zero-len arrays, define VAR_LEN as 1 before including linuxpe.
//
#ifndef VAR_LEN
    #ifndef _MSC_VER
        #define VAR_LEN 1
    #else
        #pragma warning(disable:4200)
        #define VAR_LEN
    #endif
#endif
