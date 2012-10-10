namespace paxos { namespace detail {

inline paxos_request_queue::paxos_request_queue ()
   : request_being_processed_ (false)
{
}

inline bool &
paxos_request_queue::request_being_processed ()
{
   return request_being_processed_;
}


}; };
