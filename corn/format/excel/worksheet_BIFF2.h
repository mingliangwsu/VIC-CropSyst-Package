#ifndef worksheet_BIFF2H
#define worksheet_BIFF2H
namespace CORN {
namespace Excel_BIFF                                                            //190107
{
//______________________________________________________________________________
class Worksheet_BIFF2
: public Worksheet
{
public:  // Methods specific to BIFF2
   BIFF2_rec_INTEGER &provide_INTEGER_record(uint16 col)              provides_;
   BIFF_rec_NUMBER &provide_NUMBER_record(uint16 col)                 provides_;
   BIFF_rec_LABEL &provide_LABEL_record(uint16 col)                   provides_;
};
//______________________________________________________________________________
} // namespace Excel_BIFF                                                       //190107


};

#endif
