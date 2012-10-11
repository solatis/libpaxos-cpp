namespace paxos { namespace detail { 


inline command::command ()
   : id_ (0),
     response_id_ (0),
     type_ (type_invalid),
     proposal_id_ (0)
{
}



inline void
command::set_id (
   uint64_t  id)
{
   id_ = id;
}

inline uint64_t
command::id () const
{
   return id_;
}



inline void
command::set_response_id (
   uint64_t  response_id)
{
   response_id_ = response_id;
}

inline uint64_t
command::response_id () const
{
   return response_id_;
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
command::set_error_code (
   enum paxos::error_code       error_code)
{
   error_code_ = error_code;
}

inline enum paxos::error_code
command::error_code () const
{
   return error_code_;
}

inline void
command::set_host_state (
   enum quorum::server::state state)
{
   host_state_ = state;
}

inline enum quorum::server::state
command::host_state () const
{
   return host_state_;
}



inline void
command::set_proposal_id (
   uint64_t     proposal_id)
{
   proposal_id_ = proposal_id;
}

inline uint64_t
command::proposal_id () const
{
   return proposal_id_;
}

inline void
command::set_workload (
   std::string const &       byte_array)
{
   workload_ = byte_array;
}

inline std::string const &
command::workload () const
{
   return workload_;
}



template <class Archive>
inline void
command::serialize (
   Archive &                 ar,
   unsigned int const        version) 
{
   ar & id_;
   ar & response_id_;

   ar & type_;
   ar & error_code_;

   ar & host_id_;
   ar & host_address_;
   ar & host_port_;
   ar & host_state_;

   ar & live_servers_;

   ar & proposal_id_;
   ar & workload_;
}


}; };
