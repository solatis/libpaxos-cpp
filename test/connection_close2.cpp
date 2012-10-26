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
   test_strategy (
      paxos::durable::storage & storage)
      : paxos::detail::strategy::basic_paxos::protocol::strategy::strategy (storage) {}

   /*!
     \brief Overloaded from parent
    */
   virtual void
   accept (      
      paxos::detail::tcp_connection_ptr         leader_connection,
      paxos::detail::command const &            command,
      paxos::detail::quorum::server_view &      quorum,
      paxos::detail::paxos_context &            state)

      {
         leader_connection->socket ().close ();
      }

};


class test_strategy_factory : public paxos::detail::strategy::factory
{
public:

   test_strategy_factory (
      paxos::durable::storage & storage)
      : storage_ (storage)
      {
      }


   virtual paxos::detail::strategy::strategy *
   create () const   
      {
         return new test_strategy (storage_);
      }

private:
   paxos::durable::storage &    storage_;

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
   configuration.set_strategy_factory (new test_strategy_factory (configuration.durable_storage ()));

   paxos::server server1 ("127.0.0.1", 1337, callback);
   paxos::server server2 ("127.0.0.1", 1338, callback);
   paxos::server server3 ("127.0.0.1", 1339, callback, configuration);
   paxos::client client;

   server1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   server3.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
   client.add  ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

   /*!
     This would be retried and shouldn't cause any troubles.
    */
   PAXOS_ASSERT_EQ (client.send ("foo").get (), "bar");
   PAXOS_ASSERT_EQ (response_count, 4);

   PAXOS_INFO ("test succeeded");
}


