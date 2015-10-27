#ifndef TS_HEADER_H_
#define TS_HEADER_H_

#include <boost/cstdint.hpp>

#include "bitfield.hpp"

union TsHeader
{
    unsigned char data[4];
    BitField_BE<0,8>   sync_byte;
    BitField_BE<8,1>   transport_error;
    BitField_BE<9,1>   payload_unit_start;
    BitField_BE<10,1>  transport_priority;
    BitField_BE<11,13> pid;
    BitField_BE<24,2>  scrambling_control;
    BitField_BE<26,1>  adaptation_field_exists;
    BitField_BE<27,1>  contains_payload;
    BitField_BE<28,4>  continuity_counter;
};

#endif