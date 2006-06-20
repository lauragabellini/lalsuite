/*  
 *  Copyright (C) 2005 Badri Krishnan, Alicia Sintes
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
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 * 
 */



/**
 * \file DriveHoughFStat.h
 * \brief Header file for DriveHoughFStat.c 
 * \author Badri Krishnan, Alicia Sintes
 * \date $Date$
 * 
 ****/


#ifndef _DRIVEHOUGHCOLOR_H
#define _DRIVEHOUGHCOLOR_H

/* standard includes */
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glob.h>
#include <time.h>
#include <errno.h> 

/* lal includes */
#include <lal/UserInput.h>
#include <lal/LALStdlib.h>
#include <lal/PulsarDataTypes.h>
#include <lal/SFTfileIO.h>
#include <lal/AVFactories.h>
#include <lal/RngMedBias.h>
#include <lal/LALComputeAM.h>
#include <lal/ComputeSky.h>
#include <lal/LALInitBarycenter.h>
#include <lal/Velocity.h>
#include <lal/LALDemod.h>
#include <lal/ExtrapolatePulsarSpins.h>
#include <lal/Date.h>
#include <lal/LALHough.h> 
#include <lal/NormalizeSFTRngMed.h>
#include <lal/ComputeFstat.h>
#include <lal/Statistics.h>
#include <lal/GeneratePulsarSignal.h> 

/* lalapps includes */
#include <lalapps.h>
#include "../../FDS_isolated/DopplerScan.h"
#include "../../FDS_isolated/FstatToplist.h"

/******************************************************
 *   Protection against C++ name mangling
 */

#ifdef  __cplusplus
extern "C" {
#endif


/******************************************************
 *  Assignment of Id string using NRCSID()
 */

NRCSID( HIERARCHICALSEARCHH, "$Id$" );

/******************************************************
 *  Error codes and messages.
 */
 
#define HIERARCHICALSEARCH_ENORM 0
#define HIERARCHICALSEARCH_ESUB  1
#define HIERARCHICALSEARCH_EARG  2
#define HIERARCHICALSEARCH_EBAD  3
#define HIERARCHICALSEARCH_EFILE 4
#define HIERARCHICALSEARCH_ENULL 5
#define HIERARCHICALSEARCH_EVAL  6

#define HIERARCHICALSEARCH_MSGENORM "Normal exit"
#define HIERARCHICALSEARCH_MSGESUB  "Subroutine failed"
#define HIERARCHICALSEARCH_MSGEARG  "Error parsing arguments"
#define HIERARCHICALSEARCH_MSGEBAD  "Bad argument values"
#define HIERARCHICALSEARCH_MSGEFILE "Could not create output file"
#define HIERARCHICALSEARCH_MSGENULL "Null pointer"
#define HIERARCHICALSEARCH_MSGEVAL "Invalid value"



/* ******************************************************************
 *  Structure, enum, union, etc., typdefs.
 */


  /** sequence of MultiSFT vectors -- for each stack */
  typedef struct tagMultiSFTVectorSequence {
    UINT4 length;     /**< number of stacks */
    MultiSFTVector **data; /**< the SFT vectors */
  } MultiSFTVectorSequence;

  /** sequence of Multi-noise weights vectors -- for each stack */
  typedef struct tagMultiNoiseWeightsSequence {
    UINT4 length;     /**< number of stacks */
    MultiNoiseWeights **data; /**< the noise weights */
  } MultiNoiseWeightsSequence;

  /** sequence of Multi-detector state vectors -- for each stack */
  typedef struct tagMultiDetectorStateSeriesSequence {
    UINT4 length;     /**< number of stacks */
    MultiDetectorStateSeries **data; /**< the detector state series */
  } MultiDetectorStateSeriesSequence;

  /* sequence of SFT catalogs -- for each stack */
  typedef struct tagSFTCatalogSequence {
    UINT4 length;     /**< the number of stacks */
    SFTCatalog *data; /**< the catalogs */
  } SFTCatalogSequence;


  /** parameters for the semicoherent stage -- hough or stackslide */
  typedef struct tagSemiCoherentParams {
    LIGOTimeGPSVector *tsMid;  /**< timestamps of mid points of stacks */
    REAL8 refTime;             /**< reference time for f, fdot definition */
    REAL8VectorSequence *vel;  /**< detector velocity for each stack */
    REAL8VectorSequence *pos;  /**< detector position for each stack */
    REAL8 alpha;               /**< right ascension of demodulation point */
    REAL8 delta;               /**< declination of demodulation point*/
    REAL8 fdot;                /**< spindown value of demodulation point */
    UINT4 nfdot;               /**< number of fdot values to search over */ 
    CHAR *outBaseName;         /**< file for writing output -- if chosen */
  } SemiCoherentParams;

  /** one hough candidate */
  typedef struct tagSemiCohCandidate {
    REAL8 freq;        /**< frequency */
    REAL8 alpha;       /**< right ascension */
    REAL8 delta;       /**< declination */
    REAL8 fdot;        /**< spindown */
    REAL8 dFreq;       /**< frequency error */
    REAL8 dAlpha;      /**< alpha error */
    REAL8 dDelta ;     /**< delta error */
    REAL8 dFdot;       /**< fdot error */
    REAL8 significance;/**< significance */
  } SemiCohCandidate;  

  /** structure for storing candidates produced by Hough search */
  typedef struct tagSemiCohCandidateList {
    INT4 length;               /**< maximum allowed length of vectors */
    INT4 nCandidates;          /**< number of candidates -- must be less than length */
    INT4 minSigIndex;          /**< index of least significant candidate */ 
    SemiCohCandidate *list;    /**> list of candidates */
  } SemiCohCandidateList;



  /* function prototypes */

  void PrintFstatVec_fp (LALStatus *status,
			 REAL8FrequencySeries *in,
			 FILE *fp,
			 REAL8 alpha,
			 REAL8 delta,
			 REAL8 fdot);

  void ComputeFstatHoughMap (LALStatus *status,
			     SemiCohCandidateList *out,
			     HOUGHPeakGramVector *pgV,
			     SemiCoherentParams *params);

  void FstatVectToPeakGram (LALStatus *status,
			    HOUGHPeakGramVector *pgV,
			    REAL8FrequencySeriesVector *FstatVect,
			    REAL8  thr);

  void SetUpStacks(LALStatus *status, 
		 SFTCatalogSequence  *out,  
		 REAL8 *tStack,
		 LIGOTimeGPSVector **ts,
		 SFTCatalog  *in,
		 UINT4 nStacks);

  void PrintHmap2file(LALStatus *status,
		      HOUGHMapTotal *ht, 
		      CHAR *fnameOut, 
		      INT4 iHmap);

  void GetHoughCandidates(LALStatus *status,
			  SemiCohCandidateList *semiCohCand,
			  HOUGHMapTotal *ht,
			  HOUGHPatchGrid  *patch,
			  HOUGHDemodPar   *parDem,
			  REAL8 houghThreshold);

  void GetHoughCandidates_toplist(LALStatus *status,
				  SemiCohCandidateList *semiCohCand,
				  HOUGHMapTotal *ht,
				  HOUGHPatchGrid  *patch,
				  HOUGHDemodPar   *parDem);

  void GetFstatCandidates( LALStatus *status,
			   toplist_t *list,
			   REAL8FrequencySeries *in,
			   REAL8 FstatThr,
			   REAL8 alpha,
			   REAL8 delta,
			   REAL8 fdot,
			   INT4 blockRealloc);

  void GetFstatCandidates_toplist(LALStatus *status,
				  toplist_t *list,
				  REAL8FrequencySeries   *FstatVec,
				  REAL8 alpha,
				  REAL8 delta,
				  REAL8 fdot);

  void GetMinSigIndex_toplist(LALStatus *status,
			      INT4 *minSigIndex,
			      SemiCohCandidateList *semiCohCand);

  void PrintSemiCohCandidates(LALStatus *status,
			    SemiCohCandidateList *in,
			    FILE *fp);

  void PrintHoughHistogram(LALStatus *status,
			   UINT8Vector *hist, 
			   CHAR *fnameOut);

  void GetChkPointIndex( LALStatus *status,
			 INT4 *loopindex, 
			 CHAR *fnameChkPoint);


  
#ifdef  __cplusplus
}                /* Close C++ protection */
#endif


#endif     /* Close double-include protection _HIERARCHICALSEARCH_H */



