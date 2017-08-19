// Leigh Garbs

#include "WindowsRawSocket.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <string>

#include "WindowsSocketCommon.hpp"

//=============================================================================
// Creates a Windows raw socket
//=============================================================================
WindowsRawSocket::WindowsRawSocket(int protocol) :
  SocketImpl(),
  socket_fd(INVALID_SOCKET),
  is_blocking(true)
{
  // Initialize address structures
  memset(&send_addr,        0, sizeof(sockaddr_in));
  memset(&last_source_addr, 0, sizeof(sockaddr_in));

  blocking_timeout = -1.0;
  ts_blocking_timeout = 0;

  // Initialize the Windows socket driver
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::WindowsRawSocket");
    return;
  }

  // Create the socket
  socket_fd = socket(AF_INET, SOCK_RAW, protocol);

  // Check for socket creation errors
  if (socket_fd == INVALID_SOCKET)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::WindowsRawSocket");
    return;
  }

  // Enable broadcasting
  bool t = true;
  if (setsockopt(socket_fd,
                 SOL_SOCKET,
                 SO_BROADCAST,
                 reinterpret_cast<char*>(&t),
                 sizeof(bool)) == SOCKET_ERROR)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::WindowsRawSocket");
    return;
  }
}

//=============================================================================
// Shuts this socket down
//=============================================================================
WindowsRawSocket::~WindowsRawSocket()
{
  WindowsSocketCommon::shutdown(socket_fd);
}

//=============================================================================
// Sets blocking status
//=============================================================================
bool WindowsRawSocket::enableBlocking()
{
  return WindowsSocketCommon::enableBlocking(socket_fd, is_blocking);
}

//=============================================================================
// Sets blocking status
//=============================================================================
bool WindowsRawSocket::disableBlocking()
{
  return WindowsSocketCommon::disableBlocking(socket_fd, is_blocking);
}

//=============================================================================
// Returns if the socket can block
//=============================================================================
bool WindowsRawSocket::isBlockingEnabled()
{
  return is_blocking;
}

//=============================================================================
// Sets the blocking timeout
//=============================================================================
void WindowsRawSocket::setBlockingTimeout(double blocking_timeout)
{
  WindowsSocketCommon::setBlockingTimeout(blocking_timeout,
                                          this->blocking_timeout,
                                          ts_blocking_timeout);
}

//=============================================================================
// Gets the blocking timeout
//=============================================================================
double WindowsRawSocket::getBlockingTimeout() const
{
  return blocking_timeout;
}

//=============================================================================
// Sets whether or not the IP header is expected in calls to write
//=============================================================================
bool WindowsRawSocket::setIncludeIPHeader(bool include_ip_header)
{
  // Input to setsockopt has to be an integer
  int optval = static_cast<int>(include_ip_header);

  if (setsockopt(socket_fd,
                 IPPROTO_IP,
                 IP_HDRINCL,
                 reinterpret_cast<char*>(&optval),
                 sizeof(int)) == SOCKET_ERROR)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::setIncludeIPHeader");
    return false;
  }

  return true;
}

//=============================================================================
// Gets whether or not the IP header is expected in calls to write
//=============================================================================
bool WindowsRawSocket::getIncludeIPHeader() const
{
  // Output from getsockopt will be an integer
  int include_ip_header = 0;
  int size = sizeof(int);

  if (getsockopt(socket_fd,
                 IPPROTO_IP,
                 IP_HDRINCL,
                 reinterpret_cast<char*>(&include_ip_header),
                 &size) == SOCKET_ERROR)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::getIncludeIPHeader");
    return false;
  }

  return include_ip_header != 0;
}

//=============================================================================
// Sets the interface this socket will get data from
//=============================================================================
bool WindowsRawSocket::setInputInterface(const std::string& interface_ip)
{
  // Record the "input interface"; on Windows this is an IPv4 address
  recv_addr_str = interface_ip;

  // Fill in new input interface address info
  sockaddr_in recv_addr;
  recv_addr.sin_family      = AF_INET;
  recv_addr.sin_addr.s_addr = inet_addr(interface_ip.c_str());

  if (bind(socket_fd,
           (sockaddr*)&recv_addr,
           sizeof(sockaddr_in)) == SOCKET_ERROR)
  {
    WindowsSocketCommon::printErrorMessage(
      "WindowsRawSocket::setInputInterface");
    return false;
  }

  return true;
}

//=============================================================================
// Set the interface this socket will send data to
//=============================================================================
void WindowsRawSocket::setDestinationIP(const std::string& interface_ip)
{
  // Record the "input interface"; on Windows this is an IPv4 address
  send_addr_str = interface_ip;

  // Fill in new input interface address info
  send_addr.sin_family      = AF_INET;
  send_addr.sin_addr.s_addr = inet_addr(interface_ip.c_str());
}

//=============================================================================
// Reads data from socket into buffer
//=============================================================================
int WindowsRawSocket::read(char* buffer, unsigned int size)
{
  return WindowsSocketCommon::read(
    socket_fd,
    buffer,
    size,
    blocking_timeout,
    ts_blocking_timeout,
    reinterpret_cast<sockaddr*>(&last_source_addr),
    sizeof(sockaddr_in),
    is_blocking);
}

//=============================================================================
// Writes data to socket
//=============================================================================
int WindowsRawSocket::write(const char* buffer, unsigned int size)
{
  return WindowsSocketCommon::write(socket_fd,
                                    buffer,
                                    size,
                                    blocking_timeout,
                                    ts_blocking_timeout,
                                    reinterpret_cast<sockaddr*>(&send_addr),
                                    sizeof(sockaddr_in),
                                    is_blocking);
}

//=============================================================================
// Clears socket of any received data
//=============================================================================
void WindowsRawSocket::clearBuffer()
{
  WindowsSocketCommon::clearBuffer(
    socket_fd,
    reinterpret_cast<sockaddr*>(&last_source_addr),
    sizeof(sockaddr_in),
    is_blocking);
}