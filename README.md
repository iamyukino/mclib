mclib
=====

[![mclib-readme](https://img.shields.io/badge/c%2B%2B%20-11-blue.svg?style=flat-square)](https://github.com/iamyukino/mclib) [![mclib-readme](https://img.shields.io/badge/license-LGPL-orange.svg?style=flat-square)](https://github.com/iamyukino/mclib/blob/main/LICENSE)

[mclib](https://github.com/iamyukino/mclib) is a free and open-source library for the development of multimedia applications like video games in Windows using C++.  It uses [Windows API](https://docs.microsoft.com/en-us/previous-versions/ff404219(v=msdn.10)) to abstract the most common functions into [pygame](https://www.pygame.org) style, making writing these programs a more intuitive task.  


### Credits

Thanks to aaaaaaa421 and hellovfp for their thoughts and ideas, which enabled me to begin studying winAPI and OOP programming. Thanks to my parents and instructors for their encouragement and support, which inspired me to keep going.  

Thank you for making use of this graphics library. As is customary, the code will have several mistakes and omissions, as well as numerous aspects that must be optimized and enhanced. This is completely my fault. If you discover any issues, please contact me or report them on [Github](https://github.com/iamyukino/mclib).  


### Dependencies

mclib is obviously strongly dependent on Windows API and C++.  
Dependency versions:
* C++ standard >= 201103L
* GCC version >= 4.9.2
* Windows version >= Windows 2000 Professional  

mclib guarantees that no warning will appear under the C++11 or later standards using GCC/MSVC. You can add the following commands (GCC) when calling the compiler or set warning level 4 (MSVC) to test:  

GCC
```c
-std=c++11 -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option  
```
MSVC
```c
/std:c++11 /Zc:__cplusplus /Wall
```


### License

This library is distributed under [GNU LGPL version 2.1](https://www.gnu.org/licenses/lgpl-3.0.html), which can be found in the file ``LICENSE``.  We reserve the right to place future versions of this library under a different license.

This basically means you can use mclib in any project you want, but if you make any changes or additions to mclib itself, those must be released with a compatible license (preferably submitted back to the mclib project).  Closed source and commercial games are fine.

The programs in the [examples](https://github.com/iamyukino/mclib/tree/main/examples) subdirectory are in the public domain.

See ``LICENSE`` for licenses of dependencies.  
[GPL-3.O](LICENSE) © Yukino Amamiya
