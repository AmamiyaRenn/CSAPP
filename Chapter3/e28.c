/*
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-16 16:52:03
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
 */
long fun_b(unsigned long x)
{
    long val = 0;
    long i;
    for (i = 64; i != 0; i--)
    {
        long temp = x & 1;
        val = (2 * val) | temp;
        x >>= 1;
    }
    return val;
}

long fun_b_ans(unsigned long x)
{
    long val = 0;
    long i;
    for (i = 64; i != 0; i--)
    {
        val = (val << 1) | (x & 1);
        x >>= 1;
    }
    return val;
}