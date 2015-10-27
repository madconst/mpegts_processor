#include "ts_pat.h"

bool TsPAT::isEmpty()
{
    return programs.empty();
}

bool TsPAT::isPMT(unsigned int pid)
{
    for(std::vector<TsPatProgram>::const_iterator it = programs.begin(); it != programs.end(); ++it)
    {
        if (it->program_map_pid == pid) {
            return true;
        }
    }
    return false;
}