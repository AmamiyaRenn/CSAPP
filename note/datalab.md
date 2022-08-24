![image](https://w.wallhaven.cc/full/4y/wallhaven-4yz5kx.jpg)

# CSAPP lab1: datalab

## 碎碎念

csapp 看到了第 4 章，想着之前也就做了做课内习题，也没实际敲过什么代码，听说 csapp 的 lab 非常好玩，加上刚学了 vim 正愁没带代码写，故来玩一玩这个 lab
。但是不得不说是真的难，我写了整整 3 天才完成，唯一可能比较自豪的就是我是独立完成的吧（指没直接看题解，但看了感悟之类的东西）

## 题目思路与解答

### bitXor

思路：我写出了异或的逻辑表，然后尝试用最小项表达，得到表达后把或通过德摩根定律改为与，问题解决

```cpp
int bitXor(int x, int y)
{
    return ~(~((~x) & y) & ~(x & (~y)));
}
```

### tmin

思路：根据定义可以写出答案

```cpp
int tmin(void)
{
    return 1 << 31;
}
```

### isTmax

思路：补码最大值有一个特征，即 Tmax+1==~Tmax，但是-1 也满足，所以我加了条判断用于排除-1

```cpp
int isTmax(int x)
{
    return (!((x + 1) ^ (~x))) & !!(x + 1);
}
```

### allOddBits

思路：只要 x 满足里面包括 0xAAAAAAAA 即可，所以想到用掩码，不过限制了只能 0x00~0xFF，所以要先通过位移来创造一个掩码出来

```cpp
int allOddBits(int x)
{
    int mask = (0xAA) | (0xAA << 8);
    mask = mask | (mask << 16);
    return !((x & mask) ^ mask);
}
```

### negate

思路：根据定义可以写出答案

```cpp
int negate(int x)
{
    return ~x + 1;
}
```

### isAsciiDigit

思路：观察可得 AsciiiDigit 有两个特征，一是必是 0x3?，二是对于?中的数字一定小于 A，所以对于一可以用位移掩码解决，对二可以用掩码+6 判断有没有溢出解决

```cpp
int isAsciiDigit(int x)
{
    return !(((x >> 4) ^ 0x3) | ((x & 0xF) + 6) >> 4);
}
```

### conditional

思路：条件表达式实质上只是判断是不是零，所以通过!可以很方便的获得答案，然后通过得到的答案创造掩码，利用掩码+位或即可做到选择值的效果

```cpp
int conditional(int x, int y, int z)
{
    int mask = ((!x) << 31) >> 31;
    return (~mask & y) | (mask & z);
}
```

### isLessOrEqual

思路：既然又是条件选择语句，就可以用类似上题的方法来解决，所以需要设计一个 flag 来做位或选择；判断大小最简单的想法是用 y-x（等同于 y+negate(x)），然后看符号位值，不过这只能适用与 x 与 y 符号相同时，对于不同时需要额外判断，所以通过对 x 与 y 符号位异或来得到 flag，然后可以发现符号位不同时只要 y 的符号是负则为 0，此题得解

```cpp
int isLessOrEqual(int x, int y)
{
    int signDiffFlag = !((x >> 31) ^ (y >> 31));
    return ((~((y + (~x) + 1) >> 31)) & signDiffFlag) | (!(signDiffFlag | (y >> 31)));
}
```

### logicalNeg

思路：问题的关键是让计算机“知道”0 与非 0 的区别，所以想到 0 的特殊性——~0+1 与 0 相同，不过还有一个漏洞，那就是 Tmin 也是满足这条的，所以通过符号位的区别来解决这个问题

```cpp
int logicalNeg(int x)
{
    return ((~((x ^ ((~x) + 1)) >> 31)) & 1) & (~(x >> 31));
}
```

### howManyBits

思路：这是我认为所有题目中最难的问题，整整困扰了我一个晚上+一个上午，这道题其实我想了很多方法，但一切的一切最后都是归结为如何获得最左边的“1”，而我一直都没想出来，不过我能想到的是——答案最大为 32，即需要 6 位数 0b100000 表达，不过一直没想到这个其实就是二分的方法，最后我是看到[lab1 CSAPP：datalab](https://zhuanlan.zhihu.com/p/449877475)中的心得部分才恍然大悟——二分！因为 log2 32=5！，所以这道题就是通过二分来找到最左边的“1”，因为我们需要知道的只是“1”的位置，而这个位置可以被二分查找。
而二分的思路就是不断判断二分的左边有没有数字，然后加上对应的数值，然后把 x 右移来继续满足二分性质，即可解决。对于 x 是负数的情况，改为正数即可

```cpp
int howManyBits(int x)
{
    int mask, flag, temp, ans;
    mask = x >> 31;
    x = (~mask & x) | (mask & (~x));

    flag = !!(x >> 16);
    temp = flag << 4;
    ans = temp + 1;
    x = x >> temp;

    flag = !!(x >> 8);
    temp = flag << 3;
    ans = ans + temp;
    x = x >> temp;

    flag = !!(x >> 4);
    temp = flag << 2;
    ans = ans + temp;
    x = x >> temp;

    flag = !!(x >> 2);
    temp = flag << 1;
    ans = ans + temp;
    x = x >> temp;

    flag = !!(x >> 1);
    ans = ans + flag;
    x = x >> flag;

    return x + ans;
}
```

### floatScale2

思路：根据要求与浮点数定义排除 0xFF 的情况，对于 exponent 等于 0 的情况，观察可以发现只需要用 uf+uf 的非符号位部分即可；对于规格化情况，uf 乘上 2 本质上就是让 M+1（对于 exponent==254，+1 得到 NaN 或无穷大，依然满足），本题得解

```cpp
unsigned floatScale2(unsigned uf)
{
    unsigned exponent = (uf >> 23) & 0xFF;

    if (exponent == 0xFF)
        return uf;
    if (exponent == 0)
        return (uf & 0x7FFFFFFF) + uf;

    return uf + 0x800000;
}
```

### floatFloat2Int

思路：读书可知，float 到 int 是向零舍入的，且对于溢出的情况（即 E>30，因为 1\*2 的 E 次中如果 E==31，转换的 int 将无法表达），会自动变成 Tmin，所以得到 0xFF 与 E>30 的答案；对于 E<0 的情况，一定都是在-1 到 1 之间的，所以可以返回 0；对于其他情况，根据浮点数定义，可以知道，2 的 E 次一定为整数，且 M=1+f，所以答案中一定包含`1<<E`，而对于 f，同样是左移了 E，不过本身是小数，所以要右移 23。最后关于符号位我利用负号可以自动把 int 转换为 int 类型相反数，所以我通过掩码得到正的答案，最后根据符号位转换 ans 的值，本题得解

```cpp
int floatFloat2Int(unsigned uf)
{
    unsigned sign = uf >> 31;
    unsigned exponent = (uf >> 23) & 0xFF;
    unsigned fraction = uf & 0x7FFFFF;
    int E = exponent - 127;
    int ans = ((1 << E) + ((fraction << E) >> 23)) & 0x7FFFFFFF;

    if (exponent == 0xFF || E > 30)
        return 0x80000000;
    else if (E < 0)
        return 0;

    return sign ? -ans : ans;
}
```

### floatPower2

思路：因为本身浮点数就是 2 的指数存储的，所以问题不过是这个 2 的 n 次方的 n 即 x 转化后的位置是多少，很自然的想到又要分类讨论了，我把结果分为了四类，第一是 too small，这种是 x<-149 即 x<-126-23，即不光 exponent 表达不了，连 fraction 都表达不了的情况；第二种是 too large，这种是大过了 exponent 导致无法表达的，所以是 x>127；第三种则是 fraction 能表达但不需要 exponent，或者说是非规格化情况，这种直接让 1 左移对应位置即可得到对应的 fraction；最后一种则是 fraction 无法表达需要 exponent 表达的情况，这种只需要让 x+127，即可得到 exponent 值

```cpp
unsigned floatPower2(int x)
{
    if (x < -149)
        return 0;
    else if (x < -126)
        return 1 << (x + 149);
    else if (x <= 127)
        return (x + 127) << 23;
    else
        return 0x7f800000;
}
```

## 心得

1. 异或是找不同的好帮手
2. 逻辑非是没有 bool 时的工具
3. 多想想不同事物之间的相同点与不同点，然后想办法让计算机理解这种相同与不同
4. 卡关了就想想是不是书还没搞熟
