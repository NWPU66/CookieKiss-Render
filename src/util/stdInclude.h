#pragma once
/**NOTE -  #include 的路径及顺序
 * dir2/foo2.h.
 * C 语言系统文件 (确切地说: 用使用方括号和 .h 扩展名的头文件), 例如 <unistd.h> 和 <stdlib.h>.
 * C++ 标准库头文件 (不含扩展名), 例如 <algorithm> 和 <cstddef>.
 * 其他库的 .h 文件.
 * 本项目的 .h 文件.
 */

// C Library
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// C++ Library
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
