/* @(#)w_sqrt.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * wrapper sqrt(x)
 */

#include "MSL_C/MSL_Common_Embedded/Math/fdlibm.h"

#ifdef __STDC__
double sqrt(double x) /* wrapper sqrt */
#else
double sqrt(x) /* wrapper sqrt */
double x;
#endif
{
    return __ieee754_sqrt(x);
}
