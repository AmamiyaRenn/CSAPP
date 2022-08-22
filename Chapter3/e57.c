/*
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-22 21:14:46
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
 */
double funct3(int *ap, double b, long c, float *dp)
{
    float val = *dp;
    if (b > *dp)
        return c * val;
    else
        return 2 * val + c;
}