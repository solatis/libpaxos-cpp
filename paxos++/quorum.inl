namespace paxos {

template <class OutputIterator> inline OutputIterator
quorum::get_endpoints (
   OutputIterator       result) const
{
   return std::transform (servers_.begin (), servers_.end (),
                          result,
                          boost::bind (&map_type::value_type::first, _1));
}

};
