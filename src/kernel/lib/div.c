/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/**
 * @details Divides two integers.
 */
int __div(int a, int b)
{
    int n = 0;

    while (a > b) {
        a -= b;
        n++;
    }

    return (n);
}
