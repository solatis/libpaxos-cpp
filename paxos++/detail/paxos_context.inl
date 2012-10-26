namespace paxos { namespace detail {

inline paxos_context::processor_type const &
paxos_context::processor () const
{
   return processor_;
}

inline detail::strategy::strategy &
paxos_context::strategy ()
{
   return *strategy_;
}


inline request_queue::queue <strategy::request> &
paxos_context::request_queue ()
{
   return request_queue_;
}

}; };
