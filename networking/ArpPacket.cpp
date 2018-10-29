#include <cstdint>
#include <stdexcept>

#include "ArpPacket.hpp"

#include "NetworkAddress.hpp"

//==============================================================================
// Constructs an ARP packet with the minimum amount of information provided
// up-front; the hardware and protocol lengths have to be known because they
// define the length of their associated fields.  Memory for address and
// protocol fields is dynamically allocated.
//==============================================================================
ArpPacket::ArpPacket(std::uint8_t hlen, std::uint8_t plen) :
    DataPacket(1), // always aligned on 1 byte (not byte-aligned)
    htype(0),
    ptype(0),
    hlen(hlen),
    plen(plen),
    oper(0)
{
    addDataFields();
}

//==============================================================================
// Constructs an ARP packet in such a way that memory for the hardware and
// protocol fields are dynamically allocated and maintained internally.
//==============================================================================
ArpPacket::ArpPacket(std::uint16_t htype,
                     std::uint16_t ptype,
                     std::uint8_t  hlen,
                     std::uint8_t  plen,
                     std::uint16_t oper) :
    DataPacket(1), // always aligned on 1 byte (not byte-aligned)
    htype(htype),
    ptype(ptype),
    hlen(hlen),
    plen(plen),
    oper(oper)
{
    addDataFields();
}

//==============================================================================
// Constructs an ARP packet by dynamically allocating memory and copying
// provided NetworkAddress data into them.
//==============================================================================
ArpPacket::ArpPacket(std::uint16_t   htype,
                     std::uint16_t   ptype,
                     std::uint8_t    hlen,
                     std::uint8_t    plen,
                     std::uint16_t   oper,
                     NetworkAddress* sha,
                     NetworkAddress* spa,
                     NetworkAddress* tha,
                     NetworkAddress* tpa,
                     bool            network_address_owned_sha,
                     bool            network_address_owned_spa,
                     bool            network_address_owned_tha,
                     bool            network_address_owned_tpa) :
    DataPacket(1),  // always aligned on 1 byte (not byte-aligned)
    htype(htype),
    ptype(ptype),
    hlen(hlen),
    plen(plen),
    oper(oper),
    sha_owned(network_address_owned_sha),
    spa_owned(network_address_owned_spa),
    tha_owned(network_address_owned_tha),
    tpa_owned(network_address_owned_tpa)
{
    // Ensure the length of the NetworkAddresses matches the hlen and plen
    // sizes.  If they don't the user is doing it wrong.
    if (sha->getLengthBytes() != hlen)
    {
        throw std::runtime_error("sha->getLengthBytes() and hlen must match");
    }
    else if (spa->getLengthBytes() != plen)
    {
        throw std::runtime_error("spa->getLengthBytes() and plen must match");
    }
    else if (tha->getLengthBytes() != hlen)
    {
        throw std::runtime_error("tha->getLengthBytes() and hlen must match");
    }
    else if (tpa->getLengthBytes() != plen)
    {
        throw std::runtime_error("tpa->getLengthBytes() and plen must match");
    }

    if (sha_owned)
    {
        this->sha = new NetworkAddress(hlen);
        *this->sha = *sha;
    }
    else
    {
        this->sha = sha;
    }

    if (spa_owned)
    {
        this->spa = new NetworkAddress(plen);
        *this->spa = *spa;
    }
    else
    {
        this->spa = spa;
    }

    if (tha_owned)
    {
        this->tha = new NetworkAddress(hlen);
        *this->tha = *tha;
    }
    else
    {
        this->tha = tha;
    }

    if (tpa_owned)
    {
        this->tpa = new NetworkAddress(plen);
        *this->tpa = *tpa;
    }
    else
    {
        this->tpa = tpa;
    }

    addDataFields();
}

//==============================================================================
// Constructs an ARP packet using hardware and protocol field memory that can be
// maintained either internally or externally.
//==============================================================================
ArpPacket::ArpPacket(std::uint16_t  htype,
                     std::uint16_t  ptype,
                     std::uint8_t   hlen,
                     std::uint8_t   plen,
                     std::uint16_t  oper,
                     unsigned char* buffer_sha,
                     unsigned char* buffer_spa,
                     unsigned char* buffer_tha,
                     unsigned char* buffer_tpa,
                     bool           network_address_raw_owned_sha,
                     bool           network_address_raw_owned_spa,
                     bool           network_address_raw_owned_tha,
                     bool           network_address_raw_owned_tpa) :
    DataPacket(1),  // always aligned on 1 byte (not byte-aligned)
    htype(htype),
    ptype(ptype),
    hlen(hlen),
    plen(plen),
    oper(oper),
    sha_owned(network_address_raw_owned_sha),
    spa_owned(network_address_raw_owned_spa),
    tha_owned(network_address_raw_owned_tha),
    tpa_owned(network_address_raw_owned_tpa)
{
    sha = new NetworkAddress(buffer_sha, hlen, network_address_raw_owned_sha);
    spa = new NetworkAddress(buffer_spa, plen, network_address_raw_owned_spa);
    tha = new NetworkAddress(buffer_tha, hlen, network_address_raw_owned_tha);
    tpa = new NetworkAddress(buffer_tpa, plen, network_address_raw_owned_tpa);

    addDataFields();
}

//==============================================================================
ArpPacket::~ArpPacket()
{
    if (sha_owned)
    {
        delete sha;
        sha = 0;
    }

    if (spa_owned)
    {
        delete spa;
        spa = 0;
    }

    if (tha_owned)
    {
        delete tha;
        tha = 0;
    }

    if (tpa_owned)
    {
        delete tpa;
        tpa = 0;
    }
}

//==============================================================================
void ArpPacket::addDataFields()
{
    addDataField(&htype);
    addDataField(&ptype);
    addDataField(&hlen);
    addDataField(&plen);
    addDataField(&oper);
    addDataField(sha);
    addDataField(spa);
    addDataField(tha);
    addDataField(tpa);
}
