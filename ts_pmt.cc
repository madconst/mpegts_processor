#include "ts_pmt.h"

bool TsPMT::isEmpty()
{
    return programs.empty();
}

bool TsPMT::isPMT(unsigned int pid)
{
    for(std::vector<TsPmtProgram>::const_iterator it = programs.begin(); it != programs.end(); ++it)
    {
        if (it->elementary_pid == pid) {
            return true;
        }
    }
    return false;
}

size_t TsPMT::getNPrograms()
{
    return programs.size();
}