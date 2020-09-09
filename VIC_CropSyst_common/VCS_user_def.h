#ifndef VCS_USER_DEF_H
#define VCS_USER_DEF_H

#define POTATO_SOW_DAY_IN_YEAR 76
                                                                                 //** This is hard coded for the intial model.  08152010 KJC**
#define POTATO_HARVEST_DAY_IN_YEAR 300
                                                                                 //** This is hard coded for the intial model.  USE when not harvested automatiaclly08152010 KJC**
#define WHEAT_SPRING_SOW_DAY_IN_YEAR 80
#define IRRIGATION_EFFICIENCY 0.6
                                                                                 //**hard coded foe inital runs and is uniform throughout 08172010 KJC**
#define CROP_VEG_CLASS 11
                                                                                 //**For initial development only.  This has to be changed later.
                                                                                 //For now a vegetation class 11 is considered a crop, same as veg param file. *
#define MAX_APPLIED_IRRIGATION_AMOUNT 20.0
                                                                                 //**This is the maximum irrigation amount that can be applied. *
#define MIN_APPLIED_IRRIGATION_AMOUNT 20.0
#define FULL_IRRIGATION TRUE
#define PORTION_OF_IRRIGATION_DEMAND_IN_DEFICIT 0.3

//190823LML for increasing irrigation frequency
//190827 LML used 0.1
//190828 LML used 0.0
//190828 LML put to Makefile macro defination
//#define MAD_FOR_NETIRRIGATION_DEMAND 0.0

//180511LML #define MECHANISTIC_RUNOFF TRUE

#ifdef MECHANISTIC_RUNOFF_TRUE
#define MECHANISTIC_RUNOFF TRUE
#else
#define MECHANISTIC_RUNOFF FALSE
#endif
#define UPDATE_T_VPD FALSE
#endif
