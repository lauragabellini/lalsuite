/*
 * Copyright (C) 2008 J. Creighton, K. Cannon
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <lal/LALSimBurst.h>
#include <lal/LALConstants.h>
#include <lal/FrequencySeries.h>
#include <lal/TimeFreqFFT.h>
#include <lal/TimeSeries.h>
#include <lal/RealFFT.h>
#include <lal/Units.h>
#include <lal/Date.h>


/*
 * ============================================================================
 *
 *          Fill a time series with stationary white Gaussian noise
 *
 * ============================================================================
 */


static void gaussian_noise(REAL8TimeSeries * series, REAL8 rms, gsl_rng * rng)
{
	unsigned i;

	for(i = 0; i < series->data->length; i++)
		series->data->data[i] = gsl_ran_gaussian(rng, rms);
}


/*
 * ============================================================================
 *
 *                                 Utilities
 *
 * ============================================================================
 */


/*
 * Returns the strain of the sample with the largest magnitude.
 */


static REAL8 XLALMeasureHPeak(REAL8TimeSeries *series)
{
	static const char func[] = "XLALMeasureHPeak";
	double hpeak;
	unsigned i;

	if(!series->data->length)
		XLAL_ERROR_REAL8(func, XLAL_EBADLEN);

	hpeak = series->data->data[0];
	for(i = 1; i < series->data->length; i++)
		if(fabs(series->data->data[i]) > fabs(hpeak))
			hpeak = series->data->data[i];

	return hpeak;
}


/*
 * Returns the root-sum-square strain.
 */


static REAL8 XLALMeasureHrss(REAL8TimeSeries *series)
{
	double e = 0.0;
	double sum = 0.0;
	unsigned i;

	/* Kahans's compensated summation algorithm */

	for(i = 0; i < series->data->length; i++) {
		double tmp = sum;
		/* what we want to add = h^{2} + "error from last
		 * iteration" */
		double x = series->data->data[i] * series->data->data[i] + e;
		/* add */
		sum += x;
		/* negative of what was actually added */
		e = tmp - sum;
		/* what didn't get added, add next time */
		e += x;
	}

	return sqrt(sum);
}


/*
 * Given the Fourier transform of a real-valued function h(t), compute and
 * return the integral of the square of its derivative:
 *
 * \int \dot{h}^{2} \diff t.
 *
 * The normalization factors in this function assume that
 * XLALREAL8FreqTimeFFT() will be used to convert the frequency series to
 * the time domain.
 */


static REAL8 XLALMeasureIntHDotSquaredDT(COMPLEX16FrequencySeries *fseries)
{
	unsigned i;
	double e = 0.0;
	double sum = 0.0;

	/* Kahan's compensated summation algorithm. The summation is done
	 * from lowest to highest frequency under the assumption that high
	 * frequency components tend to add more to the magnitude of the
	 * derivative.  */

	for(i = 0; i < fseries->data->length; i++) {
		double tmp = sum;
		/* what we want to add = f^{2} |\tilde{s}(f)|^{2} + "error
		 * from last iteration" */
		double x = (fseries->f0 + i * fseries->deltaF) * (fseries->f0 + i * fseries->deltaF) * (fseries->data->data[i].re * fseries->data->data[i].re + fseries->data->data[i].im * fseries->data->data[i].im) + e;
		/* add */
		sum += x;
		/* negative of what was actually added */
		e = tmp - sum;
		/* what didn't get added, add next time */
		e += x;
	}

	/* because we've only summed the positive frequency components */

	sum *= 2;

	/* 4 \pi^{2} \delta f */

	sum *= LAL_TWOPI * LAL_TWOPI * fseries->deltaF;

	return sum;
}


/*
 * ============================================================================
 *
 *            Construct a Band- and Time-Limited White Noise Burst
 *
 * ============================================================================
 */


/*
 * Parameters:
 *
 * duration
 * 	width of time domain Gaussian envelope in seconds
 * frequency
 * 	centre frequency of waveform in Hertz
 * bandwidth
 * 	width of frequency domain Gaussian envelope in Hertz
 * int_hdot_squared
 * 	waveform is normalized so that \int (\dot{h}_{+}^{2} +
 * 	\dot{h}_{\times}^{2}) \diff t equals this
 * delta_t
 * 	the sample rate of the time series to construct
 * rng
 * 	a GSL random number generator to be used to produce Gaussian random
 * 	variables
 *
 * Output:
 *
 * Two time series containing h+(t) and hx(t), with the time-domain
 * Gaussian envelope's peak located at t = 0 (as defined by the epoch and
 * deltaT).  The + and x time series are two independent injections.
 *
 * Note:  because the injection is constructed with a random number
 * generator, any changes to this function that change how random numbers
 * are chosen will indirectly have the effect of altering the relationship
 * between injection waveform and random number seed.  For example,
 * increasing the length of the time series will change the injection
 * waveforms.  There's nothing wrong with this, the waveforms are still
 * correct, but if there is a need to reproduce a waveform exactly then it
 * will be necessary to tag the code before making such changes.
 */


int XLALGenerateBandAndTimeLimitedWhiteNoiseBurst(REAL8TimeSeries **hplus, REAL8TimeSeries **hcross, REAL8 duration, REAL8 frequency, REAL8 bandwidth, REAL8 int_hdot_squared, REAL8 delta_t, gsl_rng *rng)
{
	static const char func[] = "XLALGenerateBandAndTimeLimitedWhiteNoiseBurst";
	int length;
	LIGOTimeGPS epoch;
	COMPLEX16FrequencySeries *tilde_hplus, *tilde_hcross;
	REAL8Window *window;
	REAL8FFTPlan *plan;
	REAL8 norm_factor;
	REAL8 sigma_t_squared = duration * duration / 4.0 - 1.0 / (LAL_PI * LAL_PI * bandwidth * bandwidth);
	unsigned i;

	/* check input.  checking if sigma_t_squared < 0 is equivalent to
	 * checking if duration * bandwidth < LAL_2_PI */

	if(duration < 0 || bandwidth < 0 || sigma_t_squared < 0 || int_hdot_squared < 0 || delta_t <= 0) {
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EINVAL);
	}

	/* length of the injection time series is 10 * duration, rounded to
	 * the nearest odd integer */

	length = (int) (10.0 * duration / delta_t / 2.0);
	length = 2 * length + 1;

	/* the middle sample is t = 0 */

	XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * delta_t);

	/* allocate the time series */

	*hplus = XLALCreateREAL8TimeSeries("BTLWNB +", &epoch, 0.0, delta_t, &lalStrainUnit, length);
	*hcross = XLALCreateREAL8TimeSeries("BTLWNB x", &epoch, 0.0, delta_t, &lalStrainUnit, length);
	if(!*hplus || !*hcross) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}

	/* fill with independent zero-mean unit variance Gaussian random
	 * numbers (any non-zero amplitude is OK, it will be adjusted
	 * later) */

	gaussian_noise(*hplus, 1, rng);
	gaussian_noise(*hcross, 1, rng);

	/* apply the time-domain Gaussian window.  the window function's
	 * shape parameter is ((length - 1) * delta_t / 2) / \sigma_{t} where
	 * \sigma_{t} is the compensated time-domain window duration */

	window = XLALCreateGaussREAL8Window((*hplus)->data->length, (((*hplus)->data->length - 1) * delta_t / 2) / sqrt(sigma_t_squared));
	if(!window) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	for(i = 0; i < window->data->length; i++) {
		(*hplus)->data->data[i] *= window->data->data[i];
		(*hcross)->data->data[i] *= window->data->data[i];
	}
	XLALDestroyREAL8Window(window);

	/* transform to the frequency domain */

	plan = XLALCreateForwardREAL8FFTPlan((*hplus)->data->length, 0);
	tilde_hplus = XLALCreateCOMPLEX16FrequencySeries(NULL, &epoch, 0.0, 0.0, &lalDimensionlessUnit, (*hplus)->data->length / 2 + 1);
	tilde_hcross = XLALCreateCOMPLEX16FrequencySeries(NULL, &epoch, 0.0, 0.0, &lalDimensionlessUnit, (*hplus)->data->length / 2 + 1);
	if(!plan || !tilde_hplus || !tilde_hcross) {
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hplus);
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hcross);
		XLALDestroyREAL8FFTPlan(plan);
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	i = XLALREAL8TimeFreqFFT(tilde_hplus, *hplus, plan);
	i |= XLALREAL8TimeFreqFFT(tilde_hcross, *hcross, plan);
	XLALDestroyREAL8FFTPlan(plan);
	if(i) {
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hplus);
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hcross);
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}

	/* apply the frequency-domain Gaussian window.  the window
	 * function's shape parameter is computed similarly to that of the
	 * time-domain window, with \sigma_{f} = \Delta f / 2.  the window
	 * is created with its peak on the middle sample, which we need to
	 * shift to the sample corresponding to the injection's centre
	 * frequency. */

	window = XLALCreateGaussREAL8Window(2 * tilde_hplus->data->length + 1, (tilde_hplus->data->length * tilde_hplus->deltaF) / (bandwidth / 2.0));
	if(!window) {
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hplus);
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hcross);
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	XLALResizeREAL8Sequence(window->data, tilde_hplus->data->length - (unsigned) floor(frequency / tilde_hplus->deltaF + 0.5), tilde_hplus->data->length);
	for(i = 0; i < window->data->length; i++) {
		tilde_hplus->data->data[i].re *= window->data->data[i];
		tilde_hplus->data->data[i].im *= window->data->data[i];
		tilde_hcross->data->data[i].re *= window->data->data[i];
		tilde_hcross->data->data[i].im *= window->data->data[i];
	}
	XLALDestroyREAL8Window(window);

	/* normalize the waveform to achieve the desired \int
	 * (\dot{h}_{+}^{2} + \dot{h}_{\times}^{2}) dt */

	norm_factor = sqrt(int_hdot_squared / (XLALMeasureIntHDotSquaredDT(tilde_hplus) + XLALMeasureIntHDotSquaredDT(tilde_hcross)));
	for(i = 0; i < tilde_hplus->data->length; i++) {
		tilde_hplus->data->data[i].re *= norm_factor;
		tilde_hplus->data->data[i].im *= norm_factor;
		tilde_hcross->data->data[i].re *= norm_factor;
		tilde_hcross->data->data[i].im *= norm_factor;
	}

	/* transform to the time domain */

	plan = XLALCreateReverseREAL8FFTPlan((*hplus)->data->length, 0);
	if(!plan) {
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hplus);
		XLALDestroyCOMPLEX16FrequencySeries(tilde_hcross);
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	i = XLALREAL8FreqTimeFFT(*hplus, tilde_hplus, plan);
	i |= XLALREAL8FreqTimeFFT(*hcross, tilde_hcross, plan);
	XLALDestroyREAL8FFTPlan(plan);
	XLALDestroyCOMPLEX16FrequencySeries(tilde_hplus);
	XLALDestroyCOMPLEX16FrequencySeries(tilde_hcross);
	if(i) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}

	/* apply a Tukey window for continuity at the start and end of the
	 * injection.  the window's shape parameter sets what fraction of
	 * the window is used by the tapers */

	window = XLALCreateTukeyREAL8Window((*hplus)->data->length, 0.5);
	if(!window) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	for(i = 0; i < window->data->length; i++) {
		(*hplus)->data->data[i] *= window->data->data[i];
		(*hcross)->data->data[i] *= window->data->data[i];
	}
	XLALDestroyREAL8Window(window);

	/* done */

	return 0;
}


/*
 * ============================================================================
 *
 *                         Sine-Gaussian and Friends
 *
 * ============================================================================
 */


/*
 * note: eccentricity and polarization are angles in waveform hrss space
 * plus is always a cosine gaussian; cross is always a sine gaussian
 */


int XLALSimBurstSineGaussian(REAL8TimeSeries **hplus, REAL8TimeSeries **hcross, LIGOTimeGPS *epoch, REAL8 deltaT, REAL8 Q, REAL8 f0, REAL8 hrss, REAL8 eccentricity, REAL8 polarization)
{
	LIGOTimeGPS start;
	REAL8 duration = 100.0 * Q / f0; /* CHECKME: long enough ??? */
	UINT4 length = floor( 0.5 + duration / deltaT );
	REAL8 a;
	REAL8 b;
	REAL8 cgrss;
	REAL8 sgrss;
	REAL8 hplusrss;
	REAL8 hcrossrss;

	REAL8 h0plus;
	REAL8 h0cross;
	UINT4 j;

	/* semimajor and semiminor axes of waveform ellipsoid */
	a = hrss / sqrt( 2.0 - eccentricity * eccentricity );
	b = a * sqrt( 1.0 - eccentricity * eccentricity );
	
	/* rss of plus and cross polarizations */
	hplusrss  = a * cos( polarization ) - b * sin( polarization );
	hcrossrss = b * cos( polarization ) + a * sin( polarization );

	/* rss of unit amplitude cosine- and sine-gaussian waveforms */
	/* see: K. Riles, LIGO-T040055-00.pdf */
	cgrss = sqrt( (Q/(4.0*f0*sqrt(LAL_PI))) * (1.0 + exp(-Q*Q)) );
	sgrss = sqrt( (Q/(4.0*f0*sqrt(LAL_PI))) * (1.0 - exp(-Q*Q)) );

	/* "peak" amplitudes of plus and cross */
	h0plus  = hplusrss / cgrss;
	h0cross = hplusrss / sgrss;

	/* update length to be even, correct duration, and shift start time */
	length = length % 2 ? length + 1 : length;
	duration = length * deltaT;
	start = *epoch;
	XLALGPSAdd( &start, -0.5 * duration );
	
	*hplus = XLALCreateREAL8TimeSeries( "H_PLUS", &start, 0.0, deltaT, &lalStrainUnit, length );
	*hcross = XLALCreateREAL8TimeSeries( "H_CROSS", &start, 0.0, deltaT, &lalStrainUnit, length );

	for ( j = 0; j < length; ++j ) {
		REAL4 t   = j * deltaT - 0.5 * duration;
		REAL4 phi = LAL_TWOPI * f0 * t;
		REAL4 fac = exp( -0.5*phi*phi/(Q*Q) );
		(*hplus)->data->data[j]  = h0plus * fac * cos( phi );
		(*hcross)->data->data[j] = h0cross * fac * sin( phi );
	}

	return 0;
}


/*
 * ============================================================================
 *
 *                                String Cusp
 *
 * ============================================================================
 */


/*
 * Input:
 *	amplitude = waveform's amplitude parameter
 *	f_high = high frequency cutoff
 *	delta_t = sample period of output time series
 *
 * Output:
 * 	h(t) with waveform peak at t = 0 (as defined by the epoch and
 * 	deltaT).
 *
 * The low frequency cutoff is fixed at 1 Hz;  there's nothing special
 * about 1 Hz except that it is low compared to the frequency at which we
 * should be high-passing the data
 */


int XLALGenerateStringCusp(REAL8TimeSeries **hplus, REAL8TimeSeries **hcross, REAL8 amplitude, REAL8 f_high, REAL8 delta_t)
{
	static const char func[] = "XLALGenerateStringCusp";
	COMPLEX16FrequencySeries *tilde_h;
	REAL8FFTPlan *plan;
	LIGOTimeGPS epoch;
	int length;
	int i;
	const double f_low = 1.0;

	/* check input */

	if(amplitude < 0 || f_high < f_low || delta_t <= 0) {
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EINVAL);
	}

	/* length of the injection time series is 5 / f_low, rounded to the
	 * nearest odd integer */

	length = (int) (5 / f_low / delta_t / 2.0);
	length = 2 * length + 1;

	/* the middle sample is t = 0 */

	XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * delta_t);

	/* allocate time and frequency series and FFT plan */

	*hplus = XLALCreateREAL8TimeSeries("string cusp +", &epoch, 0.0, delta_t, &lalStrainUnit, length);
	*hcross = XLALCreateREAL8TimeSeries("string cusp x", &epoch, 0.0, delta_t, &lalStrainUnit, length);
	tilde_h = XLALCreateCOMPLEX16FrequencySeries("string cusp", &epoch, 0.0, 1.0 / (length * delta_t), &lalDimensionlessUnit, length / 2 + 1);
	plan = XLALCreateReverseREAL8FFTPlan(length, 0);
	if(!*hplus || !*hcross || !tilde_h || !plan) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		XLALDestroyCOMPLEX16FrequencySeries(tilde_h);
		XLALDestroyREAL8FFTPlan(plan);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}
	XLALUnitMultiply(&tilde_h->sampleUnits, &(*hplus)->sampleUnits, &lalSecondUnit);

	/* zero the cross time series, injection is done in + only */

	memset((*hcross)->data->data, 0, (*hcross)->data->length * sizeof(*(*hcross)->data->data));

	/* construct the waveform in the frequency domain */

	for(i = 0; (unsigned) i < tilde_h->data->length; i++) {
		double f = i * tilde_h->deltaF;

		/* frequency-domain wave form */

		tilde_h->data->data[i].re = amplitude * pow((sqrt(1 + f_low * f_low / (f * f))), -8) * pow(f, -4.0 / 3.0);
		if(f > f_high)
			tilde_h->data->data[i].re *= exp(1 - f / f_high);
		tilde_h->data->data[i].im = tilde_h->data->data[i].re;

		/* phase shift to put waveform's peak on the middle sample
		 * of the time series */

		tilde_h->data->data[i].im *= sin(-LAL_PI * i * (length - 1) / length);
		tilde_h->data->data[i].re *= cos(-LAL_PI * i * (length - 1) / length);
	}

	/* set DC to zero */

	tilde_h->data->data[0].re = 0;
	tilde_h->data->data[0].im = 0;

	/* set Nyquist to zero */

	tilde_h->data->data[tilde_h->data->length - 1].re = 0;
	tilde_h->data->data[tilde_h->data->length - 1].im = 0;

	/* transform to time domain */

	i = XLALREAL8FreqTimeFFT(*hplus, tilde_h, plan);
	XLALDestroyCOMPLEX16FrequencySeries(tilde_h);
	XLALDestroyREAL8FFTPlan(plan);
	if(i) {
		XLALDestroyREAL8TimeSeries(*hplus);
		XLALDestroyREAL8TimeSeries(*hcross);
		*hplus = *hcross = NULL;
		XLAL_ERROR(func, XLAL_EFUNC);
	}

	/* apodize the time series */

	for(i = (*hplus)->data->length - 1; i >= 0; i--)
		(*hplus)->data->data[i] -= (*hplus)->data->data[0];

	/* done */

	return 0;
}
