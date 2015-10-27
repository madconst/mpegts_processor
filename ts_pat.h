#ifndef TS_PAT_H_
#define TS_PAT_H_

#include <vector>
#include "bitfield.hpp"

union TsPatProgram
{
    unsigned char data[4];
    BitField_BE<0,16> program_number;
    BitField_BE<19,13> program_map_pid;
    BitField_BE<19,13> network_pid;
};

struct TsPAT
{
    union {
        unsigned char data[8];
        BitField_BE<0,8> table_id;
        BitField_BE<8,1> section_syntax_indicator;
        BitField_BE<9,1> private_bit;
        BitField_BE<12,12> section_length;
        BitField_BE<24,16> transport_stream_id;
        BitField_BE<42,5> version_number;
        BitField_BE<47,1> current_next_indicator;
        BitField_BE<48,8> section_number;
        BitField_BE<56,8> last_section_number;
    };
    std::vector<TsPatProgram> programs;

    bool isEmpty();
    bool isPMT(unsigned int pid);
};

#endif