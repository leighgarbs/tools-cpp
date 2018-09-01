#if !defined WINDOWS_RAW_SOCKET_IMPL_HPP
#define WINDOWS_RAW_SOCKET_IMPL_HPP

#include <WinSock2.h>

#include "RawSocketImpl.hpp"

// Defines a socket implementation specific to Windows.
class WindowsRawSocketImpl : public RawSocketImpl
{
public:

    // Constructs a new Windows socket.  The 'protocol' argument should be an
    // IPPROTO_* enumeration value (defined in ws2def.h)
    explicit WindowsRawSocketImpl(int protocol);

    // Closes the associated socket.
    virtual ~WindowsRawSocketImpl();

    // Enables blocking on reads and writes.
    virtual bool enableBlocking();

    // Disables blocking on reads and writes.
    virtual bool disableBlocking();

    // Returns whether or not this socket blocks.
    virtual bool isBlockingEnabled();

    // Enables a timeout of the given length (in seconds) on blocking
    // operations.  A negative timeout value disables the blocking timeout.  On
    // Windows, resolution is millisecond-level
    virtual void setBlockingTimeout(double blocking_timeout);

    // Returns the current blocking timeout.
    virtual double getBlockingTimeout() const;

    // Sets whether or not the IP header is expected in calls to write
    bool setIncludeIPHeader(bool include_ip_header);

    // Gets whether or not the IP header is expected in calls to write
    bool getIncludeIPHeader() const;

    // Sets the interface from which to receive data.  On Windows, the receive
    // interface is specified by IP address.
    bool setInputInterface(const std::string& interface_ip);

    // Returns string representation of the IP address of the interface input is
    // being received from.
    void getInputInterface(std::string& interface_ip) const;

    // Sets the IP address data will be sent to
    void setDestinationIP(const std::string& destination_ip);

    // Gets the IP address data will be sent to
    void getDestinationIP(std::string& destination_ip) const;

    // Gets the source IP address of the last received packet
    void getPeerAddress(std::string& peer_address) const;

    // Reads the specified amount of data from this socket into the specified
    // buffer.
    virtual int read(char* buffer, unsigned int size);

    // Writes the specified amount of data to this socket from the specified
    // buffer.
    virtual int write(const char* buffer, unsigned int size);

    // Forces this socket to discard any received data.
    virtual void clearBuffer();

private:

    // Descriptor for this socket
    SOCKET socket_fd;

    // Do socket operations block?
    bool is_blocking;

    // Stores IP address to send to
    sockaddr_in send_addr;

    // Address info from the last received packet
    sockaddr_in last_source_addr;

    // Send and receive IP address strings
    std::string recv_addr_str;
    std::string send_addr_str;

    // User-given blocking timeout length
    double blocking_timeout;

    // The length of blocking timeouts, in milliseconds
    INT ts_blocking_timeout;
};

inline
void WindowsRawSocketImpl::getInputInterface(std::string& interface_ip) const
{
    interface_ip = recv_addr_str;
}

inline
void WindowsRawSocketImpl::getDestinationIP(std::string& destination_ip) const
{
    interface_ip = send_addr_str;
}

inline
void WindowsRawSocketImpl::getPeerAddress(std::string& peer_address) const
{
    peer_address = inet_ntoa(last_source_addr.sin_addr);
}

#endif
