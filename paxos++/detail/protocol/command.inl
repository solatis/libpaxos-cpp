namespace paxos { namespace detail { namespace protocol {


inline command::command ()
   : type_ (invalid)
{
}


inline void
command::set_type (
   enum type    type)
{
   type_ = type;
}

inline enum command::type
command::type () const
{
   return type_;
}

inline void
command::set_host_id (
   boost::uuids::uuid const &        id)
{
   host_id_ = id;
}

inline boost::uuids::uuid const &
command::host_id () const
{
   return host_id_;
}

template <class Archive>
inline void
command::serialize (
   Archive &                 ar,
   unsigned int const        version)
{
   ar & type_;
   ar & host_id_;
}


}; }; };
