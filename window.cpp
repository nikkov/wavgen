#include <wx/wx.h>
#include <wx/defs.h>

#include <math.h>

#include "fft_wrapper.h"


#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif


float I0Bessel (const float x)

{
    float ax, y;

    if (( ax = fabs( x)) < 3.75f)
    {
		y = x / 3.75f;
		y *= y;
		return ((float)(1.0f + y * ( 3.5156229 + y * ( 3.0899424 + y * ( 1.2067492 +
				y * ( 0.2659732 + y * ( 0.360768e-1 + y * 0.45813e-2)))))));
    }
    else
    {
		y = 3.75f / ax;
		return ((float)((exp ( ax) / sqrt(ax)) * (0.39894228 + y * (0.1328592e-1 + y * (0.225319e-2 +
				y * (-0.157565e-2 + y * (0.916281e-2 + y * (-0.2057706e-1 +
				y * (0.2635537e-1 + y * (-0.1647633e-1 + y * 0.392377e-2))))))))));
    }
}


void WindowFunc (float* data, const enum WindowFuncType windowType, float coeff, const int length)

{
	int i;
	float theta, theta_inc, z;

	theta_inc = ((float)(2.0 * M_PI)) / (((float)length) - 1.0f);
	theta = 0.0f;

	switch (windowType)
	{
		case WND_HANNING :				/* Hanning window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= 0.5f * (1.0f - (float)cos((double)theta));
			}
			return;

		case WND_HAMMING :				/* Hamming window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= 0.54f - 0.46f * (float)cos((double)theta);
			}
			return;

		case WND_BLACKMAN :				/* Blackman window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= 0.42659f - 0.49656f * (float)cos((double)theta) + 0.076849f * (float)cos(2.0 * (double)theta);
			}
			return;

		case WND_BARTLETT :				/* Bartlett window */

			for (i = 0; i < length; i++)
				data[i] *= (2.0f / ((float)length - 1.0f)) * 
					(((float)length - 1.0f) / 2.0f - fabs(i - ((float)length - 1.0f) / 2.0f));
			return;

		case WND_KAISER :				/* Kaiser window */
		    z = -((((float)length) - 1.0f) / 2.0f);
			for (i = 0; i < length; i++, z++)
			{
				data[i] *= (I0Bessel ((coeff *
					sqrt (1.0f - pow ((2.0f * z / (length - 1)), 2.0f)))) /
					I0Bessel (coeff));
			}
			return;

		case WND_BMAN_HARRIS :			/* 4 th order Blackman-Harris window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				data[i] *= 0.35875f - 0.48829f * (float)cos((double)theta) + 0.14128f * (float)cos(2.0 * (double)theta) - 0.01168f * (float)cos(3.0 * (double)theta);
			}
			return;

		case WND_RECTANGLE :				/* Rectangle window */
			return;

		case WND_FLAT_TOP :				/* Flat top window */
			for (i = 0; i < length; i++, theta += theta_inc)
			{
				//data[i] *= 0.2810638602f - 0.5208971735f * (float)cos((double)theta) + 0.1980389663f * (float)cos(2.0 * (double)theta);
				data[i] *= 1.0f - 1.93f * (float)cos((double)theta) + 1.29f * (float)cos(2.0 * (double)theta) - 0.388f * (float)cos(3.0 * (double)theta) + 0.032f * (float)cos(4.0 * (double)theta);
			}
			return;
	}
}	

