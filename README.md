# Zephyr C++ Framework - ZPP

First, it is not the intention of ZPP to implement a `std::` library for
Zephyr. ZPP tries to wrap the Zephyr C-API in C++17/C++20 without causing to
much runtime and/or memory overhead.

To add zpp to your own project just add the git repo to your west.yml manifest
file like this;

```
manifest:
  projects:
    - name: zpp
      remote: https://github.com/lowlander
      revision: master
      path: modules/zpp
```

If you just want to test it the <https://github.com/lowlander/zpp_bootstrap.git>
repository has some scripts that already have a west setup for more easy testing

## Usage

Everything of ZPP is placed in the `zpp::` namespace. All header files are
in the `zpp/` subdirectory and have a `.hpp` extension, so they can be
included as follows `#include <zpp/thread.hpp>`
