# linux-pe

*linux-pe* is a header only library aiming to simplify the parsing of Portable Executable (PE) and Common Object File Format (COFF) formats used to describe all things executable  in 32-bit and 64-bit versions of Windows operating system with no dependencies on the Windows headers.

# Supported Compilers
linux-pe uses C++20 features, so make sure to include `-std=c++20` or equivalent in your compiler options. g++10 & clang++-10 and higher should be able to compile without any issues but the tested compilers known to be working are are:

- g++12
- clang++-14
- MSVC VS 2022
