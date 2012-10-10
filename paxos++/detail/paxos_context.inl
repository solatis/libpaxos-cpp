namespace paxos { namespace detail {

inline paxos_context::paxos_context (
   processor_type const &       processor)
   : proposal_id_ (0),
     processor_ (processor)
{
}

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

inline paxos_request_queue &
paxos_context::request_queue ()
{
   return request_queue_;
}

}; };
