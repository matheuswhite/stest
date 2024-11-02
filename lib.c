/**
 * @file lib.c
 * @author Matheus T. dos Santos (matheuswhite1@protonmail.com)
 * @brief
 * @version 0.1
 * @date 01/11/24
 *
 * @copyright Copyright (c) 2024 Citrinio All rights reserved.
 */
#include "stest.h"

#include <stdlib.h>

SSUITE(suite1);

STEST(suite1, test_add) {
    int *a = NULL;
    *a = 1;
    return 0;
}

STEST(suite1, test_sub) { return 0; }

SSUITE(suite2);

STEST(suite2, test_mul) { return 3 * 2; }
