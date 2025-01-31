/*
*  Copyright (C) 2014 Jolien Creighton
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*  MA  02110-1301  USA
*/

/**
 * @defgroup lalsim_inspiral lalsim-inspiral
 * @ingroup lalsimulation_programs
 *
 * @brief Simulates a gravitational waveform from binary inspiral
 *
 * ### Synopsis
 *
 *     lalsim-inspiral [options]
 *
 * ### Description
 *
 * The `lalsim-inspiral` utility produces a stream of a simulated
 * gravitational waveform from a binary inspiral.  The output data is
 * the gravitational waveform polarizations in the time domain, or
 * in the frequency domain if the `-F` option is specified.  If the
 * option `-Q` is specified, the output data is in amplitude and phase.
 * This program uses XLALSimInspiralChooseTDWaveform() or
 * XLALSimInspiralChooseFDWaveform() unless the `-c` waveform contitioning
 * option is given, in which case it uses XLALSimInspiralTD() or
 * XLALSimInspiralFD().  The output is written to standard output as a
 * multicolumn ascii format.  The first column gives the time or frequency
 * corresponding to each sample and the remaining columns give the
 * gravitational waveform values for the two polarizations (real and
 * imaginary parts, or amplitude and phase when complex).
 *
 * ### Options
 * [default values in brackets]
 *
 * <DL>
 * <DT>`-h`, `--help`
 * <DD>print a help message and exit</DD>
 * <DT>`-v`, `--verbose`
 * <DD>verbose output</DD>
 * <DT>`-C`, `--radians`</DT>
 * <DD>use radians rather than decimal degrees</DD>
 * <DT>`-F`, `--frequency-domain`
 * <DD>output data in frequency domain</DD>
 * <DT>`-c`, `--condition-waveform`
 * <DD>apply waveform conditioning</DD>
 * <DT>`-P`, `--amp-phase`
 * <DD>output data as amplitude and phase</DD>
 * <DT>`-a` APPROX, `--approximant=`APPROX
 * <DD>approximant [TaylorT1]</DD>
 * <DT>`-w` WAVEFORM, `--waveform=`WAVEFORM
 * <DD>waveform string giving both approximant and order</DD>
 * <DT>`-D` domain, `--domain=`DOMAIN
 * <DD>domain for waveform generation when both are available {"time", "freq"}
 * [use natural domain for output]</DD>
 * <DT>`-O` PHASEO, `--phase-order=`PHASEO
 * <DD>twice pN order of phase (-1 == highest) [-1]</DD>
 * <DT>`-o` AMPO, `--amp-order=`AMPO
 * <DD>twice pN order of amplitude (-1 == highest) [0]</DD>
 * <DT>`-u` PHIREF, `--phiRef=`PHIREF
 * <DD>reference phase in degrees [0]</DD>
 * <DT>`-U` PERIANOM, `--periastron-anomaly=`PERIANOM
 * <DD>mean periastron anomaly in degrees [0]</DD>
 * <DT>`-W` LONGASC, `--longitude-ascending-node=`LONGASC
 * <DD>longitude of ascending node in degrees [0]</DD>
 * <DT>`-e` ECC, `--eccentricity=`ECC
 * <DD>orbital eccentricity [0]</DD>
 * <DT>`-R` SRATE, `--sample-rate=`SRATE
 * <DD>sample rate in Hertz [16384]</DD>
 * <DT>`-M` M1, `--m1=`M1
 * <DD>mass of primary in solar masses [1.4]</DD>
 * <DT>`-m` M2, `--m2=`M2
 * <DD>mass of secondary in solar masses [1.4]</DD>
 * <DT>`-d` D, `--distance=`D
 * <DD>distance in Mpc [1]</DD>
 * <DT>`-i` IOTA, `--inclination=`IOTA
 * <DD>inclination in degrees [0]</DD>
 * <DT>`-X` S1X, `--spin1x=`S1X
 * <DD>x-component of dimensionless spin of primary [0]</DD>
 * <DT>`-Y` S1Y, `--spin1y=`S1Y
 * <DD>y-component of dimensionless spin of primary [0]</DD>
 * <DT>`-Z` S1Z, `--spin1z=`S1Z
 * <DD>z-component of dimensionless spin of primary [0]</DD>
 * <DT>`-x` S2X, `--spin2x=`S2X
 * <DD>x-component of dimensionless spin of secondary [0]</DD>
 * <DT>`-y` S2Y, `--spin2y=`S2Y
 * <DD>y-component of dimensionless spin of secondary [0]</DD>
 * <DT>`-z` S2Z, `--spin2z=`S2Z
 * <DD>z-component of dimensionless spin of secondary [0]</DD>
 * <DT>`-L` LAM1, `--tidal-lambda1=`LAM1
 * <DD>dimensionless tidal deformability of primary [0]</DD>
 * <DT>`-l` LAM2, `--tidal-lambda2=`LAM2
 * <DD>dimensionless tidal deformability of secondary [0]</DD>
 * <DT>`-q` DQM1, `--delta-quad-mon1=`DQM1
 * <DD>difference in quadrupole-monopole term of primary [0]</DD>
 * <DT>`-Q` DQM2, `--delta-quad-mon2=`DQM2
 * <DD>difference in quadrupole-monopole term of secondary [0]</DD>
 * <DT>`-s` SPINO, `--spin-order=`SPINO
 * <DD>twice pN order of spin effects (-1 == all) [-1]</DD>
 * <DT>`-t` TIDEO, `--tidal-order=`TIDEO
 * <DD>lambdaG [0]</DD>
 * <DT>`-H` ZERO, `--Zero-Parameter=`ZERO
 * <DD>twice pN order of tidal effects (-1 == all) [-1]</DD>
 * <DT>`-f` FMIN, `--f-min=`FMIN
 * <DD>frequency to start waveform in Hertz [40]</DD>
 * <DT>`-r` FREF, `--fRef=`FREF
 * <DD>reference frequency in Hertz [0]</DD>
 * <DT>`-A` AXIS, `--axis=`AXIS
 * <DD>axis for PhenSpin {View, TotalJ, OrbitalL} [OrbitalL]</DD>
 * <DT>`-n` MODES, `--modes=`MODES
 * <DD>allowed l modes {L2, L23, ..., ALL} [L2]</DD>
 * <DT>`-p` KEY1`=`VAL1`,`KEY2`=`VAL2,...,
 * `--params=`KEY1`=`VAL1`,`KEY2`=`VAL2,...</DT>
 * <DD>extra parameters as a key-value pair</DD>
 * </DL>
 *
 * ### Environment
 *
 * The `LAL_DEBUG_LEVEL` can used to control the error and warning reporting of
 * `lalsim-inspiral`.  Common values are: `LAL_DEBUG_LEVEL=0` which suppresses
 * error messages, `LAL_DEBUG_LEVEL=1`  which prints error messages alone,
 * `LAL_DEBUG_LEVEL=3` which prints both error messages and warning messages,
 * and `LAL_DEBUG_LEVEL=7` which additionally prints informational messages.
 *
 * ### Exit Status
 *
 * The `lalsim-inspiral` utility exits 0 on success, and >0 if an error occurs.
 *
 * ### Example
 *
 * The command:
 *
 *     lalsim-inspiral --approx=TaylorT3
 *
 * produces a three-column ascii output to standard output; the rows are
 * samples (at the default rate of 16384 Hz), and the three columns are 1. the
 * time of each sample, 2. the plus-polarization strain, and 3.  the
 * cross-polarization strain.  The waveform produced is for the TaylorT3
 * post-Newtonian approximant for the default parameters of a 1.4 solar mass +
 * 1.4 solar mass binary inspiral at 1 Mpc distance.
 *
 * The command:
 *
 *     lalsim-inspiral --m1=10 --m2=10 --approx=TaylorF2 --frequency-domain
 *
 * produces a frequency-domain waveform for a 10 solar mass + 10 solar mass
 * binary inspiral at 1 Mpc distance using the TaylorF2 approximant.  The five
 * columns written to standard output are the frequency of each sample, the
 * real part of the plus-polarization, the imaginary part of the
 * plus-polarization, the real part of the cross-polarization, and the
 * imaginary part of the cross-polarization.
 *
 * The command:
 *
 *     lalsim-inspiral --m1=10 --m2=10 --approx=TaylorF2 --condition
 *
 * produces the same waveform as in the previous example, but in the
 * time domain and conditioned so that it is suitable for injection
 * into detector data.
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lal/Date.h>
#include <lal/LALgetopt.h>
#include <lal/Units.h>
#include <lal/Sequence.h>
#include <lal/TimeSeries.h>
#include <lal/FrequencySeries.h>
#include <lal/TimeFreqFFT.h>
#include <lal/BandPassTimeSeries.h>
#include <lal/VectorOps.h>
#include <lal/LALConstants.h>
#include <lal/LALDatatypes.h>
#include <lal/LALError.h>
#include <lal/LALString.h>
#include <lal/LALDict.h>
#include <lal/LALSimInspiral.h>
#include <lal/LALSimIMR.h>
#include <math.h>

/* default values of parameters */
#define DEFAULT_APPROX "TaylorT1"
#define DEFAULT_DOMAIN -1
#define DEFAULT_PHASEO -1
#define DEFAULT_AMPO -1
#define DEFAULT_PHIREF 0.0
#define DEFAULT_MEANPERANO 0.0
#define DEFAULT_LONGASCNODE 0.0
#define DEFAULT_ECCENTRICITY 0.0
#define DEFAULT_FREF 0.0
#define DEFAULT_SRATE 16384.0
#define DEFAULT_M1 1.4
#define DEFAULT_M2 1.4
#define DEFAULT_F_MIN 40.0
#define DEFAULT_DISTANCE 1.0
#define DEFAULT_INCLINATION 0.0
#define DEFAULT_S1X 0.0
#define DEFAULT_S1Y 0.0
#define DEFAULT_S1Z 0.0
#define DEFAULT_S2X 0.0
#define DEFAULT_S2Y 0.0
#define DEFAULT_S2Z 0.0
#define DEFAULT_LAMBDA1 0.0
#define DEFAULT_LAMBDA2 0.0
#define DEFAULT_DQUADMON1 0.0
#define DEFAULT_DQUADMON2 0.0
#define DEFAULT_LAMBDAG pow(10,15)

/* parameters given in command line arguments */
struct params {
    int verbose;
    int freq_dom;
    int condition;
    int amp_phase;
    Approximant approx;
    int domain;
    double phiRef;
    double meanPerAno;
    double longAscNodes;
    double eccentricity;
    double fRef;
    double srate;
    double m1;
    double m2;
    double f_min;
    double distance;
    double inclination;
    double s1x;
    double s1y;
    double s1z;
    double s2x;
    double s2y;
    double s2z;
    double lambdaG;
    LALDict *params;
};

int usage(const char *program);
struct params parseargs(int argc, char **argv);
int print_params(struct params params);
const char *frame_axis_to_string(LALSimInspiralFrameAxis axis);
const char *modes_choice_to_string(LALSimInspiralModesChoice modes);
int output_td_waveform(REAL8TimeSeries * h_plus, REAL8TimeSeries * h_cross, struct params params);
int output_fd_waveform(COMPLEX16FrequencySeries * htilde_plus, COMPLEX16FrequencySeries * htilde_cross, struct params params);
int create_td_waveform(REAL8TimeSeries ** h_plus, REAL8TimeSeries ** h_cross, struct params params);
int create_fd_waveform(COMPLEX16FrequencySeries ** htilde_plus, COMPLEX16FrequencySeries ** htilde_cross, struct params params);
double imr_time_bound(double f_min, double m1, double m2, double s1z, double s2z);

int main(int argc, char *argv[])
{
    struct params p;
    int istd, isfd;

    XLALSetErrorHandler(XLALBacktraceErrorHandler);

    p = parseargs(argc, argv);
    print_params(p);

    /* sanity check on domain; set to natural value if unspecified */
    istd = XLALSimInspiralImplementedTDApproximants(p.approx);
    isfd = XLALSimInspiralImplementedFDApproximants(p.approx);
    if (!istd && !isfd) {
        fprintf(stderr, "error: approximant not supported\n");
        exit(1);
    }
    switch (p.domain) {
    case LAL_SIM_DOMAIN_TIME:
        if (!istd) {
            fprintf(stderr, "error: approximant not supported in time domain\n");
            exit(1);
        }
        break;
    case LAL_SIM_DOMAIN_FREQUENCY:
        if (!isfd) {
            fprintf(stderr, "error: approximant not supported in frequency domain\n");
            exit(1);
        }
        break;
    default:
        switch (p.freq_dom) {
        case 0:
            p.domain = istd ? LAL_SIM_DOMAIN_TIME : LAL_SIM_DOMAIN_FREQUENCY;
            break;
        case 1:
            p.domain = isfd ? LAL_SIM_DOMAIN_FREQUENCY : LAL_SIM_DOMAIN_TIME;
            break;
        }
        break;
    }

    /* generate and output the waveform in appropriate domain */
    if (p.freq_dom) {
        COMPLEX16FrequencySeries *htilde_plus = NULL;
        COMPLEX16FrequencySeries *htilde_cross = NULL;
        create_fd_waveform(&htilde_plus, &htilde_cross, p);
        output_fd_waveform(htilde_plus, htilde_cross, p);
        XLALDestroyCOMPLEX16FrequencySeries(htilde_cross);
        XLALDestroyCOMPLEX16FrequencySeries(htilde_plus);
    } else {
        REAL8TimeSeries *h_plus = NULL;
        REAL8TimeSeries *h_cross = NULL;
        create_td_waveform(&h_plus, &h_cross, p);
        output_td_waveform(h_plus, h_cross, p);
        XLALDestroyREAL8TimeSeries(h_cross);
        XLALDestroyREAL8TimeSeries(h_plus);
    }

    /* cleanup */
    XLALDestroyDict(p.params);
    LALCheckMemoryLeaks();
    return 0;
}

/* writes a time-domain waveform to stdout as tab-separated values */
int output_td_waveform(REAL8TimeSeries * h_plus, REAL8TimeSeries * h_cross, struct params p)
{
    double t0;
    size_t j;
    t0 = XLALGPSGetREAL8(&h_plus->epoch);
    if (p.amp_phase) {
        REAL8Sequence *amp = XLALCreateREAL8Sequence(h_plus->data->length);
        REAL8Sequence *phi = XLALCreateREAL8Sequence(h_plus->data->length);
        double phi0;

        /* compute the amplitude and phase of h+ - i hx */
        for (j = 0; j < h_plus->data->length; ++j) {
            double complex z = h_plus->data->data[j] - I * h_cross->data->data[j];
            amp->data[j] = cabs(z);
            phi->data[j] = carg(z);
        }

        /* unwrap the phase */
        XLALREAL8VectorUnwrapAngle(phi, phi);

        /* make phase in range -pi to +pi at end of waveform */
        /* extrapolate the end of the waveform using last and second last points */
        phi0 = 2 * phi->data[phi->length - 1] - phi->data[phi->length - 2];
        // phi0 = phi->data[phi->length - 1];
        phi0 -= fmod(phi0 + copysign(LAL_PI, phi0), 2.0 * LAL_PI) - copysign(LAL_PI, phi0);
        for (j = 0; j < phi->length; ++j)
            phi->data[j] -= phi0;

        fprintf(stdout, "# time (s)\th_abs (strain)\t h_arg (rad)\n");
        for (j = 0; j < h_plus->data->length; ++j)
            fprintf(stdout, "%.9f\t%.18e\t%.18e\n", t0 + j * h_plus->deltaT, amp->data[j], phi->data[j]);

        XLALDestroyREAL8Sequence(phi);
        XLALDestroyREAL8Sequence(amp);
    } else {
        fprintf(stdout, "# time (s)\th_+ (strain)\th_x (strain)\n");
        for (j = 0; j < h_plus->data->length; ++j)
            fprintf(stdout, "%.9f\t%.18e\t%.18e\n", t0 + j * h_plus->deltaT, h_plus->data->data[j], h_cross->data->data[j]);
    }
    return 0;
}

/* writes a frequency-domain waveform to stdout as tab-separated values */
int output_fd_waveform(COMPLEX16FrequencySeries * htilde_plus, COMPLEX16FrequencySeries * htilde_cross, struct params p)
{
    size_t k;
    if (p.amp_phase) {
        REAL8Sequence *abs_plus = XLALCreateREAL8Sequence(htilde_plus->data->length);
        REAL8Sequence *arg_plus = XLALCreateREAL8Sequence(htilde_plus->data->length);
        REAL8Sequence *abs_cross = XLALCreateREAL8Sequence(htilde_cross->data->length);
        REAL8Sequence *arg_cross = XLALCreateREAL8Sequence(htilde_cross->data->length);
        double arg0;
        size_t kref;

        /* convert h+ and hx to polar */
        XLALCOMPLEX16VectorAbs(abs_plus, htilde_plus->data);
        XLALCOMPLEX16VectorArg(arg_plus, htilde_plus->data);
        XLALCOMPLEX16VectorAbs(abs_cross, htilde_cross->data);
        XLALCOMPLEX16VectorArg(arg_cross, htilde_cross->data);

        /* unwrap the phases */
        XLALREAL8VectorUnwrapAngle(arg_plus, arg_plus);
        XLALREAL8VectorUnwrapAngle(arg_cross, arg_cross);

        /* make sure that unwound phases are in -pi to pi at fRef */
        kref = round((p.fRef > 0 ? p.fRef : p.f_min) / htilde_plus->deltaF);
        arg0 = arg_plus->data[kref];
        arg0 -= fmod(arg0 + copysign(LAL_PI, arg0), 2.0 * LAL_PI) - copysign(LAL_PI, arg0);
        for (k = 0; k < arg_plus->length; ++k)
            arg_plus->data[k] -= arg0;

        arg0 = arg_cross->data[kref];
        arg0 -= fmod(arg0 + copysign(LAL_PI, arg0), 2.0 * LAL_PI) - copysign(LAL_PI, arg0);
        for (k = 0; k < arg_cross->length; ++k)
            arg_cross->data[k] -= arg0;

        fprintf(stdout, "# freq (s^-1)\tabs_htilde_+ (strain s)\targ_htilde_+ (rad)\tabs_htilde_x (strain s)\targ_htilde_x (rad)\n");
        for (k = 0; k < htilde_plus->data->length; ++k)
            fprintf(stdout, "%f\t%.18e\t%.18e\t%.18e\t%.18e\n", k * htilde_plus->deltaF, abs_plus->data[k], arg_plus->data[k],
                abs_cross->data[k], arg_cross->data[k]);

        XLALDestroyREAL8Sequence(arg_cross);
        XLALDestroyREAL8Sequence(abs_cross);
        XLALDestroyREAL8Sequence(arg_plus);
        XLALDestroyREAL8Sequence(abs_plus);
    } else {
        fprintf(stdout, "# freq (s^-1)\treal_htilde_+ (strain s)\timag_htilde_+ (strain s)\treal_htilde_x (strain s)\timag_htilde_x (strain s)\n");
        for (k = 0; k < htilde_plus->data->length; ++k)
            fprintf(stdout, "%f\t%.18e\t%.18e\t%.18e\t%.18e\n", k * htilde_plus->deltaF, creal(htilde_plus->data->data[k]),
                cimag(htilde_plus->data->data[k]), creal(htilde_cross->data->data[k]), cimag(htilde_cross->data->data[k]));
    }
    return 0;
}

/* creates a waveform in the time domain; the waveform might be generated in
 * the frequency-domain and transformed */
int create_td_waveform(REAL8TimeSeries ** h_plus, REAL8TimeSeries ** h_cross, struct params p)
{
    clock_t timer_start = 0;

    if (p.condition) {
        if (p.verbose) {
            fprintf(stderr, "generating waveform in time domain using XLALSimInspiralTD...\n");
            timer_start = clock();
        }
        XLALSimInspiralTD(h_plus, h_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, 1.0 / p.srate, p.f_min, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);
    } else if (p.domain == LAL_SIM_DOMAIN_TIME) {
        /* generate time domain waveform */
        if (p.verbose) {
            fprintf(stderr, "generating waveform in time domain using XLALSimInspiralChooseTDWaveform...\n");
            timer_start = clock();
        }
        XLALSimInspiralChooseTDWaveform(h_plus, h_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, 1.0 / p.srate, p.f_min, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);
    } else {
        COMPLEX16FrequencySeries *htilde_plus = NULL;
        COMPLEX16FrequencySeries *htilde_cross = NULL;
        REAL8FFTPlan *plan;
        double chirplen, deltaF;
        int chirplen_exp;

        /* determine required frequency resolution */
        /* length of the chirp in samples */
        chirplen = imr_time_bound(p.f_min, p.m1, p.m2, p.s1z, p.s2z) * p.srate;
        /* make chirplen next power of two */
        frexp(chirplen, &chirplen_exp);
        chirplen = ldexp(1.0, chirplen_exp);
        deltaF = p.srate / chirplen;
        if (p.verbose)
            fprintf(stderr, "using frequency resolution deltaF = %g Hz\n", deltaF);

        /* generate waveform in frequency domain */
        if (p.verbose) {
            fprintf(stderr, "generating waveform in frequency domain using XLALSimInspiralChooseFDWaveform...\n");
            timer_start = clock();
        }
        XLALSimInspiralChooseFDWaveform(&htilde_plus, &htilde_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, deltaF, p.f_min, 0.5 * p.srate, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);

        /* put the waveform in the time domain */
        if (p.verbose) {
            fprintf(stderr, "transforming waveform to time domain...\n");
            timer_start = clock();
        }
        *h_plus = XLALCreateREAL8TimeSeries("h_plus", &htilde_plus->epoch, 0.0, 1.0 / p.srate, &lalStrainUnit, (size_t) chirplen);
        *h_cross = XLALCreateREAL8TimeSeries("h_cross", &htilde_cross->epoch, 0.0, 1.0 / p.srate, &lalStrainUnit, (size_t) chirplen);
        plan = XLALCreateReverseREAL8FFTPlan((size_t) chirplen, 0);
        XLALREAL8FreqTimeFFT(*h_cross, htilde_cross, plan);
        XLALREAL8FreqTimeFFT(*h_plus, htilde_plus, plan);
        if (p.verbose)
            fprintf(stderr, "transformation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);

        /* clean up */
        XLALDestroyREAL8FFTPlan(plan);
        XLALDestroyCOMPLEX16FrequencySeries(htilde_cross);
        XLALDestroyCOMPLEX16FrequencySeries(htilde_plus);
    }

    return 0;
}

/* creates a waveform in the frequency domain; the waveform might be generated
 * in the time-domain and transformed */
int create_fd_waveform(COMPLEX16FrequencySeries ** htilde_plus, COMPLEX16FrequencySeries ** htilde_cross, struct params p)
{
    clock_t timer_start = 0;
    double chirplen, deltaF;
    int chirplen_exp;

    /* length of the chirp in samples */
    chirplen = imr_time_bound(p.f_min, p.m1, p.m2, p.s1z, p.s2z) * p.srate;
    /* make chirplen next power of two */
    frexp(chirplen, &chirplen_exp);
    chirplen = ldexp(1.0, chirplen_exp);
    deltaF = p.srate / chirplen;
    if (p.verbose)
        fprintf(stderr, "using frequency resolution deltaF = %g Hz\n", deltaF);

    if (p.condition) {
        if (p.verbose) {
            fprintf(stderr, "generating waveform in frequency domain using XLALSimInspiralFD...\n");
            timer_start = clock();
        }
        XLALSimInspiralFD(htilde_plus, htilde_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, deltaF, p.f_min, 0.5 * p.srate, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);
    } else if (p.domain == LAL_SIM_DOMAIN_FREQUENCY) {
        if (p.verbose) {
            fprintf(stderr, "generating waveform in frequency domain using XLALSimInspiralChooseFDWaveform...\n");
            timer_start = clock();
        }
        XLALSimInspiralChooseFDWaveform(htilde_plus, htilde_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, deltaF, p.f_min, 0.5 * p.srate, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);
    } else {
        REAL8TimeSeries *h_plus = NULL;
        REAL8TimeSeries *h_cross = NULL;
        REAL8FFTPlan *plan;

        /* generate time domain waveform */
        if (p.verbose) {
            fprintf(stderr, "generating waveform in time domain using XLALSimInspiralChooseTDWaveform...\n");
            timer_start = clock();
        }
        XLALSimInspiralChooseTDWaveform(&h_plus, &h_cross, p.m1, p.m2, p.s1x, p.s1y, p.s1z, p.s2x, p.s2y, p.s2z, p.distance, p.inclination, p.phiRef, p.longAscNodes, p.eccentricity, p.meanPerAno, 1.0 / p.srate, p.f_min, p.fRef, p.params, p.approx);
        if (p.verbose)
            fprintf(stderr, "generation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);

        /* resize the waveforms to the required length */
        XLALResizeREAL8TimeSeries(h_plus, h_plus->data->length - (size_t) chirplen, (size_t) chirplen);
        XLALResizeREAL8TimeSeries(h_cross, h_cross->data->length - (size_t) chirplen, (size_t) chirplen);

        /* put the waveform in the frequency domain */
        /* (the units will correct themselves) */
        if (p.verbose) {
            fprintf(stderr, "transforming waveform to frequency domain...\n");
            timer_start = clock();
        }
        *htilde_plus = XLALCreateCOMPLEX16FrequencySeries("htilde_plus", &h_plus->epoch, 0.0, deltaF, &lalDimensionlessUnit, (size_t) chirplen / 2 + 1);
        *htilde_cross = XLALCreateCOMPLEX16FrequencySeries("htilde_cross", &h_cross->epoch, 0.0, deltaF, &lalDimensionlessUnit, (size_t) chirplen / 2 + 1);
        plan = XLALCreateForwardREAL8FFTPlan((size_t) chirplen, 0);
        XLALREAL8TimeFreqFFT(*htilde_cross, h_cross, plan);
        XLALREAL8TimeFreqFFT(*htilde_plus, h_plus, plan);
        if (p.verbose)
            fprintf(stderr, "transformation took %g seconds\n", (double)(clock() - timer_start) / CLOCKS_PER_SEC);

        /* clean up */
        XLALDestroyREAL8FFTPlan(plan);
        XLALDestroyREAL8TimeSeries(h_cross);
        XLALDestroyREAL8TimeSeries(h_plus);
    }
    return 0;
}

/* routine to crudely overestimate the duration of the inspiral, merger, and ringdown */
double imr_time_bound(double f_min, double m1, double m2, double s1z, double s2z)
{
    double tchirp, tmerge;
    double s;

    /* lower bound on the chirp time starting at f_min */
    tchirp = XLALSimInspiralChirpTimeBound(f_min, m1, m2, s1z, s2z);

    /* upper bound on the final black hole spin */
    s = XLALSimInspiralFinalBlackHoleSpinBound(s1z, s2z);

    /* lower bound on the final plunge, merger, and ringdown time */
    tmerge = XLALSimInspiralMergeTimeBound(m1, m2) + XLALSimInspiralRingdownTimeBound(m1 + m2, s);

    return tchirp + tmerge;
}

/* returns the string corresponding to a particular frame axis enum value */
const char *frame_axis_to_string(LALSimInspiralFrameAxis axis)
{
    switch (axis) {
    case LAL_SIM_INSPIRAL_FRAME_AXIS_VIEW:
        return "View";
    case LAL_SIM_INSPIRAL_FRAME_AXIS_TOTAL_J:
        return "TotalJ";
    case LAL_SIM_INSPIRAL_FRAME_AXIS_ORBITAL_L:
        return "OrbitalL";
    default:
        fprintf(stderr, "error: unknown frame axis\n");
        exit(1);
    }
    return NULL;
}

/* returns the string corresponding to a particular mode choice enum value */
const char *modes_choice_to_string(LALSimInspiralModesChoice modes)
{
    switch (modes) {
    case LAL_SIM_INSPIRAL_MODES_CHOICE_RESTRICTED:
        return "L2";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_3L:
        return "L3";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_4L:
        return "L4";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND3L:
        return "L23";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND4L:
        return "L24";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_3AND4L:
        return "L34";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND3AND4L:
        return "L234";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_5L:
        return "L5";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND5L:
        return "L25";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_3AND5L:
        return "L35";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_4AND5L:
        return "L45";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND3AND5L:
        return "L235";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_2AND4AND5L:
        return "L245";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_3AND4AND5L:
        return "L345";
    case LAL_SIM_INSPIRAL_MODES_CHOICE_ALL:
        return "ALL";
    default:
        fprintf(stderr, "error: unknown modes choice\n");
        exit(1);
    }
    return NULL;
}

/* prints the chosen waveform parameters to stderr if verbosity is set */
int print_params(struct params p)
{
    if (p.verbose) {
        int phaseO = XLALSimInspiralWaveformParamsLookupPNPhaseOrder(p.params);
        int ampO = XLALSimInspiralWaveformParamsLookupPNAmplitudeOrder(p.params);
        int spinO = XLALSimInspiralWaveformParamsLookupPNSpinOrder(p.params);
        int tideO = XLALSimInspiralWaveformParamsLookupPNTidalOrder(p.params);
        int axis = XLALSimInspiralWaveformParamsLookupFrameAxis(p.params);
        int modes = XLALSimInspiralWaveformParamsLookupModesChoice(p.params);
        fprintf(stderr, "approximant:                                  %s\n", XLALSimInspiralGetStringFromApproximant(p.approx));
        if (phaseO == -1)
            fprintf(stderr, "phase post-Newtonian order:                   highest available\n");
        else
            fprintf(stderr, "twice phase post-Newtonian order:             %d (%g pN)\n", phaseO, 0.5 * phaseO);
        if (ampO == -1)
            fprintf(stderr, "amplitude post-Newtonian order:               highest available\n");
        else
            fprintf(stderr, "twice amplitude post-Newtonian order:         %d (%g pN)\n", ampO, 0.5 * ampO);
        if (spinO == -1)
            fprintf(stderr, "spin post-Newtonian order:                    highest available\n");
        else
            fprintf(stderr, "twice spin post-Newtonian order:              %d (%g pN)\n", spinO, 0.5 * spinO);
        if (tideO == -1)
            fprintf(stderr, "tidal post-Newtonian order:                   highest available\n");
        else
            fprintf(stderr, "twice tidal post-Newtonian order:             %d (%g pN)\n", tideO, 0.5 * tideO);
        fprintf(stderr, "reference phase:                              %g deg, %g rad\n", p.phiRef / LAL_PI_180, p.phiRef);
        fprintf(stderr, "sample rate:                                  %g Hz\n", p.srate);
        fprintf(stderr, "primary mass:                                 %g Msun\n", p.m1 / LAL_MSUN_SI);
        fprintf(stderr, "secondary mass:                               %g Msun\n", p.m2 / LAL_MSUN_SI);
        fprintf(stderr, "primary dimensionless spin vector:            (%g, %g, %g)\n", p.s1x, p.s1y, p.s1z);
        fprintf(stderr, "secondary dimensionless spin vector:          (%g, %g, %g)\n", p.s2x, p.s2y, p.s2z);
        fprintf(stderr, "starting frequency:                           %g Hz\n", p.f_min);
        fprintf(stderr, "reference frequency:                          %g Hz\n", p.fRef);
        fprintf(stderr, "distance:                                     %g Mpc\n", p.distance / (1e6 * LAL_PC_SI));
        fprintf(stderr, "inclination:                                  %g deg, %g rad\n", p.inclination / LAL_PI_180, p.inclination);
        fprintf(stderr, "frame axis:                                   %s\n", frame_axis_to_string(axis));
        fprintf(stderr, "higher mode l values:                         %s\n", modes_choice_to_string(modes));
        if (p.params) {
            LALDictEntry *param;
            LALDictIter iter;
            XLALDictIterInit(&iter, p.params);
            while ((param = XLALDictIterNext(&iter)) != NULL) {
                fprintf(stderr, "extra parameters:                             %s=", XLALDictEntryGetKey(param));
                XLALValuePrint(XLALDictEntryGetValue(param), fileno(stderr));
                fprintf(stderr, "\n");
            }
        }
    }
    return 0;
}

/* prints the usage message */
int usage(const char *program)
{
    int a, c;
    fprintf(stderr, "usage: %s [options]\n", program);
    fprintf(stderr, "options [default values in brackets]:\n");
    fprintf(stderr, "\t-h, --help               \tprint this message and exit\n");
    fprintf(stderr, "\t-v, --verbose            \tverbose output\n");
    fprintf(stderr, "\t-C, --radians            \tuse radians rather than decimal degrees\n");
    fprintf(stderr, "\t-F, --frequency-domain   \toutput data in frequency domain\n");
    fprintf(stderr, "\t-c, --condition-waveform \tapply waveform conditioning\n");
    fprintf(stderr, "\t-P, --amp-phase          \toutput data as amplitude and phase\n");
    fprintf(stderr, "\t-a APPROX, --approximant=APPROX \n\t\tapproximant [%s]\n", DEFAULT_APPROX);
    fprintf(stderr, "\t-w WAVEFORM, --waveform=WAVEFORM \n\t\twaveform string giving both approximant and order\n");
    fprintf(stderr, "\t-D domain, --domain=DOMAIN      \n\t\tdomain for waveform generation when both are available\n\t\t{\"time\", \"freq\"} [use natural domain for output]\n");
    fprintf(stderr, "\t-O PHASEO, --phase-order=PHASEO \n\t\ttwice pN order of phase (-1 == highest) [%d]\n", DEFAULT_PHASEO);
    fprintf(stderr, "\t-o AMPO, --amp-order=AMPO       \n\t\ttwice pN order of amplitude (-1 == highest) [%d]\n", DEFAULT_AMPO);
    fprintf(stderr, "\t-u PHIREF, --phiRef=PHIREF      \n\t\treference phase in degrees [%g]\n", DEFAULT_PHIREF);
    fprintf(stderr, "\t-U PERIANOM, --periastron-anomaly=PERIANOM\n\t\tmean periastron anomaly in degrees [%g]\n", DEFAULT_MEANPERANO);
    fprintf(stderr, "\t-W LONGASC, --longitude-ascending-node=LONGASC\n\t\tlongitude of ascending node in degrees [%g]\n", DEFAULT_LONGASCNODE);
    fprintf(stderr, "\t-e ECC, --eccentricity=ECC      \n\t\torbital eccentricity [%g]\n", DEFAULT_ECCENTRICITY);
    fprintf(stderr, "\t-R SRATE, --sample-rate=SRATE   \n\t\tsample rate in Hertz [%g]\n", DEFAULT_SRATE);
    fprintf(stderr, "\t-M M1, --m1=M1                  \n\t\tmass of primary in solar masses [%g]\n", DEFAULT_M1);
    fprintf(stderr, "\t-m M2, --m2=M2                  \n\t\tmass of secondary in solar masses [%g]\n", DEFAULT_M2);
    fprintf(stderr, "\t-d D, --distance=D              \n\t\tdistance in Mpc [%g]\n", DEFAULT_DISTANCE);
    fprintf(stderr, "\t-i IOTA, --inclination=IOTA     \n\t\tinclination in degrees [%g]\n", DEFAULT_INCLINATION);
    fprintf(stderr, "\t-X S1X, --spin1x=S1X            \n\t\tx-component of dimensionless spin of primary [%g]\n", DEFAULT_S1X);
    fprintf(stderr, "\t-Y S1Y, --spin1y=S1Y            \n\t\ty-component of dimensionless spin of primary [%g]\n", DEFAULT_S1Y);
    fprintf(stderr, "\t-Z S1Z, --spin1z=S1Z            \n\t\tz-component of dimensionless spin of primary [%g]\n", DEFAULT_S1Z);
    fprintf(stderr, "\t-x S2X, --spin2x=S2X            \n\t\tx-component of dimensionless spin of secondary [%g]\n", DEFAULT_S2X);
    fprintf(stderr, "\t-y S2Y, --spin2y=S2Y            \n\t\ty-component of dimensionless spin of secondary [%g]\n", DEFAULT_S2Y);
    fprintf(stderr, "\t-z S2Z, --spin2z=S2Z            \n\t\tz-component of dimensionless spin of secondary [%g]\n", DEFAULT_S2Z);
    fprintf(stderr, "\t-L LAM1, --tidal-lambda1=LAM1   \n\t\tdimensionless tidal deformability of primary [%g]\n", DEFAULT_LAMBDA1);
    fprintf(stderr, "\t-l LAM2, --tidal-lambda2=LAM2   \n\t\tdimensionless tidal deformability of secondary [%g]\n", DEFAULT_LAMBDA2);
    fprintf(stderr, "\t-q DQM1, --delta-quad-mon1=DQM1 \n\t\tdifference in quadrupole-monopole term of primary [%g]\n", DEFAULT_DQUADMON1);
    fprintf(stderr, "\t-Q DQM2, --delta-quad-mon2=DQM2 \n\t\tdifference in quadrupole-monopole term of secondary [%g]\n", DEFAULT_DQUADMON2);
    fprintf(stderr, "\t-s SPINO, --spin-order=SPINO    \n\t\ttwice pN order of spin effects (-1 == all) [%d]\n", LAL_SIM_INSPIRAL_SPIN_ORDER_DEFAULT);
    fprintf(stderr, "\t-t TIDEO, --tidal-order=TIDEO   \n\t\ttwice pN order of tidal effects (-1 == all) [%d]\n", LAL_SIM_INSPIRAL_TIDAL_ORDER_DEFAULT);
    fprintf(stderr, "\t-H ZERO, --Zero-Parameter=ZERO   \n\t\tlambdaG [%g]\n", DEFAULT_LAMBDAG);
    fprintf(stderr, "\t-f FMIN, --f-min=FMIN           \n\t\tfrequency to start waveform in Hertz [%g]\n", DEFAULT_F_MIN);
    fprintf(stderr, "\t-r FREF, --fRef=FREF            \n\t\treference frequency in Hertz [%g]\n", DEFAULT_FREF);
    fprintf(stderr, "\t-A AXIS, --axis=AXIS            \n\t\taxis for PhenSpin {View, TotalJ, OrbitalL} [%s]\n", frame_axis_to_string(LAL_SIM_INSPIRAL_FRAME_AXIS_DEFAULT));
    fprintf(stderr, "\t-n MODES, --modes=MODES         \n\t\tallowed l modes {L2, L23, ..., ALL} [%s]\n", modes_choice_to_string(LAL_SIM_INSPIRAL_MODES_CHOICE_DEFAULT));
    fprintf(stderr,
        "\t-p KEY1=VAL1,KEY2=VAL2,..., --params=KEY1=VAL1,KEY2=VAL2,...  \n\t\textra parameters as a key-value pair\n");
    fprintf(stderr, "recognized time-domain approximants:");
    for (a = 0, c = 0; a < NumApproximants; ++a) {
        if (XLALSimInspiralImplementedTDApproximants(a)) {
            const char *s = XLALSimInspiralGetStringFromApproximant(a);
            c += fprintf(stderr, "%s%s", c ? ", " : "\n\t", s);
            if (c > 50)
                c = 0;
        }
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "recognized frequency-domain approximants:");
    for (a = 0, c = 0; a < NumApproximants; ++a) {
        if (XLALSimInspiralImplementedFDApproximants(a)) {
            const char *s = XLALSimInspiralGetStringFromApproximant(a);
            c += fprintf(stderr, "%s%s", c ? ", " : "\n\t", s);
            if (c > 50)
                c = 0;
        }
    }
    fprintf(stderr, "\n");
    return 0;
}

/* sets params to default values and parses the command line arguments */
struct params parseargs(int argc, char **argv)
{
    int degrees = 1;
    int phaseO;
    char *inclination_string = NULL;
    char *phiRef_string = NULL;
    char *meanPerAno_string = NULL;
    char *longAscNodes_string = NULL;
    char *kv;
    struct params p = {
        .verbose = 0,
        .approx = XLALSimInspiralGetApproximantFromString(DEFAULT_APPROX),
        .condition = 0,
        .freq_dom = 0,
        .amp_phase = 0,
        .domain = DEFAULT_DOMAIN,
        .phiRef = DEFAULT_PHIREF * LAL_PI_180,
        .meanPerAno = DEFAULT_MEANPERANO * LAL_PI_180,
        .longAscNodes = DEFAULT_LONGASCNODE * LAL_PI_180,
        .eccentricity = DEFAULT_ECCENTRICITY,
        .fRef = DEFAULT_FREF,
        .srate = DEFAULT_SRATE,
        .m1 = DEFAULT_M1 * LAL_MSUN_SI,
        .m2 = DEFAULT_M2 * LAL_MSUN_SI,
        .f_min = DEFAULT_F_MIN,
        .distance = DEFAULT_DISTANCE * 1e6 * LAL_PC_SI,
        .inclination = DEFAULT_INCLINATION * LAL_PI_180,
        .s1x = DEFAULT_S1X,
        .s1y = DEFAULT_S1Y,
        .s1z = DEFAULT_S1Z,
        .s2x = DEFAULT_S2X,
        .s2y = DEFAULT_S2Y,
        .s2z = DEFAULT_S2Z,
        .lambdaG = DEFAULT_LAMBDAG,
        .params = NULL
    };
    struct LALoption long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"radians", no_argument, 0, 'C'},
        {"frequency-domain", no_argument, 0, 'F'},
        {"condition-waveform", no_argument, 0, 'c'},
        {"amp-phase", no_argument, 0, 'P'},
        {"approximant", required_argument, 0, 'a'},
        {"waveform", required_argument, 0, 'w'},
        {"domain", required_argument, 0, 'D'},
        {"phase-order", required_argument, 0, 'O'},
        {"amp-order", required_argument, 0, 'o'},
        {"phiRef", required_argument, 0, 'u'},
        {"periastron-anomaly", required_argument, 0, 'U'},
        {"longitude-ascending-node", required_argument, 0, 'W'},
        {"eccentricity", required_argument, 0, 'e'},
        {"fRef", required_argument, 0, 'r'},
        {"sample-rate", required_argument, 0, 'R'},
        {"m1", required_argument, 0, 'M'},
        {"m2", required_argument, 0, 'm'},
        {"spin1x", required_argument, 0, 'X'},
        {"spin1y", required_argument, 0, 'Y'},
        {"spin1z", required_argument, 0, 'Z'},
        {"spin2x", required_argument, 0, 'x'},
        {"spin2y", required_argument, 0, 'y'},
        {"spin2z", required_argument, 0, 'z'},
        {"tidal-lambda1", required_argument, 0, 'L'},
        {"tidal-lambda2", required_argument, 0, 'l'},
        {"delta-quad-mon1", required_argument, 0, 'q'},
        {"delta-quad-mon2", required_argument, 0, 'Q'},
        {"spin-order", required_argument, 0, 's'},
        {"tidal-order", required_argument, 0, 't'},
        {"Zero-Parameter", required_argument, 0, 'H'},
        {"f-min", required_argument, 0, 'f'},
        {"f-max", required_argument, 0, 'F'},
        {"distance", required_argument, 0, 'd'},
        {"inclination", required_argument, 0, 'i'},
        {"axis", required_argument, 0, 'A'},
        {"modes", required_argument, 0, 'n'},
        {"params", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };
    char args[] = "hvCFcPa:w:D:O:o:u:U:W:e:r:R:M:m:X:x:Y:y:Z:z:L:l:q:Q:s:t:H:f:d:i:A:n:p:";

    while (1) {
        int option_index = 0;
        int c;

        c = LALgetopt_long_only(argc, argv, args, long_options, &option_index);
        if (c == -1)    /* end of options */
            break;

        switch (c) {
        case 0:        /* if option set a flag, nothing else to do */
            if (long_options[option_index].flag)
                break;
            else {
                fprintf(stderr, "error parsing option %s with argument %s\n", long_options[option_index].name, LALoptarg);
                exit(1);
            }
        case 'h':      /* help */
            usage(argv[0]);
            exit(0);
        case 'v':      /* verbose */
            p.verbose = 1;
            break;
        case 'C':      /* radians */
            degrees = 0;
            break;
        case 'F':      /* frequency-domain */
            p.freq_dom = 1;
            break;
        case 'c':      /* condition-waveform */
            p.condition = 1;
            break;
        case 'P':      /* amp-phase */
            p.amp_phase = 1;
            break;
        case 'a':      /* approximant */
            p.approx = XLALSimInspiralGetApproximantFromString(LALoptarg);
            if ((int)p.approx == XLAL_FAILURE) {
                fprintf(stderr, "error: invalid value %s for %s\n", LALoptarg, long_options[option_index].name);
                exit(1);
            }
            break;
        case 'w':      /* waveform */
            p.approx = XLALSimInspiralGetApproximantFromString(LALoptarg);
            if ((int)p.approx == XLAL_FAILURE) {
                fprintf(stderr, "error: could not parse approximant from %s for %s\n", LALoptarg, long_options[option_index].name);
                exit(1);
            }
            phaseO = XLALSimInspiralGetPNOrderFromString(LALoptarg);
            if ((int)phaseO == XLAL_FAILURE) {
                fprintf(stderr, "error: could not parse order from %s for %s\n", LALoptarg, long_options[option_index].name);
                exit(1);
            }
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPNPhaseOrder(p.params, phaseO);
            break;
        case 'D':      /* domain */
            switch (*LALoptarg) {
            case 'T':
            case 't':
                p.domain = LAL_SIM_DOMAIN_TIME;
                break;
            case 'F':
            case 'f':
                p.domain = LAL_SIM_DOMAIN_FREQUENCY;
                break;
            default:
                fprintf(stderr, "error: invalid value %s for %s\n", LALoptarg, long_options[option_index].name);
                exit(1);
            }
	    break;
        case 'O':      /* phase-order */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPNPhaseOrder(p.params, atoi(LALoptarg));
            break;
        case 'o':      /* amp-order */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPNAmplitudeOrder(p.params, atoi(LALoptarg));
            break;
        case 'u':      /* phiRef */
            phiRef_string = LALoptarg;
            break;
        case 'U':      /* mean-periastron-anomaly */
            meanPerAno_string = LALoptarg;
            break;
        case 'W':      /* longitude-ascending-node */
            longAscNodes_string = LALoptarg;
            break;
        case 'e':      /* eccentricity */
            p.eccentricity = atof(LALoptarg);
            break;
        case 'r':      /* fRef */
            p.fRef = atof(LALoptarg);
            break;
        case 'R':      /* sample-rate */
            p.srate = atof(LALoptarg);
            break;
        case 'M':      /* m1 */
            p.m1 = atof(LALoptarg) * LAL_MSUN_SI;
            break;
        case 'm':      /* m2 */
            p.m2 = atof(LALoptarg) * LAL_MSUN_SI;
            break;
        case 'X':      /* spin1x */
            p.s1x = atof(LALoptarg);
            break;
        case 'Y':      /* spin1y */
            p.s1y = atof(LALoptarg);
            break;
        case 'Z':      /* spin1z */
            p.s1z = atof(LALoptarg);
            break;
        case 'x':      /* spin2x */
            p.s2x = atof(LALoptarg);
            break;
        case 'y':      /* spin2y */
            p.s2y = atof(LALoptarg);
            break;
        case 'z':      /* spin2z */
            p.s2z = atof(LALoptarg);
            break;
        case 'L':      /* tidal-lambda1 */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertTidalLambda1(p.params, atoi(LALoptarg));
            break;
        case 'l':      /* tidal-lambda2 */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertTidalLambda2(p.params, atoi(LALoptarg));
            break;
        case 'q':      /* diff-quad-mon1 */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertdQuadMon1(p.params, atoi(LALoptarg));
            break;
        case 'Q':      /* diff-quad-mon2 */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertdQuadMon2(p.params, atoi(LALoptarg));
            break;
        case 's':      /* spin-order */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPNSpinOrder(p.params, atoi(LALoptarg));
            break;
        case 't':      /* tidal-order */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPNTidalOrder(p.params, atoi(LALoptarg));
            break;
        case 'H':      /* lambdaG */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertPhenomZPHMLambdaG(p.params, atoi(LALoptarg));
            break;
        case 'f':      /* f-min */
            p.f_min = atof(LALoptarg);
            break;
        case 'd':      /* distance */
            p.distance = atof(LALoptarg) * 1e6 * LAL_PC_SI;
            break;
        case 'i':      /* inclination */
            inclination_string = LALoptarg;
            break;
        case 'A':      /* axis */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertFrameAxis(p.params, XLALSimInspiralGetFrameAxisFromString(LALoptarg));
            break;
        case 'n':      /* modes */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            XLALSimInspiralWaveformParamsInsertModesChoice(p.params, XLALSimInspiralGetHigherModesFromString(LALoptarg));
            break;
        case 'p':      /* params */
            if (p.params == NULL)
                p.params = XLALCreateDict();
            while ((kv = XLALStringToken(&LALoptarg, ",", 0))) {
                char *key = XLALStringToken(&kv, "=", 0);
                if (kv == NULL || key == NULL || *key == '\0') {
                    fprintf(stderr, "error: invalid key-value pair for %s\n", long_options[option_index].name);
                    exit(1);
                }
                XLALDictInsertREAL8Value(p.params, key, atof(kv));
            }
            break;
        case '?':
        default:
            fprintf(stderr, "unknown error while parsing options\n");
            exit(1);
        }
    }
    if (LALoptind < argc) {
        fprintf(stderr, "extraneous command line arguments:\n");
        while (LALoptind < argc)
            fprintf(stderr, "%s\n", argv[LALoptind++]);
        exit(1);
    }

    /* set angles, converting to degrees to radians if needed */
    if (phiRef_string) {
        p.phiRef = atof(phiRef_string);
        if (degrees)
            p.phiRef *= LAL_PI_180;
    }
    if (inclination_string) {
        p.inclination = atof(inclination_string);
        if (degrees)
            p.inclination *= LAL_PI_180;
    }
    if (meanPerAno_string) {
        p.meanPerAno = atof(meanPerAno_string);
        if (degrees)
            p.meanPerAno *= LAL_PI_180;
    }
    if (longAscNodes_string) {
        p.longAscNodes = atof(longAscNodes_string);
        if (degrees)
            p.longAscNodes *= LAL_PI_180;
    }

    return p;
}
