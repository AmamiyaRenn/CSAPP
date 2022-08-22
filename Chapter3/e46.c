/*
 * @Description:
 * @Author: Amamiya
 * @Date: 2022-08-18 15:31:40
 * TechChangeTheWorld
 * WHUROBOCON_Alright_Reserved
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *get_line()
{
    char buf[4];
    char *result;
    gets(buf);
    result = (char *)malloc(strlen(buf));
    strcpy(result, buf);
    return result;
}

int main()
{
}