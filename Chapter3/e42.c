/*
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-18 11:25:43
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
 */
struct ELE
{
    long v;
    struct ELE *p;
};

long fun(struct ELE *ptr)
{
    long val = 0;
    while (ptr != 0)
    {
        val += ptr->v;
        ptr = ptr->p;
    }
    return val;
}