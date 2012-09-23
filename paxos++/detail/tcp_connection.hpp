/*!
  Copyright (c) 2012, Leon Mergen
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

 */

#ifndef LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace paxos { namespace detail {

/*!
  \brief Represents a connection to another paxos server.
 */
class tcp_connection 
   : public boost::enable_shared_from_this <tcp_connection>
{
public:

   typedef boost::shared_ptr <tcp_connection>   pointer;

   static pointer 
   create (
      boost::asio::io_service &        io_service);

   boost::asio::ip::tcp::socket &
   socket ();

   void
   write (
      std::string const &       message);
   
private:

   tcp_connection (
      boost::asio::io_service & io_service);

   void
   start_write ();

   void
   handle_write (
      boost::system::error_code const & error,
      size_t                            bytes_transferred);

private:

   boost::asio::ip::tcp::socket socket_;
   std::string                  buffer_;
};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_HPP
