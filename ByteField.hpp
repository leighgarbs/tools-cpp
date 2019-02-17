#if !defined BYTE_FIELD_HPP
#define BYTE_FIELD_HPP

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <string>

#include "RawDataField.hpp"

#include "misc.hpp"

// Raw binary data as a sequence of bytes.  Implements RawDataField increasing
// or decreasing significance byte indexing.
class ByteField : public RawDataField
{
public:

    // Dynamically allocates and maintains a byte field that is "length_bytes"
    // in size.  All bits are initially unset (set to 0).  Storage is
    // dynamically allocated.
    // cppcheck-suppress noExplicitConstructor
    ByteField(unsigned long length_bytes);

    // Behavior depends on the value of "memory_internal".  If "memory_internal"
    // is true, the data at "buffer" of length "length_bytes" will be copied
    // into dynamically-allocated memory internal to this class.  If
    // "memory_internal" is false, the data at "buffer" of length "length_bytes"
    // will be used by this class in-place and no dynamic memory allocation will
    // occur.
    ByteField(std::uint8_t* buffer,
              unsigned long length_bytes,
              bool          memory_internal = true);

    // Copy constructor; dynamically allocates and maintains a byte field that
    // is "length_bytes" in size, and then copies the given byte field into this
    // newly-allocated memory.
    ByteField(const ByteField& byte_field);

    // Will free the memory at "byte_field_raw" if it is owned by this class
    virtual ~ByteField();

    // Reads from the "buffer" memory location plus an offset of "bit_offset"
    // bits.  No byteswapping is performed even when "source_byte_order" doesn't
    // match host byte ordering, since this is just raw data.
    virtual unsigned long readRaw(std::uint8_t*   buffer,
                                  misc::ByteOrder source_byte_order,
                                  unsigned long   offset_bits);

    // Writes to the "buffer" memory location plus an offset of "bit_offset"
    // bits.  No byteswapping is performed even when "source_byte_order" doesn't
    // match host byte ordering, since this is just raw data.
    virtual unsigned long writeRaw(std::uint8_t*   buffer,
                                   misc::ByteOrder destination_byte_order,
                                   unsigned long   offset_bits) const;

    // Returns the size of this byte field in bits.  This will equal the number
    // of bits written by writeRaw() and read by readRaw().
    virtual unsigned long getLengthBits() const;

    // Accessor to individual bits, returns a copy
    std::uint8_t getByte(unsigned long index) const;

    // Mutator of individual bits
    void setByte(unsigned long index, std::uint8_t byte);

    ByteField& operator=(const ByteField& byte_field);

private:

    // Raw byte field is stored at this location
    std::uint8_t* byte_field_raw;

    // Raw byte field is this many bytes in length
    unsigned long length_bytes;
};

//==============================================================================
inline unsigned long ByteField::getLengthBits() const
{
    return length_bytes * BITS_PER_BYTE;
}

//==============================================================================
inline std::uint8_t ByteField::getByte(unsigned long index) const
{
    throwIfIndexOutOfRange(index, getLengthBytes());

    unsigned long real_index = index;

    if (getIndexingMode() == misc::LS_ZERO)
    {
        real_index = length_bytes - index - 1;
    }

    return byte_field_raw[real_index];
}

//==============================================================================
inline void ByteField::setByte(unsigned long index, std::uint8_t byte)
{
    throwIfIndexOutOfRange(index, getLengthBytes());

    unsigned long real_index = index;

    if (getIndexingMode() == misc::LS_ZERO)
    {
        real_index = length_bytes - index - 1;
    }

    byte_field_raw[real_index] = byte;
}

bool operator==(const ByteField& lhs, const ByteField& rhs);
bool operator!=(const ByteField& lhs, const ByteField& rhs);

#endif