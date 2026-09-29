/* @(#)w_hypot.c 1.3 95/01/18 */
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
 * wrapper hypot(x, y)
 */

#include "MSL_C/MSL_Common_Embedded/Math/fdlibm.h"

#ifdef __STDC__
double hypot(double x, double y) /* wrapper hypot */
#else
double hypot(x, y) /* wrapper hypot */
double x, y;
#endif
{
    return __ieee754_hypot(x, y);
}
