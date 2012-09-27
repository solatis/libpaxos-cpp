namespace paxos {

inline quorum::map_type &
quorum::servers ()
{
   return servers_;
}


inline quorum::map_type const &
quorum::servers () const
{
   return servers_;
}

};
