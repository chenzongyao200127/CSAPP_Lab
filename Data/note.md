# 位运算

C语言中的位运算是一种高效处理数据的方法，它直接在数据的二进制表示上进行操作。
位运算主要用于对整型数据（如 `int`、`unsigned int` 等）的单个或多个位进行操作。以下是C语言中位运算的基本种类和用法：

1. **按位与（AND）- `&`**：
   - 用法：`result = a & b;`
   - 功能：对 `a` 和 `b` 的每一位进行AND操作。只有当两位都为1时，结果位才为1。

2. **按位或（OR）- `|`**：
   - 用法：`result = a | b;`
   - 功能：对 `a` 和 `b` 的每一位进行OR操作。如果任一位为1，结果位就为1。

3. **按位异或（XOR）- `^`**：
   - 用法：`result = a ^ b;`
   - 功能：对 `a` 和 `b` 的每一位进行XOR操作。只有当两位不同（一个为1，一个为0）时，结果位才为1。

4. **按位取反（NOT）- `~`**：
   - 用法：`result = ~a;`
   - 功能：对 `a` 的每一位进行取反操作。1变0，0变1。

5. **左移 - `<<`**：
   - 用法：`result = a << n;`
   - 功能：将 `a` 的所有位向左移动 `n` 位。左边超出的位被丢弃，右边空出的位用0填充。

6. **右移 - `>>`**：
   - 用法：`result = a >> n;`
   - 功能：将 `a` 的所有位向右移动 `n` 位。对于无符号数，左边空出的位用0填充；对于有符号数，行为依赖于机器和编译器（可能用0填充，也可能用符号位填充）。

位运算的应用场景非常广泛，包括但不限于数据压缩、加密、性能优化、特殊计算等领域。掌握位运算可以帮助你更深入地理解计算机中数据的表示和处理方式。


### 1. 按位与（AND）- `&`
```c
unsigned int a = 12; // 二进制表示为 1100
unsigned int b = 5;  // 二进制表示为 0101
unsigned int result = a & b;  // 结果为 0100，即 4
```
在这个例子中，只有第三位同时为1，所以结果是 `0100`。

### 2. 按位或（OR）- `|`
```c
unsigned int a = 12; // 二进制表示为 1100
unsigned int b = 5;  // 二进制表示为 0101
unsigned int result = a | b;  // 结果为 1101，即 13
```
在这个例子中，除了第二位是两个操作数都为0外，其他位至少有一个1，所以结果是 `1101`。

### 3. 按位异或（XOR）- `^`
```c
unsigned int a = 12; // 二进制表示为 1100
unsigned int b = 5;  // 二进制表示为 0101
unsigned int result = a ^ b;  // 结果为 1001，即 9
```
在这个例子中，只有第一位和第四位是一个1一个0，所以结果是 `1001`。

### 4. 按位取反（NOT）- `~`
```c
unsigned int a = 12; // 二进制表示为 0000 1100
unsigned int result = ~a;  // 结果为 1111 0011
```
所有的位都取反了。

### 5. 左移 - `<<`
```c
unsigned int a = 5; // 二进制表示为 0101
unsigned int result = a << 2;  // 结果为 10100，即 20
```
`a` 中的位被向左移动了2位，右边用0填充。

### 6. 右移 - `>>`
```c
unsigned int a = 20; // 二进制表示为 10100
unsigned int result = a >> 2;  // 结果为 00101，即 5
```
`a` 中的位被向右移动了2位，左边用0填充。

这些例子展示了基本的位运算操作。在实际编程中，你可能会结合使用这些操作来执行更复杂的任务，如设置或清除特定的位、检查位的状态等。

在C语言中，指定或操作特定的位可以通过结合位运算符和位掩码来实现。这通常涉及以下几个步骤：

1. **设置（置位）特定的位**
2. **清除（清零）特定的位**
3. **切换（翻转）特定的位**
4. **检查（读取）特定的位**

让我们逐一解释这些操作，并提供相应的示例：

### 1. 设置（置位）特定的位
要设置（或称置位）变量中的特定位，可以使用按位或操作（`|`）和一个位掩码。位掩码是一个只在要设置的位上为1的数。

例如，要设置第3位（从0开始计数）：

```c
unsigned int value = 0b00000100;  // 二进制表示，第3位为1，其余为0
unsigned int bitmask = 0b00001000; // 要设置的位为第3位
value |= bitmask;  // 现在 value 变为 0b00001100
```

### 2. 清除（清零）特定的位
要清除变量中的特定位，可以使用按位与操作（`&`）和一个位掩码。这个掩码在要清除的位上为0，其他位上为1。

例如，要清除第2位：

```c
unsigned int value = 0b00000110;  // 初始值，第2位为1
unsigned int bitmask = 0b11111011; // 第2位为0，其余位为1
value &= bitmask;  // 现在 value 变为 0b00000010
```

### 3. 切换（翻转）特定的位
要切换变量中的特定位，即将1变为0，将0变为1，可以使用按位异或操作（`^`）和一个位掩码。这个掩码在要切换的位上为1。

例如，要切换第1位：

```c
unsigned int value = 0b00000010;  // 初始值，第1位为1
unsigned int bitmask = 0b00000010; // 要切换的位为第1位
value ^= bitmask;  // 现在 value 变为 0b00000000
```

### 4. 检查（读取）特定的位
要检查变量中的特定位是否为1，可以使用按位与操作（`&`）和一个位掩码。如果结果非零，则该位为1。

例如，检查第2位是否为1：

```c
unsigned int value = 0b00000100;  // 初始值
unsigned int bitmask = 0b00000100; // 检查的位为第2位
if (value & bitmask) {
    // 第2位为1
} else {
    // 第2位为0
}
```

这些是基本的位操作技巧。通过使用这些技术，你可以有效地操作和查询变量中的单个位。在实际的应用中，这些操作常用于处理标志位、设定配置选项、操作硬件寄存器等场景。


# 浮点数

浮点数在C语言和许多其他编程语言中用于表示实数，它能够表示非常大或非常小的数值。了解浮点数的工作原理对于精确和有效的数值计算至关重要。以下是关于浮点数的主要概念和特性：

### 1. 浮点数的表示
浮点数通常遵循IEEE 754标准，该标准定义了浮点数的存储和计算方式。一个浮点数由三部分组成：
   - **符号位（Sign bit）**：确定数值的正负。
   - **指数（Exponent）**：表示数值的范围。
   - **尾数（Mantissa或Significand）**：表示数值的精确度。

### 2. 浮点数的类型
在C语言中，常见的浮点数类型有两种：
   - **float**：通常是32位，包括1位符号位、8位指数和23位尾数。
   - **double**：通常是64位，包括1位符号位、11位指数和52位尾数。
   - 还有 **long double** 类型，其大小和精度在不同系统和编译器中可能不同。

### 3. 精度和范围
   - **float** 类型提供的精度大约是6到7位十进制数。
   - **double** 类型提供的精度大约是15到16位十进制数。
   - 浮点数的范围取决于其大小和如何使用指数和尾数来表示数值。

### 4. 特殊值
   - **NaN**（Not a Number）：表示未定义或无法表示的值。
   - **正负无穷大**：当数值超出浮点数能表示的范围时使用。

### 5. 精度问题和舍入
浮点数的计算可能不精确，因为某些值无法用有限的尾数位精确表示。这可能导致舍入误差。

### 6. 浮点数和整数的转换
将浮点数转换为整数时，数值会被截断。这意味着小数部分被丢弃，只保留整数部分。

### 7. 数学运算
C语言提供了一系列用于浮点数运算的数学函数，如 `sqrt()`、`sin()`、`cos()` 等，这些函数在 `<math.h>` 头文件中声明。

### 8. 浮点数在内存中的表示
浮点数在内存中的表示是非直观的，因为它们使用二进制表示小数和指数。这与整数的二进制表示完全不同。

### 9. 浮点数和二进制表示的转换
在某些高级应用中，可能需要直接操作或查看浮点数的二进制表示。这通常涉及位操作和类型转换技巧。

### 10. 限制和替代方案
对于需要高精度或特定数值范围的应用，浮点数可能不是最佳选择。在这些情况下，可以考虑使用整数、固定点数表示，或使用专门的数学库来处理。

了解和掌握浮点数的这些基本概念对于进行科学计算、图形处理、工程计算等领域的编程至关重要。浮点数的处理方式在不同的编程语言中可能略有不同，但基本原则和挑战是相似的。










# 二进制补码




# 学生必读
~~~c
/*
 * 给学生的指令：
 *
 * 步骤 1：仔细阅读以下指令。
 */

你将通过编辑这个源文件中的一系列函数来提供你对数据实验室的解决方案。

整数编码规则：

  用一行或多行C语言代码替换每个函数中的 "return" 语句，以实现该函数。你的代码必须遵循以下风格：

  int Funct(arg1, arg2, ...) {
      /* 简要描述你的实现方式 */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  每个 "Expr" 是一个表达式，仅使用以下内容：
  1. 整数常量0到255（0xFF）之间，包括这两个值。你不允许使用如 0xffffffff 这样的大常量。
  2. 函数参数和局部变量（无全局变量）。
  3. 单目整数操作 ! ~
  4. 双目整数操作 & ^ | + << >>
    
  一些问题进一步限制了允许的操作符集合。
  每个 "Expr" 可能由多个操作符组成。你不限于每行一个操作符。

  你明确禁止：
  1. 使用任何控制结构，如 if, do, while, for, switch 等。
  2. 定义或使用任何宏。
  3. 在此文件中定义任何额外的函数。
  4. 调用任何函数。
  5. 使用任何其他操作，如 &&, ||, -, 或 ?:
  6. 使用任何形式的类型转换。
  7. 使用除 int 之外的任何数据类型。这意味着你不能使用数组、结构体或联合体。

 
  你可以假设你的机器：
  1. 使用2的补码、32位表示整数。
  2. 算术右移。
  3. 当移位量小于0或大于31时，行为不可预测。


可接受的编码风格示例：
  /*
   * pow2plus1 - 返回 2^x + 1，其中 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* 利用移位的能力来计算2的幂 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - 返回 2^x + 4，其中 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* 利用移位的能力来计算2的幂 */
     int result = (1 << x);
     result += 4;
     return result;
  }

====================================================================================================

浮点编码规则

对于要求你实现浮点操作的问题，编码规则不那么严格。你可以使用循环和条件控制。你可以使用 int 和 unsigned 类型。
你可以使用任意的整数和无符号常量。你可以对 int 或 unsigned 数据使用任何算术、逻辑或比较操作。

你明确禁止：
  1. 定义或使用任何宏。
  2. 在此文件中定义任何额外的函数。
  3. 调用任何函数。
  4. 使用任何形式的类型转换。
  5. 使用除 int 或 unsigned 之外的任何数据类型。这意味着你不能使用数组、结构体或联合体。
  6. 使用任何浮点数据类型、操作或常量。


注意事项：
  1. 使用 dlc（数据实验室检查器）编译器（手册中有描述）来检查你的解决方案是否合法。
  2. 每个函数有一个最大操作数（整数、逻辑或比较操作），你被允许在函数的实现中使用。最大操作数由 dlc 检查。注意赋值（'='）不被计算在内；你可以根据需要使用任意多个，不会受到惩罚。
  3. 使用 btest 测试工具来检查你的函数是否正确。
  4. 使用 BDD 检查器正式验证你的函数。
  5. 每个函数的最大操作数在每个函数的头部注释中给出。如果 writeup 和这个文件之间有任何不一致，请以这个文件为准。

/*
 * 步骤 2：根据编码规则修改以下函数。
 * 
 *   重要提示。为避免评分时的意外：
 *   1. 使用 dlc 编译器来检查你的解决方案是否符合编码规则。
 *   2. 使用 BDD 检查器正式验证你的解决方案是否产生正确的答案。
 */

~~~