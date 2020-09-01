#include "emitter_A.h"
namespace CORN
{
//____________________________________________________________________________
Emitter_abstract::Emitter_abstract
(const CS::Optation *optation_/*=0*/)
: optation                                                           (optation_)
, date_time_column_index                                                     (2)
{
}
//_____________________________________________________________________________/
Emitter_abstract_text::Emitter_abstract_text
(const char *source_filename_
,const CS::Optation *optation_/*=0*/)
: Emitter_abstract                                                   (optation_)
, source_stream_owned                                         (source_filename_)
, source_stream                                            (source_stream_owned)
{
}
//_____________________________________________________________________________/
Emitter_abstract_text::Emitter_abstract_text
(std::istream        &source_stream_ref
,const CS::Optation  *optation_/*=0*/)
: Emitter_abstract                                                   (optation_)
, source_stream                                              (source_stream_ref)
{
}
//_____________________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/

