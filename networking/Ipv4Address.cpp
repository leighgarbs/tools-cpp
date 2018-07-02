#include <cstdio>
#include <cstring>
#include <ios>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "Ipv4Address.hpp"

//==============================================================================
// Ipv4Address constructor; initializes to all zeros
//==============================================================================
Ipv4Address::Ipv4Address()
{
    initialize();
}

//==============================================================================
// Ipv4Address constructor; initializes to match the given string
//==============================================================================
Ipv4Address::Ipv4Address(const std::string& ipv4_address_str)
{
    initialize();

    *this = ipv4_address_str;
}

//==============================================================================
// Ipv4Address copy constructor; copies the address of the given IPv4 address
//==============================================================================
Ipv4Address::Ipv4Address(const Ipv4Address& ipv4_address)
{
    initialize();

    *this = ipv4_address;
}

//==============================================================================
// Defines how to convert a MacAddress to a std::string
//==============================================================================
Ipv4Address::operator std::string() const
{
    std::ostringstream tempstream;
    tempstream << *this;
    return tempstream.str();
}

//==============================================================================
// Ipv4Address destructor; does nothing since no dynamic memory is allocated
//==============================================================================
Ipv4Address::~Ipv4Address()
{
}

//==============================================================================
// Assigns a string to a MAC address
//==============================================================================
void Ipv4Address::readRaw(const char* buf)
{
    memcpy(&at(0), buf, IPV4_LENGTH_BYTES);
}

//==============================================================================
// Assigns a string to a MAC address
//==============================================================================
void Ipv4Address::writeRaw(char* buf) const
{
    memcpy(buf, &at(0), IPV4_LENGTH_BYTES);
}

//==============================================================================
// Assigns a string to a IPv4 address
//==============================================================================
Ipv4Address& Ipv4Address::operator=(const std::string& ipv4_address_str)
{
    std::istringstream tempstream(ipv4_address_str);
    tempstream >> *this;

    return *this;
}

//==============================================================================
// All constructors run this
//==============================================================================
void Ipv4Address::initialize()
{
    reserve(IPV4_LENGTH_BYTES);
    assign(IPV4_LENGTH_BYTES, 0);
}

//==============================================================================
// Writes string representation of self to the ostream
//==============================================================================
std::ostream& operator<<(std::ostream& os, const Ipv4Address& ipv4_address)
{
    // 16 characters for the whole representation; 12 for the actual numbers, 3
    // for the periods in-between, and 1 on the end for the null
    char ipv4_cstr[Ipv4Address::IPV4_MAX_STR_LENGTH_CHARS];
    ipv4_cstr[Ipv4Address::IPV4_MAX_STR_LENGTH_CHARS - 1] = 0;
    if (snprintf(ipv4_cstr,
                 Ipv4Address::IPV4_MAX_STR_LENGTH_CHARS,
                 "%hhu.%hhu.%hhu.%hhu",
                 static_cast<unsigned char>(ipv4_address.at(0)),
                 static_cast<unsigned char>(ipv4_address.at(1)),
                 static_cast<unsigned char>(ipv4_address.at(2)),
                 static_cast<unsigned char>(ipv4_address.at(3))) < 0)
    {
        // Something bad happened, so set the fail bit on the stream
        os.setstate(std::ios_base::failbit);
        return os;
    }

    return os << std::string(ipv4_cstr);
}

//==============================================================================
// Reads string representation of self from the istream
//==============================================================================
std::istream& operator>>(std::istream& is, Ipv4Address& ipv4_address)
{
    // Grab characters from the stream and store temporarily
    char tempstr[Ipv4Address::IPV4_MAX_STR_LENGTH_CHARS];
    is.get(tempstr, Ipv4Address::IPV4_MAX_STR_LENGTH_CHARS);

    unsigned int tempipv4[Ipv4Address::IPV4_LENGTH_BYTES];
    // Scan the temporary string as a IPv4 address
    if (sscanf(tempstr,
               "%u.%u.%u.%u",
               &tempipv4[0],
               &tempipv4[1],
               &tempipv4[2],
               &tempipv4[3]) != Ipv4Address::IPV4_LENGTH_BYTES)
    {
        // We didn't convert all 4 bytes.  Leave our internal state as-is but
        // set the fail bit on the stream so the user has some way of knowing
        is.setstate(std::ios_base::failbit);
        return is;
    }

    // Copy from temporary storage into permanent storage
    for (unsigned int i = 0; i < Ipv4Address::IPV4_LENGTH_BYTES; i++)
    {
        ipv4_address.at(i) = static_cast<unsigned char>(tempipv4[i]);
    }

    return is;
}

//==============================================================================
// Equality comparison, Ipv4Address == Ipv4Address
//==============================================================================
bool
operator==(const Ipv4Address& ipv4_address1, const Ipv4Address& ipv4_address2)
{
    return !memcmp(&ipv4_address1.at(0),
                   &ipv4_address2.at(0),
                   Ipv4Address::IPV4_LENGTH_BYTES);
}

//==============================================================================
// Equality comparison, Ipv4Address == std::string
//==============================================================================
bool
operator==(const Ipv4Address& ipv4_address1, const std::string& ipv4_address2)
{
    return ipv4_address1 == Ipv4Address(ipv4_address2);
}

//==============================================================================
// Equality comparison, std::string == Ipv4Address
//==============================================================================
bool
operator==(const std::string& ipv4_address1, const Ipv4Address& ipv4_address2)
{
    return Ipv4Address(ipv4_address1) == ipv4_address2;
}

//==============================================================================
// Inequality comparison, Ipv4Address != Ipv4Address
//==============================================================================
bool
operator!=(const Ipv4Address& ipv4_address1, const Ipv4Address& ipv4_address2)
{
    return !(ipv4_address1 == ipv4_address2);
}

//==============================================================================
// Inequality comparison, Ipv4Address != std::string
//==============================================================================
bool
operator!=(const Ipv4Address& ipv4_address1, const std::string& ipv4_address2)
{
    return !(ipv4_address1 == ipv4_address2);
}

//==============================================================================
// Inequality comparison, std::string != Ipv4Address
//==============================================================================
bool
operator!=(const std::string& ipv4_address1, const Ipv4Address& ipv4_address2)
{
    return !(ipv4_address1 == ipv4_address2);
}