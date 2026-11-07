# linux-pe

*linux-pe* is a header only library aiming to simplify the parsing of Portable Executable (PE) and Common Object File Format (COFF) formats used to describe all things executable  in 32-bit and 64-bit versions of Windows operating system with no dependencies on the Windows headers.

# Supported Compilers
linux-pe uses C++20 features, so make sure to include `-std=c++20` or equivalent in your compiler options. g++10 & clang++-10 and higher should be able to compile without any issues but the tested compilers known to be working are are:

- g++12
- clang++-14
- MSVC VS 2022

# C++20 Modules
Modules are opt-in and require CMake 3.28+ with the Ninja or Visual Studio generator:

```cmake
set(LINUXPE_BUILD_MODULES ON)
add_subdirectory(linux-pe)
target_link_libraries(my_target PRIVATE linux-pe::modules)
```

```cpp
import linuxpe;                          // Everything.
import linuxpe.nt;                       // All NT headers and data directories.
import linuxpe.nt.directories;           // All data directories.
import linuxpe.nt.directories.exports;   // Only includes/nt/directories/dir_export.hpp and its dependencies.
```

Every header has its own module, named after its path: `img_common.hpp` is `linuxpe.common`, `dir_`/`aux_` prefixes are dropped, and `dir_export.hpp`/`dir_import.hpp` are `exports`/`imports` because `export` and `import` cannot be used in module names. `linuxpe`, `linuxpe.coff`, `linuxpe.coff.auxiliaries`, `linuxpe.nt` and `linuxpe.nt.directories` re-export everything below them. The module units in `modules/` mirror the layout of `includes/` (`modules/nt/directories/dir_export.cppm`), with each re-exporting module next to its directory (`modules/nt/directories.cppm`).

- Macros (`VAR_LEN`, `LEN_SHORT_STR`, ...) are not exported. Configure the modules through the module target, e.g. `target_compile_definitions(linux-pe-modules PUBLIC VAR_LEN=1)`, and include `img_macros.hpp` if you need the macros themselves.
- Standard library headers are not re-exported; include what you use.
- Headers and modules share the same entities, so translation units using `#include` and `import` can be linked together.
- Define `LINUXPE_IMPORT_MODULES` to turn every `#include` of a linux-pe header into an `import` of its module. This is required when one translation unit both imports a module and includes a header (directly or through another library).
