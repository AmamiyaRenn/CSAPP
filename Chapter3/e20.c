long arith(long n)
{
    long val = 1;
    for (; n > 1; n--)
        val *= n;
    return val;
}
