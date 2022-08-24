<!--
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-16 11:07:14
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
-->

# 阅读笔记

## 第三章

### 需要加强的题目

1. e20
2. e26 C
3. e28 B/C
4. e30
5. e33
    1. 不懂答案
6. e44 E
7. e46 E
# Lab
## datalab
### howManyBits
这道题其实我想了很多方法，但一切的一切最后都是归结为如何获得最左边的“1”，而我一直都没想出来，不过我能想到的是——答案最大为32，即需要6位数0b100000表达，不过一直没想到这个其实就是二分的方法，最后我是看到[lab1 CSAPP：datalab](https://zhuanlan.zhihu.com/p/449877475)中的心得部分才恍然大悟——log2 32=5！，所以这道题就是通过二分来找到最左边的“1”，因为我们需要知道的只是“1”的位置，而这个位置可以被二分查找。