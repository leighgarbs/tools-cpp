#include <cmath>
#include <cstring>
#include <new>
#include <stdexcept>

#include "RawDataField.hpp"

#include "DataField.hpp"
#include "misc.hpp"

//==============================================================================
RawDataField::RawDataField(unsigned long          length,
                           misc::DataUnits        length_units,
                           misc::DataIndexingMode indexing_mode) :
    RawDataField(length, length_units, indexing_mode, true)
{
}

//==============================================================================
RawDataField::RawDataField(std::uint8_t*          buffer,
                           unsigned long          length,
                           misc::DataUnits        length_units,
                           misc::DataIndexingMode indexing_mode,
                           bool                   memory_internal) :
    RawDataField(length, length_units, indexing_mode, memory_internal)
{
    if (memory_internal)
    {
        DataField::readRaw(buffer);
    }
    else
    {
        raw_data = buffer;
    }
}

//==============================================================================
RawDataField::RawDataField(const RawDataField& raw_data_field) :
    RawDataField(raw_data_field.getLengthBits(),
                 misc::BITS,
                 raw_data_field.getIndexingMode(),
                 true)
{
    raw_data_field.DataField::writeRaw(raw_data);
}

//==============================================================================
RawDataField::RawDataField(unsigned long          length,
                           misc::DataUnits        length_units,
                           misc::DataIndexingMode indexing_mode,
                           bool                   memory_internal) :
    DataField(),
    memory_internal(memory_internal),
    indexing_mode(indexing_mode)
{
    // Set length_bits appropriately
    if (length_units == misc::BYTES)
    {
        length_bits = length * BITS_PER_BYTE;
    }
    else if (length_units == misc::BITS)
    {
        length_bits = length;
    }
    else
    {
        std::runtime_error("Unsupported units type specified");
    }

    if (memory_internal)
    {
        // We're managing memory internally so we need some memory.  Memory
        // amount calculation is copied from the getLengthBytes definition in
        // DataField.  We don't use that directly here since getLengthBytes will
        // call getLengthBits on this class, and this class isn't fully
        // instantiated yet.
        raw_data = new std::uint8_t[static_cast<unsigned int>(
                std::ceil(static_cast<double>(length_bits) /
                          static_cast<double>(BITS_PER_BYTE)))];
    }
}

//==============================================================================
RawDataField::~RawDataField()
{
    if (memory_internal)
    {
        delete[] raw_data;
    }
}

//==============================================================================
unsigned long RawDataField::readRaw(std::uint8_t* buffer,
                                    misc::ByteOrder     source_byte_order,
                                    unsigned long       offset_bits)
{
    // No byteswapping regardless of "source_byte_order" setting
    memcpy(raw_data, buffer, getLengthBytes());
    return length_bits;
}

//==============================================================================
unsigned long RawDataField::writeRaw(
    std::uint8_t*   buffer,
    misc::ByteOrder destination_byte_order,
    unsigned long   offset_bits) const
{
    // No byteswapping regardless of "destination_byte_order" setting
    memcpy(buffer, raw_data, getLengthBytes());
    return length_bits;
}

//==============================================================================
template <class T>
void RawDataField::getBitsAsNumericType(T&           type_var,
                                        unsigned int start_bit,
                                        unsigned int count) const
{
    // Handle bad input
    if (start_bit >= length_bits || start_bit + count > length_bits)
    {
        throw std::out_of_range("Out-of-range bit(s) specified");
    }
    else if (count > sizeof(T) * BITS_PER_BYTE)
    {
        throw std::out_of_range("Not enough bits in the destination type");
    }

    // We could not do this and leave irrelevant bits untouched, but that
    // wouldn't match the usage convention of pretty much every other getter
    // class member function ever
    type_var = 0;

    // Use the given variable as if it were a bitfield, and set bits inside it
    RawDataField working_bitfield(reinterpret_cast<std::uint8_t*>(&type_var),
                                  sizeof(T),
                                  misc::BYTES,
                                  indexing_mode,
                                  false);

    // Copy all the bits; an alternative implementation would be to memcpy the
    // relevant data over, shift down and then mask out the irrelevant bits
    for (unsigned int i = 0; i < count; ++i)
    {
        working_bitfield.setBit(i, getBit(start_bit + i));
    }
}

// Use this macro to instantiate getBitsAsNumericType() for all the intrinsic
// numeric types.  "char" is arguably not a numeric type but let's include it
// since it's sometimes useful to use it as if it were a numeric type
#define INSTANTIATE_GETBITSASNUMERICTYPE(Type)                          \
    template void                                                       \
    RawDataField::getBitsAsNumericType(Type&, unsigned int, unsigned int) const;

INSTANTIATE_GETBITSASNUMERICTYPE(char);
INSTANTIATE_GETBITSASNUMERICTYPE(double);
INSTANTIATE_GETBITSASNUMERICTYPE(float);
INSTANTIATE_GETBITSASNUMERICTYPE(int);
INSTANTIATE_GETBITSASNUMERICTYPE(long);
INSTANTIATE_GETBITSASNUMERICTYPE(long double);
INSTANTIATE_GETBITSASNUMERICTYPE(long long);
INSTANTIATE_GETBITSASNUMERICTYPE(short);
INSTANTIATE_GETBITSASNUMERICTYPE(unsigned char);
INSTANTIATE_GETBITSASNUMERICTYPE(unsigned int);
INSTANTIATE_GETBITSASNUMERICTYPE(unsigned long);
INSTANTIATE_GETBITSASNUMERICTYPE(unsigned long long);
INSTANTIATE_GETBITSASNUMERICTYPE(unsigned short);

//==============================================================================
template <class T>
void RawDataField::setBitsAsNumericType(T            type_var,
                                        unsigned int start_bit,
                                        unsigned int count)
{
    // Handle bad input
    if (start_bit >= length_bits || start_bit + count > length_bits)
    {
        throw std::out_of_range("Out-of-range bit(s) specified");
    }
    else if (count > sizeof(T) * BITS_PER_BYTE)
    {
        throw std::out_of_range("Not enough bits in the source type");
    }

    // Use the given variable as if it were a bitfield, and set bits inside it
    RawDataField working_bitfield(reinterpret_cast<std::uint8_t*>(&type_var),
                                  sizeof(T),
                                  misc::BYTES,
                                  indexing_mode,
                                  false);

    // Copy all the bits; an alternative implementation would be to memcpy the
    // relevant data over, shift down and then mask out the irrelevant bits
    for (unsigned int i = 0; i < count; ++i)
    {
        setBit(start_bit + i, working_bitfield.getBit(i));
    }
}

// Use this macro to instantiate setBitsAsNumericType() for all the intrinsic
// numeric types.  "char" is arguably not a numeric type but let's include it
// since it's sometimes useful to use it as if it were a numeric type
#define INSTANTIATE_SETBITSASNUMERICTYPE(Type)                          \
    template void                                                       \
    RawDataField::setBitsAsNumericType(Type, unsigned int, unsigned int);

INSTANTIATE_SETBITSASNUMERICTYPE(char);
INSTANTIATE_SETBITSASNUMERICTYPE(double);
INSTANTIATE_SETBITSASNUMERICTYPE(float);
INSTANTIATE_SETBITSASNUMERICTYPE(int);
INSTANTIATE_SETBITSASNUMERICTYPE(long);
INSTANTIATE_SETBITSASNUMERICTYPE(long double);
INSTANTIATE_SETBITSASNUMERICTYPE(long long);
INSTANTIATE_SETBITSASNUMERICTYPE(short);
INSTANTIATE_SETBITSASNUMERICTYPE(unsigned char);
INSTANTIATE_SETBITSASNUMERICTYPE(unsigned int);
INSTANTIATE_SETBITSASNUMERICTYPE(unsigned long);
INSTANTIATE_SETBITSASNUMERICTYPE(unsigned long long);
INSTANTIATE_SETBITSASNUMERICTYPE(unsigned short);

//==============================================================================
void RawDataField::shiftLeft(unsigned int shift_bits)
{
    if (shift_bits >= length_bits)
    {
        throw std::runtime_error(
            "Requested shift amount must be less than the width of the field");
    }

    // A shift of 0 bits is a no-op
    if (shift_bits == 0)
    {
        return;
    }

    for (unsigned int i = length_bits - 1; i != shift_bits - 1; --i)
    {
        setBit(i, getBit(i - shift_bits));
    }

    // Shift in zeros
    for (unsigned int i = shift_bits - 1; i != 0; --i)
    {
        setBit(i, false);
    }

    // Least significant bit will always be 0
    setBit(0, false);
}

//==============================================================================
void RawDataField::shiftRight(unsigned int shift_bits)
{
    if (shift_bits >= length_bits)
    {
        throw std::runtime_error(
            "Requested shift amount must be less than the width of the field");
    }

    // A shift of 0 bits is a no-op
    if (shift_bits == 0)
    {
        return;
    }

    // Copy over the shifted bits
    for (unsigned int i = 0; i < length_bits - shift_bits; i++)
    {
        setBit(i, getBit(i + shift_bits));
    }

    // Shift in zeros
    for (unsigned int i = length_bits - shift_bits; i < length_bits; i++)
    {
        setBit(i, false);
    }
}

//==============================================================================
RawDataField& RawDataField::operator=(const RawDataField& bit_field)
{
    if (this != &bit_field)
    {
        bit_field.DataField::writeRaw(raw_data);
    }

    return *this;
}

//==============================================================================
RawDataField& RawDataField::operator<<=(unsigned int shift_bits)
{
    shiftLeft(shift_bits);
    return *this;
}

//==============================================================================
RawDataField& RawDataField::operator>>=(unsigned int shift_bits)
{
    shiftRight(shift_bits);
    return *this;
}

//==============================================================================
bool operator==(const RawDataField& bit_field1, const RawDataField& bit_field2)
{
    if (bit_field1.getLengthBits() != bit_field2.getLengthBits())
    {
        return false;
    }

    // We know both bit fields have equal length at this point
    unsigned int length_bytes = bit_field1.getLengthBytes();

    for (unsigned int i = 0; i < length_bytes; i++)
    {
        if (bit_field1.getByte(i) != bit_field2.getByte(i))
        {
            return false;
        }
    }

    return true;
}

//==============================================================================
bool operator!=(const RawDataField& bit_field1, const RawDataField& bit_field2)
{
    return !(bit_field1 == bit_field2);
}

//==============================================================================
RawDataField operator<<(const RawDataField& bit_field, unsigned int shift_bits)
{
    // Copy the bitfield then return a shifted copy
    RawDataField new_bit_field(bit_field);
    new_bit_field.shiftLeft(shift_bits);
    return new_bit_field;
}

//==============================================================================
RawDataField operator>>(const RawDataField& bit_field, unsigned int shift_bits)
{
    // Copy the bitfield then return a shifted copy
    RawDataField new_bit_field(bit_field);
    new_bit_field.shiftRight(shift_bits);
    return new_bit_field;
}