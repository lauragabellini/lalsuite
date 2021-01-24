/*
*  Copyright (C) 2007 Badri Krishnan, Alicia Sintes Olives
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
*/

/**
 * \file
 * \ingroup lalapps_pulsar_Hough
 * \author Badri Krishnan
 */

#include "config.h"

#include <lal/SFTClean.h>
#include <lal/SFTutils.h>
#include <glob.h> 
#include <LALAppsVCSInfo.h>

/* Error codes and messages */

/**\name Error Codes */ /** @{ */
#define SFTCLEANC_ENORM 0
#define SFTCLEANC_ESUB  1
#define SFTCLEANC_EARG  2
#define SFTCLEANC_EBAD  3
#define SFTCLEANC_EFILE 4

#define SFTCLEANC_MSGENORM "Normal exit"
#define SFTCLEANC_MSGESUB  "Subroutine failed"
#define SFTCLEANC_MSGEARG  "Error parsing arguments"
#define SFTCLEANC_MSGEBAD  "Bad argument values"
#define SFTCLEANC_MSGEFILE "Could not create output file"
/** @} */

/*---------- local defines ---------- */
#define MAXFILENAMELENGTH 256
#define TRUE (1==1)
#define FALSE (1==0)

#define CMT_NONE 0
#define CMT_OLD  1
#define CMT_FULL 2

int main(int argc, char *argv[]){ 

  static LALStatus       status;  /* LALStatus pointer */ 

  static MultiSFTVector  *inputSFTs = NULL;
  static SFTCatalog *catalog = NULL;
  static SFTCatalog thisCatalog;
  static SFTConstraints constraints;


  /* 09/09/05 gam; randPar now a parameter for LALCleanCOMPLEX8SFT */
  FILE *fp=NULL;   
  INT4 seed, ranCount;
  RandomParams *randPar=NULL; 
  UINT4 k, j;  

  /* user input variables */
  LALStringVector *uvar_linefiles=NULL; /* files with harmonics info */
  CHAR *uvar_sftDir;    /* directory for unclean sfts */
  CHAR *uvar_outDir;   /* directory for cleaned sfts */
  REAL8 uvar_fMin, uvar_fMax;
  INT4  uvar_window, uvar_maxBins, uvar_addComment;

  /* set defaults */
  uvar_sftDir = NULL;
  uvar_outDir = NULL;
  uvar_fMin = -1;
  uvar_fMax = -1;
  uvar_window = 100;
  uvar_maxBins = 20;
  uvar_addComment = CMT_FULL; /* add VCS ID and full command-line to every SFT file */

  /* register user input variables */
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_sftDir,    "sftDir",    STRING,       'i', REQUIRED,  "Input SFT file pattern") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_outDir,    "outDir",    STRING,       'o', REQUIRED,  "Output SFT Directory") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_fMin,      "fMin",      REAL8,        0,   OPTIONAL, "start Frequency (default: full input SFTs width)") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_fMax,      "fMax",      REAL8,        0,   OPTIONAL, "Max Frequency  (default: full input SFTs width)") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_window,    "window",    INT4,         'w',OPTIONAL,  "Window size for noise floor estimation in vicinity of a line") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_maxBins,   "maxBins",   INT4,         'm',OPTIONAL,  "Max. bins to clean") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_linefiles, "linefiles", STRINGVector, 0,   OPTIONAL, "List of per-detector files with list of lines (each full path must start with a canonical IFO name)") == XLAL_SUCCESS, XLAL_EFUNC);
  XLAL_CHECK_MAIN( XLALRegisterNamedUvar( &uvar_addComment, "addComment", INT4,       'c', OPTIONAL, "How to deal with comments - 0 means no comment is written at all, 1 means that the comment is taken unmodified from the input SFTs, 2 (default) means that the program appends its RCS id and command-line to the comment.") == XLAL_SUCCESS, XLAL_EFUNC);


  /* read all command line variables */
  BOOLEAN should_exit = 0;
  XLAL_CHECK_MAIN( XLALUserVarReadAllInput(&should_exit, argc, argv, lalAppsVCSInfoList) == XLAL_SUCCESS, XLAL_EFUNC);
  if (should_exit)
    exit(1);

  /* record VCS ID and command-line for the comment */
  char *cmdline = NULL;
  if ( uvar_addComment > CMT_OLD ) {
    XLAL_CHECK_MAIN( ( cmdline = ( char * )XLALMalloc( strlen( lalAppsVCSIdentInfo.vcsId ) + strlen( lalAppsVCSIdentInfo.vcsStatus ) + 2 ) ) != NULL, XLAL_ENOMEM, "out of memory allocating cmdline" );
    strcpy( cmdline, lalAppsVCSIdentInfo.vcsId );
    strcat( cmdline, lalAppsVCSIdentInfo.vcsStatus );
    strcat( cmdline, "\n" );
    for ( int arg = 0; arg < argc; arg++ ) {
        XLAL_CHECK_MAIN( ( cmdline = ( char * )XLALRealloc( ( void * )cmdline, strlen( cmdline ) + strlen( argv[arg] ) + 2 ) ) != NULL, XLAL_ENOMEM, "out of memory allocating cmdline" );
        strcat( cmdline, argv[arg] );
        if ( arg == argc - 1 ) {
        strcat( cmdline, "\n" );
        } else {
        strcat( cmdline, " " );
        }
    }
  }

  /* set detector constraint */
  constraints.detector = NULL;

  /* get sft catalog */
  XLAL_CHECK_MAIN( ( catalog = XLALSFTdataFind( uvar_sftDir, &constraints) ) != NULL, XLAL_EFUNC);
  if ( (catalog == NULL) || (catalog->length == 0) ) {
    fprintf (stderr,"Unable to match any SFTs with pattern '%s'\n", uvar_sftDir );
    exit(1);
  }
  fprintf(stdout, "Created catalog with %d SFTs.\n", catalog->length);

  /* get a new seed value, and use it to create a new random parameter structure */
  fp=fopen("/dev/urandom", "r");
  if (!fp) 
    { 
      fprintf(stderr,"Error in opening /dev/urandom \n"); 
      exit(1); 
    } 
  ranCount = fread(&seed, sizeof(seed), 1, fp);
  fclose(fp);
  if (!(ranCount==1)) 
    { 
      fprintf(stderr,"Error in reading random seed \n"); 
      exit(1); 
    } 
  LAL_CALL ( LALCreateRandomParams (&status, &randPar, seed), &status );
  
  /* loop over sfts and clean them -- load one sft at a time */
  thisCatalog.length = 1;
  for (j=0; j<catalog->length; j++) {

    thisCatalog.data = catalog->data + j;
  
    /* read the sfts */
    XLAL_CHECK_MAIN( ( inputSFTs = XLALLoadMultiSFTs ( &thisCatalog, uvar_fMin, uvar_fMax) ) != NULL, XLAL_EFUNC);    

    /* construct comment for output SFTs */
    char *comment = NULL;
    char *oldcomment = thisCatalog.data[0].comment;
    int comment_length = strlen(oldcomment) + 1;
    if ( uvar_addComment > CMT_OLD ) {
      /* allocate space for new comment */
      XLAL_CHECK_MAIN( ( comment = ( char * )XLALMalloc( comment_length + strlen( cmdline ) + 1 ) ) != NULL, XLAL_ENOMEM, "out of memory allocating comment" );
      /* append the commandline of this program to the old comment */
      if ( oldcomment ) {
        strcpy( comment, oldcomment );
      } else {
        *comment = '\0';
      }
      strcat( comment, cmdline );
    } else if ( uvar_addComment == CMT_OLD ) {
      /* only copied existing comment, no additional space needed */
      comment = oldcomment;
    } /* else (uvar_addComment == CMT_NONE) and (comment == NULL) i.e. no comment at all */

    /* clean  lines */
    if ( XLALUserVarWasSet( &uvar_linefiles ) ) {
      LAL_CALL( LALRemoveKnownLinesInMultiSFTVector ( &status, inputSFTs, uvar_maxBins, 
						      uvar_window, uvar_linefiles, randPar), &status);
    }
    
    /* write output */
    for (k = 0; k < inputSFTs->length; k++) {
      XLAL_CHECK_MAIN( XLALWriteSFTVector2Dir (inputSFTs->data[k], uvar_outDir, comment, "cleaned") == XLAL_SUCCESS, XLAL_EFUNC);
    }

    XLALDestroyMultiSFTVector( inputSFTs);
    if ( uvar_addComment > CMT_OLD ) {
      XLALFree( comment );
    }

  } /* end loop over sfts */

  /* Free memory */
  XLALDestroySFTCatalog(catalog );
  XLALDestroyUserVars();
  LAL_CALL( LALDestroyRandomParams (&status, &randPar), &status);
  if ( uvar_addComment > CMT_OLD ) {
    XLALFree( cmdline );
  }

  LALCheckMemoryLeaks(); 

  return 0;
}
