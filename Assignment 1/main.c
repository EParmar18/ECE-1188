/*
 * main.c
 *
 *  Created on: Jan 25, 2022
 *      Author: Eshan
 */

#include "msp.h"

int modify_value(int z);

void main(void)
{
    // Variables
    int i,j,x,y,alpha,beta,finished;

    // Variable Initialization
    x = 0;
    y = 0;
    finished = 0;
    alpha = 10;
    beta = 3;

    for(i = 0; i < alpha; i++)
    {
        for(j = 0; j < beta; j++)
        {
            y = modify_value(y);
        }
    }

    finished = 1;
}

int modify_value(int z)
{
    //int w;
    z = z + 1;

    return (z);

}

