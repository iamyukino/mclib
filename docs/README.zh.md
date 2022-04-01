mclib
=====

[![mclib-readme](https://img.shields.io/badge/c%2B%2B%20-11-blue.svg?style=flat-square)](https://github.com/iamyukino/mclib) [![mclib-readme](https://img.shields.io/badge/license-LGPL-orange.svg?style=flat-square)](https://github.com/iamyukino/mclib/blob/main/LICENSE)

[mclib](https://github.com/iamyukino/mclib) 是一个免费且开源的图形库，使您能够在 C++ 中使用 [pygame](https://www.pygame.org) 风格函数来制作基于 Windows 的多媒体应用程序，比如游戏。它通过使用 [Windows API](https://docs.microsoft.com/en-us/previous-versions/ff404219(v=msdn.10)) 抽象常用功能，并且按照 pygame 风格进行实现，使编写这些程序变成一项更为直观的任务。  


### 致谢

感谢 aaaaaaa421 和 hellovfp 的意见和建议，使我开始和进一步学习 windows API 和面向对象编程。感谢父母和老师的支持与鼓励，是你们给我了前进的动力。  

感谢您使用本图形库。像往常一样，代码中存在着大量需要优化和改进的问题与不足，我对此承担所有责任并且向你致歉。对不起。如果你对此图形库有疑问，或希望提出任何意见或建议，欢迎在 [Github](https://github.com/iamyukino/mclib) 上给我私信或留言。  


### 环境依赖

毋庸置疑，mclib 基于并且强烈依赖 Windows API 和 C++。  
最低支持版本:
* C++ 标准 >= 201103L
* Windows 系统版本 >= Windows 2000 Professional  

mclib 保证在 C++11 或更晚的标准版本下，使用 GCC 或 MSVC 进行编译不会产生任何警告信息。您可以在编译时添加以下或更多的编译选项来测试（MSVC 可以将警告等级设置为等级 4）：

GCC:
```c
-std=c++11 -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option  
```
MSVC:
```c
/std:c++11 /Zc:__cplusplus /Wall
```


### 许可证

此图形库使用 [GNU LGPL version 2.1](https://www.gnu.org/licenses/lgpl-3.0.html) 开源许可证进行许可，该文件的详细内容可在 ``LICENSE`` 中找到。我们保留此库在未来版本中更改为使用其他许可证的权利。

此许可证基本上意味着您可以在任何项目中使用 mclib，但是如果您对 mclib 图形库本身进行任何改动，都必须使用同一许可证进行许可。您可以将使用此图形库的项目闭源，或将其用作商业用途。

特别地，[examples](https://github.com/iamyukino/mclib/tree/main/examples) 子目录下的代码或文件属于公共文件，不受开源许可证的限制。

有关开源许可证的详细内容，请参见 ``LICENSE`` 。  
[GPL-3.O](LICENSE) © Yukino Amamiya