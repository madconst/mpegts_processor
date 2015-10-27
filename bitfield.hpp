/*
Original BitField implementation by Evan Teran:
http://blog.codef00.com/2014/12/06/portable-bitfields-using-c11/
My version was ported to C++98 and adapted for big endian (network byte order).
*/

#ifndef BITFIELD_H_
#define BITFIELD_H_

#include <algorithm>
#include <cstddef>
#include <boost/type_traits/conditional.hpp>
#include <boost/cstdint.hpp>

using std::reverse_copy;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

namespace {
    template <size_t LastBit>
    struct MinimumTypeHelper {
        typedef
            typename boost::conditional<LastBit == 0 , void,
            typename boost::conditional<LastBit <= 8 , uint8_t,
            typename boost::conditional<LastBit <= 16, uint16_t,
            typename boost::conditional<LastBit <= 32, uint32_t,
            typename boost::conditional<LastBit <= 64, uint64_t,
            void>::type>::type>::type>::type>::type type;
    };
}

template <size_t Index, size_t Bits = 1>
class BitField {
private:
    enum {
        Mask = (1u << Bits) - 1u
    };

    typedef typename MinimumTypeHelper<Index + Bits>::type T;
public:
    template <class T2>
    BitField &operator=(T2 value) {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    operator T() const             { 
        return (value_ >> Index) & Mask;
    }
    BitField &operator++()         { return *this = *this + 1; }
    T operator++(int)              { T r = *this; ++*this; return r; }
    BitField &operator--()         { return *this = *this - 1; }
    T operator--(int)              { T r = *this; ++*this; return r; }

private:
    T value_;
};

template <size_t Index, size_t Bits = 1>
class BitField_BE {
private:
    typedef typename MinimumTypeHelper<Bits>::type T;
    enum {
        StartByte = Index / 8,
        EndByte = (Index + Bits - 1) / 8,
        NBytes = EndByte - StartByte + 1,
        BitsTotal = NBytes * 8,
        Bits_ = BitsTotal - Bits,
        Mask = (1ull << Bits) - 1u
    };
    typedef typename MinimumTypeHelper<NBytes*8>::type T1;
public:
    template <class T2>
    BitField_BE &operator=(T2 value)
    {
        T1 value_shifted = value << (Bits_ + StartByte * 8 - Index);
        uint8_t* pvalue_shifted = reinterpret_cast<uint8_t*>(&value_shifted);

        for(uint8_t i = 0; i < NBytes; ++i) {
            uint8_t byte = *(pvalue_shifted + NBytes - i - 1);
            uint64_t mask = ~(Mask << (Bits_ + StartByte * 8 - Index));
            data[StartByte + i] = (data[StartByte + i] & mask) | byte;
            mask = mask >> 8;
        }
        return *this;
    }

    operator T() const
    {
        T1 value_ = 0;
        uint8_t* pvalue_ = reinterpret_cast<uint8_t *>(&value_);
        reverse_copy(data + StartByte, data + EndByte + 1, pvalue_);
        T value_right_aligned = value_ >> (Bits_ + StartByte * 8 - Index);
        return value_right_aligned & Mask;
    }
    BitField_BE &operator++()      { return *this = *this + 1; }
    T operator++(int)              { T r = *this; ++*this; return r; }
    BitField_BE &operator--()      { return *this = *this - 1; }
    T operator--(int)              { T r = *this; ++*this; return r; }

private:
    uint8_t data[StartByte + NBytes];
};

#endif
