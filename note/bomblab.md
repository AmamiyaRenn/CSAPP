# gdb 常用指令

## 启动与退出 gdb

gdb bomb
gdb -q bomb // 表示不打印 gdb 版本信息，界面较为干净；
或者 gdb -q，然后再 file bomb
quit(q) 退出 gdb

## tui(gdb 自带 gui 界面)

用于分割窗口，可以一边查看代码，一边测试：
layout src 显示源代码窗口
layout asm 显示反汇编窗口
layout regs 显示源代码/反汇编和 CPU 寄存器窗口
layout split 显示源代码和反汇编窗口
Ctrl + l 刷新窗口

## 运行代码

run(r) 运行程序，当遇到断点后，程序会在断点处停止运行；如果没断点就会运行完
start 运行程序并停在 main 第一行
next(n) 单步运行
n 3 运行 3 条指令
step(s) 单步进入
s 3 允许进入函数的运行三条指令
continue(c) 继续执行，到下一个断点处（或运行结束）
finish 运行完当前函数并返回

## 查看源码

list(l) 列出程序的源代码，默认每次显示 10 行

## 反汇编

disas 查看当前所在函数的汇编代码
disas func 查看 func 函数的汇编代码
nexti(ni) 汇编单步运行
stepi(si) 汇编单步进入

## 断点

breakpoint(b/br)
b n 在第 n 行处设置断点
b func 在函数 func()的入口处设置断点
delete(d) 删除所有断点
d n 删除第 n 号断点

## 显示信息

info(i)
i r 显示寄存器信息
i br 显示断点信息
backtrace(bt) 显示当前调用的堆栈
where 当前所在堆栈
set args 设定 main 函数的\*args[]参数
show args 查看设定的参数

## 打印表达式

print(p) 其中“表达式”包括数字，变量甚至是函数调用

## 查看内存内容

examine(x) 用指定格式查看指定内存的内容，格式为`x/<n/f/u> address` n、f、u 是可选的参数，address 为地址如 0x4025cf，也可以是寄存器如$rsp

-   n 是一个正整数，表示显示内存的长度，也就是说从当前地址向后显示几个单位的内容
-   f 表示显示的格式（f 可选值）
    -   x 按十六进制格式显示变量。
    -   d 按十进制格式显示变量。
    -   u 按十六进制格式显示无符号整型。
    -   o 按八进制格式显示变量。
    -   t 按二进制格式显示变量。
    -   a 按十六进制格式显示变量。
    -   c 按字符格式显示变量。
    -   f 按浮点数格式显示变量。
-   u 表示每个单位长度，默认为 4 字节（u 可选值）
    -   b 表示单字节
    -   h 表示双字节
    -   w 表示四字 节
    -   g 表示八字节
