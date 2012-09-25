namespace paxos { namespace detail { namespace protocol {


template <typename Protocol> inline session <Protocol>::session (
   detail::protocol::protocol & protocol)       
   : protocol_ (protocol)
{
}


template <typename Protocol> inline Protocol &
session <Protocol>::create ()
{
   return *(sessions_.insert (sessions_.end (),
                              new Protocol (protocol_)));
}

template <typename Protocol> inline void
session <Protocol>::remove (
   Protocol const &  session)
{

   struct equals 
   {
      static bool
      compare (
         Protocol const &       lhs,
         Protocol const &       rhs)
         {
            return &lhs == &rhs;
         }
   };

   sessions_.erase (std::remove_if (sessions_.begin (),
                                    sessions_.end (),
                                    boost::bind (&equals::compare,
                                                 boost::ref (session),
                                                 _1)),
                    sessions_.end ());
  
}


template <typename Protocol> inline size_t
session <Protocol>::size () const
{
   return sessions_.size ();
}

}; }; };
