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
#include "common.hpp"
#include "file_header.hpp"
#include "optional_header.hpp"
#include "data_directories.hpp"

namespace win
{
    namespace debug
    {
        // Enum -> String
        //
        static inline const char* resolve_enum( machine_id id )
        {
            switch ( id )
            {
                case machine_id::unknown:                       return "unknown";
                case machine_id::target_host:                   return "target_host";
                case machine_id::i386:                          return "i386";
                case machine_id::r3000:                         return "r3000";
                case machine_id::r4000:                         return "r4000";
                case machine_id::r10000:                        return "r10000";
                case machine_id::wcemipsv2:                     return "wcemipsv2";
                case machine_id::alpha:                         return "alpha";
                case machine_id::sh3:                           return "sh3";
                case machine_id::sh3dsp:                        return "sh3dsp";
                case machine_id::sh3e:                          return "sh3e";
                case machine_id::sh4:                           return "sh4";
                case machine_id::sh5:                           return "sh5";
                case machine_id::arm:                           return "arm";
                case machine_id::thumb:                         return "thumb";
                case machine_id::armnt:                         return "armnt";
                case machine_id::am33:                          return "am33";
                case machine_id::powerpc:                       return "powerpc";
                case machine_id::powerpcfp:                     return "powerpcfp";
                case machine_id::ia64:                          return "ia64";
                case machine_id::mips16:                        return "mips16";
                case machine_id::alpha64:                       return "alpha64";
                case machine_id::mipsfpu:                       return "mipsfpu";
                case machine_id::mipsfpu16:                     return "mipsfpu16";
                case machine_id::tricore:                       return "tricore";
                case machine_id::cef:                           return "cef";
                case machine_id::ebc:                           return "ebc";
                case machine_id::amd64:                         return "amd64";
                case machine_id::m32r:                          return "m32r";
                case machine_id::arm64:                         return "arm64";
                case machine_id::cee:                           return "cee";
                default:                                        return nullptr;
            }
        }
        static inline const char* resolve_enum( subsystem_id id )
        {
            switch ( id )
            {
                case subsystem_id::unknown:                     return "unknown";
                case subsystem_id::native:                      return "native";
                case subsystem_id::windows_gui:                 return "windows_gui";
                case subsystem_id::windows_cui:                 return "windows_cui";
                case subsystem_id::os2_cui:                     return "os2_cui";
                case subsystem_id::posix_cui:                   return "posix_cui";
                case subsystem_id::native_windows:              return "native_windows";
                case subsystem_id::windows_ce_gui:              return "windows_ce_gui";
                case subsystem_id::efi_application:             return "efi_application";
                case subsystem_id::efi_boot_service_driver:     return "efi_boot_service_driver";
                case subsystem_id::efi_runtime_driver:          return "efi_runtime_driver";
                case subsystem_id::efi_rom:                     return "efi_rom";
                case subsystem_id::xbox:                        return "xbox";
                case subsystem_id::windows_boot_application:    return "windows_boot_application";
                case subsystem_id::xbox_code_catalog:           return "xbox_code_catalog";
                default:                                        return nullptr;
            }
        }
        static inline const char* resolve_enum( directory_id id )
        {
            switch ( id )
            {
                case directory_entry_export:                    return "export";
                case directory_entry_import:                    return "import";
                case directory_entry_resource:                  return "resource";
                case directory_entry_exception:                 return "exception";
                case directory_entry_security:                  return "security";
                case directory_entry_basereloc:                 return "basereloc";
                case directory_entry_debug:                     return "debug";
                case directory_entry_architecture:              return "architecture/copyright";
                case directory_entry_globalptr:                 return "globalptr";
                case directory_entry_tls:                       return "tls";
                case directory_entry_load_config:               return "load_config";
                case directory_entry_bound_import:              return "bound_import";
                case directory_entry_iat:                       return "iat";
                case directory_entry_delay_import:              return "delay_import";
                case directory_entry_com_descriptor:            return "com_descriptor";
                case directory_reserved0:                       return "reserved0";
                default:                                        return nullptr;
            }
        }
    };
};