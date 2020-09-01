/*
 * Purpose: allocate and free memory for the atmos data struct
 * Usage  : Part of VIC
 * Author : Bart Nijssen
 * E-mail : nijssen@u.washington.edu
 * Created: Fri Aug 27 18:22:42 1999
 * Last Changed: Tue Sep  2 15:11:02 2003 by Keith Cherkauer <cherkaue@u.washington.edu>
 * Notes  :
 */

/****************************************************************************/
/*			  PREPROCESSOR DIRECTIVES                           */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

static char vcid[] = "$Id: alloc_atmos.c,v 5.5.2.5 2011/12/23 06:57:55 vicadmin Exp $";

/****************************************************************************/
/*			       alloc_atmos()                                */
/****************************************************************************/
void alloc_atmos(int nrecs, atmos_data_struct **atmos)
/*******************************************************************
  alloc_atmos

  Modifications:
  01-11-00 Fixed allocation bug                             KAC
  2006-Sep-23 Implemented flexible output configuration; removed
	      LDAS_OUTPUT and OPTIMIZE compile-time options.		TJB
  2006-Dec-20 All atmos_data arrays are always dynamically allocated
	      now.							TJB
  2010-Mar-31 Added runoff_in.						TJB
  2010-Sep-24 Renamed runoff_in to channel_in.				TJB
  2011-Nov-04 Added tskc.						TJB
  2013-Feb-06 CropSyst added                     KM
*******************************************************************/
{
  extern param_set_struct param_set;

  int i;

  *atmos = (atmos_data_struct *) calloc(nrecs, sizeof(atmos_data_struct));
  if (*atmos == NULL)
    vicerror("Memory allocation error in alloc_atmos().");

  for (i = 0; i < nrecs; i++) {
    (*atmos)[i].air_temp = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].air_temp == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].channel_in = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].channel_in == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].density = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].density == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].longwave = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].longwave == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].prec = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].prec == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].pressure = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].pressure == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].shortwave = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].shortwave == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].snowflag = (char *) calloc(NR+1, sizeof(char));
    if ((*atmos)[i].snowflag == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].tskc = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].tskc == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].vp = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].vp == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].vpd = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].vpd == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
    (*atmos)[i].wind = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].wind == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
      /*********added because of the coupling Keyvan Malek 01162013**/
#ifdef VIC_CROPSYST_VERSION
/*130219
     (*atmos)[i].tmax = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].tmax == NULL)
       vicerror("Memory allocation error in alloc_atmos().");
     (*atmos)[i].tmin = (double *) calloc(NR+1, sizeof(double));
     if ((*atmos)[i].tmin == NULL)
       vicerror("Memory allocation error in alloc_atmos().");
*/
    (*atmos)[i].VCS.relative_humidity = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].VCS.relative_humidity == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
#ifdef  __BCPLUSPLUS__
//190130RLN compiler doesn't see sph
#else
    (*atmos)[i].VCS.sph = (double *) calloc(NR+1, sizeof(double));
    if ((*atmos)[i].VCS.sph == NULL)
      vicerror("Memory allocation error in alloc_atmos().");
#endif
#endif
  } //question? should we allocate a memory for the min and max relative humidity or they should take the memory from stack? keyvan

}

/****************************************************************************/
/*	      		  free_atmos()                                      */
/****************************************************************************/
void free_atmos(const int nrecs, atmos_data_struct *atmos)
/***************************************************************************
  Modifications:
  09-02-2003 Added check for LINK_DEBUG global option.  If LINK_DEBUG is
             TRUE atmospheric data is not dynamically allocated, so it
             should not be freed.                                   KAC
  2006-Sep-23 (Port from 4.0.6) Implemented flexible output configuration;
	      removed LDAS_OUTPUT and OPTIMIZE compile-time options.	TJB
  2006-Dec-20 All atmos_data arrays are always dynamically allocated
	      now.							TJB
  2010-Mar-31 Added runoff_in.						TJB
  2010-Sep-24 Renamed runoff_in to channel_in.				TJB
  2011-Nov-04 Added tskc.						TJB
  2012-Nov-13 cropsyst related changes added              KM
***************************************************************************/
{
  if (atmos == NULL) return;
  for (int i = 0; i < nrecs; i++) {
    free(atmos[i].air_temp);
    free(atmos[i].channel_in);
    free(atmos[i].density);
    free(atmos[i].longwave);
    free(atmos[i].prec);
    free(atmos[i].pressure);
    free(atmos[i].shortwave);
    free(atmos[i].snowflag);
    free(atmos[i].tskc);
    free(atmos[i].vp);
    free(atmos[i].vpd);
    free(atmos[i].wind);
#ifdef VIC_CROPSYST_VERSION
    free(atmos[i].VCS.relative_humidity);                                        //keyvan added 130214
#ifdef __BCPLUSPLUS__
//190130RLN compiler doesn't see sph
#else
    free(atmos[i].VCS.sph);
#endif
#endif
  }
  free(atmos);
}
