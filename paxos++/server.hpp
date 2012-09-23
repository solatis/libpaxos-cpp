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

#ifndef LIBPAXOS_CPP_SERVER_HPP
#define LIBPAXOS_CPP_SERVER_HPP

#include <string>
#include <stdint.h>

#include <boost/asio.hpp>

#include "detail/tcp_connection.hpp"
#include "detail/quorum.hpp"

namespace paxos {

/*!
  \brief Represents a paxos server.

  This is the main entry point which communications within the paxos quorum take place.
  The open () function will open the server socket and makes it ready to accept connections
  from the quorum. 

  However, it does not process any messages until handle_events () is called. 
 */
class server
{
public:

   /*!
     \brief Opens socket to listen on port
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
     \param address     Interface/ip to listen for new connections
     \param port        Port to listen for new connections
    */
   server (
      boost::asio::io_service & io_service,
      std::string const &       address,
      int16_t                   port);
   
   /*!
     \brief Stops listening for new connections
    */
   void
   close ();

private:

   void
   accept ();

   void
   handle_accept (
      detail::tcp_connection::pointer   new_connection,
      boost::system::error_code const & error);


private:

   boost::asio::ip::tcp::acceptor       acceptor_;
   detail::quorum                       quorum_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
