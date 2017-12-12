#include <math.h>
#include "window.h"


namespace namespace_utilfunc
{

	REAL I0Bessel (const REAL x)
{
    REAL ax, y;

    if (( ax = fabs( x)) < 3.75)
    {
		y = (REAL)(x / 3.75);
		y *= y;
		return ((REAL)(1.0 + y * ( 3.5156229 + y * ( 3.0899424 + y * ( 1.2067492 +
				y * ( 0.2659732 + y * ( 0.360768e-1 + y * 0.45813e-2)))))));
    }
    else
    {
		y = 3.75f / ax;
		return ((REAL)((exp ( ax) / sqrt(ax)) * (0.39894228 + y * (0.1328592e-1 + y * (0.225319e-2 +
				y * (-0.157565e-2 + y * (0.916281e-2 + y * (-0.2057706e-1 +
				y * (0.2635537e-1 + y * (-0.1647633e-1 + y * 0.392377e-2))))))))));
    }
}




COREDSP_API void window_func_real ( REAL* data, const enum WindowFuncType windowType, REAL coeff, ulong length )
{
	ulong i;
	REAL theta, theta_inc, z;

	theta_inc = (REAL)(2.0 * M_PI / (((REAL)length) - 1.0));
	theta = 0.0;

	switch (windowType)
	{
		case WND_HANNING :				/* Hanning window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= (REAL)(0.5 * (1.0 - (REAL)cos((double)theta)));
			}
			return;

		case WND_HAMMING :				/* Hamming window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= (REAL)(0.54 - 0.46 * (REAL)cos((double)theta));
			}
			return;

		case WND_BLACKMAN :				/* Blackman window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= (REAL)(0.42659 - 0.49656 * (REAL)cos((double)theta) + 0.076849 * (REAL)cos(2.0 * (double)theta));
			}
			return;

		case WND_BARTLETT :				/* Bartlett window */

			for (i = 0; i < length; i++)
				data[i] *= (REAL)((2.0 / ((REAL)length - 1.0)) * 
					(((REAL)length - 1.0) / 2.0 - fabs(i - ((REAL)length - 1.0) / 2.0)));
			return;

		case WND_KAISER :				/* Kaiser window */
		    z = -(REAL)((((REAL)length) - 1.0) / 2.0);
			for (i = 0; i < length; i++, z++)
			{
				data[i] *= (REAL)(I0Bessel ((REAL)(coeff *
					sqrt (1.0 - pow ((2.0 * z / (length - 1)), 2.0)))) /
					I0Bessel (coeff));
			}
			return;

		case WND_BMAN_HARRIS :			/* 4 th order Blackman-Harris window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= (REAL)(0.35875 - 0.48829 * (REAL)cos((double)theta) + 0.14128 * (REAL)cos(2.0 * (double)theta) - 0.01168 * (REAL)cos(3.0 * (double)theta));
			}
			return;

		case WND_RECTANGLE :				/* Rectangle window */
			return;

		case WND_FLAT_TOP :				/* Flat top window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				//data[i] *= 0.2810638602f - 0.5208971735f * (REAL)cos((double)theta) + 0.1980389663f * (REAL)cos(2.0 * (double)theta);
				data[i] *= (REAL)(1.0 - 1.93 * (REAL)cos((double)theta) + 1.29 * (REAL)cos(2.0 * (double)theta) - 0.388 * (REAL)cos(3.0 * (double)theta) + 0.032 * (REAL)cos(4.0 * (double)theta));
			}
			return;
	}
}


}