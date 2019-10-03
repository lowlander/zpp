# Zephyr C++ Framework - ZPP

First, it is not the intention of ZPP to implement a `std::` library for
Zephyr. ZPP tries to wrap the Zephyr C-API in C++17 without causing to much
runtime and/or memory overhead. 

Since ZPP is a header only library, it is not that hard to add to your own
Zephyr project, by just copying in the include directory.

The <https://github.com/lowlander/zpp_bootstrap.git> repository has some
scripts that make testing ZPP a bit more easy.

## API

Everything of ZPP is placed in the `zpp::` namespace. All header files are
in the `zpp/` subdirectory and have a `.hpp` extension, so they can be
included as follows `#include <zpp/thread.hpp>`

### zpp::thread

TODO

### zpp::mutex

TODO

### zpp::sem

TODO

### zpp::clock

TODO

### zpp::timer

TODO

