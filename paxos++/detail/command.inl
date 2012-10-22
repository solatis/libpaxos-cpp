namespace paxos { namespace detail { 


inline command::command ()
   : type_ (type_invalid),
     error_code_ (no_error),
     proposal_id_ (0)
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
command::set_error_code (
   enum detail::error_code      error_code)
{
   error_code_ = error_code;
}

inline enum detail::error_code
command::error_code () const
{
   return error_code_;
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
   ar & type_;
   ar & error_code_;

   ar & host_id_;
   ar & host_address_;
   ar & host_port_;

   ar & proposal_id_;
   ar & workload_;
}


}; };
