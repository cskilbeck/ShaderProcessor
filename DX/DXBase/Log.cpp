//////////////////////////////////////////////////////////////////////

#include "DXBase.h"

//////////////////////////////////////////////////////////////////////

namespace DX {

//////////////////////////////////////////////////////////////////////

namespace Logging {

int log_level = Info;

FILE *log_file = null;
FILE *error_log_file = null;

}    // namespace Logging

//////////////////////////////////////////////////////////////////////

void Log_SetOutputFiles(wchar const *log_filename, wchar const *error_log_filename /* = null */)
{
#if !defined(DISABLE_LOGGING)
    _wfopen_s(&Logging::log_file, log_filename, L"w");
    if(error_log_filename == null || wcscmp(log_filename, error_log_filename) == 0) {
        Logging::error_log_file = Logging::log_file;
    } else {
        _wfopen_s(&Logging::error_log_file, error_log_filename, L"w");
    }
#endif
}

//////////////////////////////////////////////////////////////////////

void Log_Close()
{
    if(Logging::error_log_file != null && Logging::error_log_file != Logging::log_file) {
        fclose(Logging::error_log_file);
        Logging::error_log_file = null;
    }

    if(Logging::log_file != null) {
        fclose(Logging::log_file);
        Logging::log_file = null;
    }
}

//////////////////////////////////////////////////////////////////////

void Log_SetLevel(int level)
{
    if(level < min_log_level) {
        level = (int)min_log_level;
    }
    if(level > max_log_level) {
        level = (int)max_log_level;
    }
    Logging::log_level = level;
}


} // namespace DX
