#ifndef primitiveH
#define primitiveH

// Header file primitive.h
// Primitive types
// For C and/or C++
// This header file is intended to define a set of primitive types
// that will always be the same number bytes on any operating operating systems
// and/or for several popular C/C++ compiler vendors.
// Currently the type definitions cover:
// Windows (16 or 32 bit)
// Linux
// UNIX (HP/US, Solaris)
// And the following compiler vendors
// Microsoft, Borland/Imprise/CodeGear, SunStudio,  HP/UX
// (maybe GNU C/C++)
// This does not currently include 64bit primitives.


//______________________________________________________________________________
#if (defined(__MINGW32__) )
// MING ?maybe GCC? doesn't seem to consistently define float as 32bit
#define _QT_
// There doesn't appear to be a predefined macro identifying the Qt compiler
// currently assuming that if compiling with MinGW the compiler is Qt.
#endif
//______________________________________________________________________________

#if defined(_MSC_VER) && defined(_WIN32)
#include <stdint.h>                                                             //LML 150220 uint32_t
#endif

#include <stdlib.h>

//MS VS needs stdlib.h for wchar_t

#define float64 double

#define float32 float
#define float_P float
   // I was using 32bit for parameters loaded from text parameter files.

// Some old C++ compilers didn't have bool type
// If your compiler does not have bool then add   emulate_bool
// to your command line -D option or defined macros.

#if (!defined(__cplusplus) && !defined(__BOOL_DEFINED) && !defined(__bool_true_false_are_defined))
#   ifdef TVISION
#     define bool int
#     define true 1
#     define false 0
#   else
#     ifdef __BCPLUSPLUS__
      //BC++ bool type not available until 5.0
#        define BI_NO_BOOL
#        include <classlib/defs.h>
#     else
//       #include <stdbool.h>
#        define bool int
#        define true 1
#        define false 0
#     endif
#  endif
#endif


#ifdef __BORLANDC__
#  include <systypes.h>
   #if (__BORLANDC__ <= 0x0560)
      // wchar_t did not exist in BCB until after version 5
      #include <basetyps.h>
   #endif
#else
#  ifdef __GNUC__
#     include <stdint.h>
#     define int8 int8_t
#     define int16 int16_t
#     define int32 int32_t
#     define uint8 uint8_t
#     define uint16 uint16_t
#     define uint32 uint32_t
      // needed to compile netCDF 180828
      #define uchar  unsigned char
      #define ushort unsigned short
      #define uint   unsigned int
      #define longlong  long long
      #define ulonglong unsigned long long
#  else
#  if (defined(unix) || defined(linux))
#     ifdef hpux
#        include <sys/_inttypes.h>
#     endif
#     ifdef sun
         // Solaris appears to use sys/int_types.h instead of stdint.h>
#        include <sys/int_types.h>
#     endif
#     define int8 int8_t
#     define int16 int16_t
#     define int32 int32_t
#     define uint8 uint8_t
#     define uint16 uint16_t
#     define uint32 uint32_t
#  else
#     ifdef  _MSC_VER
#        ifndef _BASETSD_H_
#        include <BaseTSD.h>
#        endif
#        define int8 INT8
#        define uint8 UINT8
#        define int16 INT16
#        define int32 INT32
#        define uint16 UINT16
#     ifdef _WIN64
#        define uint32 uint32_t
#     else
#        define uint32 UINT32
#     endif
#     else
#        ifndef OWL6
//          OWL version 6 already defines these types
#           define int8 char
#           define uint8 unsigned char
#           ifdef __WIN32_
#              define int16 short int
#              define int32 long
#              define uint16 unsigned short int
#              define uint32 unsigned long
#           else
#              define int16 int
#              define int32 long
#              define uint16 unsigned int
#              define uint32 unsigned long
#           endif
#        endif
#      endif
#  endif
#endif
#endif


typedef int8   sint8;
typedef int16  sint16;
typedef int32  sint32;
typedef uint8  nat8;
typedef uint16 nat16;
typedef uint32 nat32;
#ifdef __GNUC__
//#if __WORDSIZE == 64
typedef  int64_t int64;
typedef uint64_t uint64;
typedef uint64_t nat64;
#else
typedef          __int64 int64;
typedef unsigned __int64 uint64;
typedef unsigned __int64 nat64;
#endif
//typedef const char      * const_ASCIIZ;  // constant null terminated char array
typedef char            *ASCIIZ;         // null terminated char array
//typedef const wchar_t   * const_UnicodeZ;

typedef wchar_t         *UnicodeZ;
// For MicroSoft Visual Studio enable Project properties: C/C++: Lanugage: Treat wchar_t as Built-in Type
// See http://msdn.microsoft.com/en-us/library/dh8che7s(v=vs.80).aspx

#define const_ASCIIZ    const char *
#define const_UnicodeZ  const wchar_t *

#ifndef UNUSED_arg
#define UNUSED_arg(arg) (void)(arg)
#endif

#endif
//primitive.h

