#ifndef directory_entryH
#define directory_entryH

namespace CORN {
namespace OS {
namespace FS {
/*
   // The bit mask and enumeration is not arbitrary, 
   // The bitmask and sequence matches the
   // high order nyble of the st_mode word16 of the
   // directory entry 'stat' (POSIX complient)

   Note that dirent.h defines  DT_xxx

A comment in the sys/stat.h file seems to indicate that the
st_mode 0 could represent a file, but the S_ISREG macro only test for value 8
*/

typedef nat16 Directory_entry_bitmask;

#ifdef __cplusplus
const Directory_entry_bitmask         FIFO_bit = 0x0001; //  1 0000000000000001
const Directory_entry_bitmask    character_bit = 0x0002; //  2 0000000000000010
const Directory_entry_bitmask undefined_03_bit = 0x0004; //  3 0000000000000100
const Directory_entry_bitmask    directory_bit = 0x0008; //  4 0000000000001000
const Directory_entry_bitmask undefined_05_bit = 0x0010; //  5 0000000000010000
const Directory_entry_bitmask        block_bit = 0x0020; //  6 0000000000100000
const Directory_entry_bitmask undefined_07_bit = 0x0040; //  7 0000000001000000
const Directory_entry_bitmask         file_bit = 0x0080; //  8 0000000010000000
const Directory_entry_bitmask undefined_09_bit = 0x0100; //  9 0000000100000000
const Directory_entry_bitmask         link_bit = 0x0200; // 10 0000001000000000
const Directory_entry_bitmask undefined_11_bit = 0x0400; // 11 0000010000000000
const Directory_entry_bitmask       socket_bit = 0x0800; // 12 0000100000000000
const Directory_entry_bitmask undefined_13_bit = 0x1000; // 13 0001000000000000
const Directory_entry_bitmask          WTH_bit = 0x2000; // 14 0010000000000000
const Directory_entry_bitmask undefined_15_bit = 0x4000; // 15 0100000000000000
const Directory_entry_bitmask undefined_16_bit = 0x8000; // 16 1000000000000000
#else
#define         FIFO_bit 0x0001
#define    character_bit 0x0002
#define undefined_03_bit 0x0004
#define    directory_bit 0x0008
#define undefined_05_bit 0x0010
#define        block_bit 0x0020
#define undefined_07_bit 0x0040
#define      regular_bit 0x0080
#define undefined_09_bit 0x0100
#define         link_bit 0x0200
#define undefined_11_bit 0x0400
#define       socket_bit 0x0800
#define undefined_13_bit 0x1000
#define          WTH_bit 0x2000
#define undefined_15_bit 0x4000
#define undefined_16_bit 0x8000
#endif

//_Directory_entry_bitmask_____________________________________________________/
enum Directory_entry_type
{
   // Note: This enumeration is not arbitrary, it matches the
   // high order nyble of the st_mode word16 of the
   // directory entry 'stat' (POSIX complient)
    unknown_entry      = 0
   ,        FIFO_entry = 1   // DT_FIFO
   ,   character_entry = 2   // DT_CHR
   ,undefined_03_entry = 3   // check posix
   ,   directory_entry = 4   // DT_DIR
   ,undefined_05_entry = 5   // check posix
   // The following are defined in dirent.h
   ,       block_entry = 6  // DT_BLK
   ,undefined_07_entry = 7
   ,     regular_entry = 8  // DT_REG
   ,undefined_09_entry = 9
   ,        link_entry =10  // DT_LNK  // I think ymbolic
   ,undefined_11_entry =11
   ,      socket_entry =12  // DT_SOCK
   ,undefined_13_entry =13
   ,         WTH_entry =14  // DT_WHT  ???
,directory_entry_type_count
};
static const Directory_entry_type file_entry = regular_entry;
//_Directory_entry_bit________________________________________________________/
/* Note that dirent.h defines
    DT_UNKNOWN = 0,
# define DT_UNKNOWN        DT_UNKNOWN
    DT_FIFO = 1,
# define DT_FIFO        DT_FIFO
    DT_CHR = 2,
# define DT_CHR                DT_CHR
    DT_DIR = 4,
# define DT_DIR                DT_DIR
    DT_BLK = 6,
# define DT_BLK                DT_BLK
    DT_REG = 8,
# define DT_REG                DT_REG
    DT_LNK = 10,
# define DT_LNK                DT_LNK
    DT_SOCK = 12,
# define DT_SOCK        DT_SOCK
    DT_WHT = 14
# define DT_WHT                DT_WHT
*/
}}}//_namespace CORN:OS:FS__________________________________________2019-12-12_/
#endif
