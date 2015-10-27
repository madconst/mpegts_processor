#ifndef TS_ADAPTATION_FIELD_H_
#define TS_ADAPTATION_FIELD_H_

#include "bitfield.hpp"

union TsAdaptationFieldBase
{
	unsigned char data[2];
	BitField_BE<0,8> length;
	BitField_BE<8,1> discontinuity;
	BitField_BE<9,1> random_access;
	BitField_BE<10,1> elementary_stream_priority;
	BitField_BE<11,1> pcr_flag;
	BitField_BE<12,1> opcr_flag;
	BitField_BE<13,1> splicing_point_flag;
	BitField_BE<14,1> transport_private_data_flag;
	BitField_BE<15,1> extension_flag;
};

union TsAdaptationFieldPcr
{
	unsigned char data[6];
	BitField_BE<0,33> base;
	BitField_BE<39,9> extension;
};

struct TsAdaptationField
{
	TsAdaptationFieldBase base;
	TsAdaptationFieldPcr pcr;
	TsAdaptationFieldPcr opcr;
	char splice_countdown;
};

#endif