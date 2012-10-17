namespace paxos { namespace detail {

inline uint64_t &
paxos_context::proposal_id ()
{
   return proposal_id_;
}

inline uint64_t
paxos_context::proposal_id () const
{
   return proposal_id_;
}

inline paxos_context::processor_type const &
paxos_context::processor () const
{
   return processor_;
}

inline detail::strategy::strategy const &
paxos_context::strategy () const
{
   return *strategy_;
}


inline request_queue::queue <strategy::request> &
paxos_context::request_queue ()
{
   return request_queue_;
}

}; };
