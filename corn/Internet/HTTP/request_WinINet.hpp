#if (!defined(_WIN32) && ! defined(_WIN64))
#error Windows only
// These functions are only available under development environments for Windows
// For Linux or other, use request_cURL.hpp
#endif
#ifndef request_WinINetHPP
#define request_WinINetHPP

#include <Windows.h>
#include <string>
#include <stdio.h>
#include "corn/OS/uniform_resource_identifier.h"

namespace CORN     {
namespace Internet {
#include "CORN/Internet/HTTP/request_WinINet.h"
//______________________________________________________________________________
extern bool WinINet_request_to_string
   (int Method
   ,LPCSTR Host, LPCSTR url, LPCSTR header, LPSTR data
   ,std::string &result);
//_WinINet_request_to_string________________________________________2016-10-18_/
extern bool WinINet_request_URI_to_stream
   (int Method
   ,const CORN::OS::Uniform_resource_identifier &URI
   ,LPCSTR header, LPSTR data
   ,std::ostream &write_stream );
//_WinINet_request_URI_to_stream____________________________________2016-08-02_/
extern bool WinINet_request_URI_to_FILE
   (int Method
   ,const CORN::OS::Uniform_resource_identifier &URI
   ,LPCSTR header, LPSTR data
   ,FILE *to_file);
//_WinINet_request_URI_to_FILE______________________________________2016-10-18_/
}}//_namespace_CORN:Internet___________________________________________________/
#endif

