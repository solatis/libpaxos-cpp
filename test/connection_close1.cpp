/*!
  Tests what happens when a connection closes after a follower has received a 'prepare' request
 */

#include <atomic>

#include <paxos++/client.hpp>
#include <paxos++/server.hpp>
#include <paxos++/configuration.hpp>
#include <paxos++/detail/util/debug.hpp>

#include <paxos++/detail/tcp_connection.hpp>
#include <paxos++/detail/strategy/factory.hpp>
#include <paxos++/detail/strategy/basic_paxos/protocol/strategy.hpp>


/*!
  This is our "bad apple" paxos strategy, which will close the connection whenever
  a prepare request is received. This should generate an error, cause the node to be
  marked as dead, and make the paxos call recover the next time it's made.
 */

class test_strategy : public paxos::detail::strategy::basic_paxos::protocol::strategy
{
public:
   /*!
     \brief Overloaded from parent
    */
   virtual void
   prepare (      
      paxos::detail::tcp_connection_ptr leader_connection,
      paxos::detail::command const &    command,
      paxos::detail::quorum::quorum &   quorum,
      paxos::detail::paxos_context &    state) const 

      {
         leader_connection->socket ().close ();
      }

};

class test_strategy_factory : public paxos::detail::strategy::factory
{
public:

   virtual paxos::detail::strategy::strategy *
   create () const   
      {
         return new test_strategy ();
      }

};


int main ()
{
   std::atomic <uint16_t> response_count (0);

   paxos::server::callback_type callback =
      [& response_count](std::string const & workload) -> std::string
      {
         ++response_count;

         return "bar";
      };

   paxos::configuration configuration;
   configuration.set_strategy_factory (new test_strategy_factory ());

   paxos::server server1 ("127.0.0.1", 1337, callback);
   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::server server3 ("127.0.0.1", 1339, callback, configuration);

   server1.add ("127.0.0.1", 1337);
   server1.add ("127.0.0.1", 1338);
   server1.add ("127.0.0.1", 1339);

   server2.add ("127.0.0.1", 1337);
   server2.add ("127.0.0.1", 1338);
   server2.add ("127.0.0.1", 1339);

   server3.add ("127.0.0.1", 1337);
   server3.add ("127.0.0.1", 1338);
   server3.add ("127.0.0.1", 1339);

   server1.start ();
   server2.start ();
   server3.start ();

   paxos::client client;
   client.add ("127.0.0.1", 1337);
   client.add ("127.0.0.1", 1338);
   client.add ("127.0.0.1", 1339);
   client.start ();   

   PAXOS_ASSERT (client.send ("foo", 10).get () == "bar");

}


