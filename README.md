# linux-pe

_linux-pe_ is a header only library aiming to simplify the parsing of Portable
Executable (PE) and Common Object File Format (COFF) formats used to describe
all things executable in 32-bit and 64-bit versions of Windows operating system
with no dependencies on the Windows headers.

# Supported Compilers

linux-pe uses C++20 features, so make sure to include `-std=c++20` or equivalent
in your compiler options. g++10 & clang++-10 and higher should be able to
compile without any issues but the tested compilers known to be working are are:

- g++12
- clang++-14
- MSVC VS 2022

# C++20 Modules

Modules are optional, and using linux-pe as a header-only library works as
before.

## Requirements

|           | Minimum                                 | Tested                                |
| --------- | --------------------------------------- | ------------------------------------- |
| GCC       | 14                                      | 16.2.1                                |
| Clang     | 16 (with `clang-scan-deps`)             | 23.1.4                                |
| MSVC      | Visual Studio 2022 17.4 (toolset 14.34) | 19.50, 19.51 (toolset v145)           |
| CMake     | 3.28                                    | 3.28, 4.4                             |
| Generator | Ninja 1.11+ or Visual Studio 2022+      | Ninja 1.11, 1.13, MSBuild 18.4, 18.11 |

The minimum versions are the ones CMake supports for building modules. MSVC was
tested with [docker-msvc-wine](https://github.com/neonbyte1/docker-msvc-wine).

## Usage

```cmake
set(LINUXPE_BUILD_MODULES ON)
add_subdirectory(linux-pe)
target_link_libraries(my_target PRIVATE linux-pe::modules)
```

For Visual Studio solutions, add `linuxpe-modules.vcxproj` to the solution and
reference it from your project (Add > Reference). The reference also adds the
`includes/` directory to your project. The modules are built with the default
runtime library (`/MD`, `/MDd`), so change it in `linuxpe-modules.vcxproj` if
your project uses `/MT`.

```cpp
import linuxpe;                          // Everything.
import linuxpe.nt;                       // All NT headers and data directories.
import linuxpe.nt.directories;           // All data directories.
import linuxpe.nt.directories.exports;   // Only includes/nt/directories/dir_export.hpp and its dependencies.
```

Every header has its own module, named after its path: `img_common.hpp` is
`linuxpe.common`, `dir_`/`aux_` prefixes are dropped, and
`dir_export.hpp`/`dir_import.hpp` are `exports`/`imports` because `export` and
`import` cannot be used in module names. `linuxpe`, `linuxpe.coff`,
`linuxpe.coff.auxiliaries`, `linuxpe.nt` and `linuxpe.nt.directories` re-export
everything below them. The module units in `modules/` mirror the layout of
`includes/` (`modules/nt/directories/dir_export.cppm`), with each re-exporting
module next to its directory (`modules/nt/directories.cppm`).

- Macros (`VAR_LEN`, `LEN_SHORT_STR`, ...) are not exported. Configure the
  modules through the module target, e.g.
  `target_compile_definitions(linux-pe-modules PUBLIC VAR_LEN=1)`, and include
  `img_macros.hpp` if you need the macros themselves.
- Standard library headers are not re-exported; include what you use.
- Headers and modules share the same entities, so translation units using
  `#include` and `import` can be linked together.
- Define `LINUXPE_IMPORT_MODULES` to turn every `#include` of a linux-pe header
  into an `import` of its module. This is required when one translation unit
  both imports a module and includes a header (directly or through another
  library).
- With GCC, put your `#include`s before any `import`.

## Breaking change: `ar::entry_t::long_string_offset`

`ar::entry_t::long_string_offset` is now `ar::entry_t::long_string.offset`.

The field used to live in an unnamed struct inside a union. GCC doesn't allow
members with constructors there ("member ... with constructor not allowed in
anonymous aggregate"), and `string_integer` has one, so `coff/archive.hpp` never
compiled with GCC and the `linuxpe.coff.archive` module couldn't be built
either. Giving the struct a name fixes that. The layout is unchanged:
`sizeof(ar::entry_t)` is still 60 and the offset still starts at byte 1.

```cpp
// before
auto offset = entry.long_string_offset.get();
// after
auto offset = entry.long_string.offset.get();
```
