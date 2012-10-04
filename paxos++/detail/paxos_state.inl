namespace paxos { namespace detail {

inline paxos_state::paxos_state (
   processor_type const &       processor)
   : proposal_id_ (0),
     processor_ (processor)
{
}

inline uint64_t &
paxos_state::proposal_id ()
{
   return proposal_id_;
}

inline uint64_t
paxos_state::proposal_id () const
{
   return proposal_id_;
}

inline paxos_state::processor_type const &
paxos_state::processor () const
{
   return processor_;
}


}; };
