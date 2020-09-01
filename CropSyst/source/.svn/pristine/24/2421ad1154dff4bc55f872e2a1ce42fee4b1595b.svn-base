#ifndef VERSIONH
#define VERSIONH

#ifndef CS_VERSION
//#error CS_VERSION is not defined, this must  now be set with the compiler command line option (Usually -D CS_VERSION=4)
#define CS_VERSION 4
#endif
#if (CS_VERSION == 4)
#     define CS_VERSION_NUMBERS 4,20,7
// The version numbers now represent the  Major version , year and month
#  ifndef CS_VERSION_CODE
// Version code is now being set with the compiler command line option (Usually -D)
// DONT FORGET to also modify the makefile or IDE options to setup the CS_VERSION_CODE define!
#     define CS_VERSION_CODE    0x041407
#  endif
#endif

#if (CS_VERSION == 5)
#  ifndef CS_VERSION_NUMBERS
#     define CS_VERSION_NUMBERS 5,20,7
#  endif
//180625 #     define CS_VERSION_STRING "5.00.00"
#  ifndef CS_VERSION_CODE
// Version code is now being set with the compiler command line option (Usually -D)
// DONT FORGET to also modify the makefile or IDE options to setup the CS_VERSION_CODE define!
// CS_VERSION_CODE = 0x040C12
#     define CS_VERSION_CODE    0x051320
#  endif
#endif

#endif
