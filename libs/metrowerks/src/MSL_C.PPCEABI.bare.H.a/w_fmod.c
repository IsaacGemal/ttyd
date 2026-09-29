/* @(#)w_fmod.c 1.3 95/01/18 */
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
 * wrapper fmod(x, y)
 */

#include "MSL_C/MSL_Common_Embedded/Math/fdlibm.h"

#ifdef __STDC__
double fmod(double x, double y) /* wrapper fmod */
#else
double fmod(x, y) /* wrapper fmod */
double x, y;
#endif
{
    return __ieee754_fmod(x, y);
}
