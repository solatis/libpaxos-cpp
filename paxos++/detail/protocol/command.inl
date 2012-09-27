namespace paxos { namespace detail { namespace protocol {


inline command::command ()
   : type_ (type_invalid)
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


inline void
command::set_host_state (
   enum remote_server::state state)
{
   host_state_ = state;
}

inline enum remote_server::state
command::host_state () const
{
   return host_state_;
}



template <class Archive>
inline void
command::serialize (
   Archive &                 ar,
   unsigned int const        version) 
{
   ar & type_;

   ar & host_id_;
   ar & host_address_;
   ar & host_port_;
   ar & host_state_;
}


}; }; };
