namespace paxos { namespace detail { 


inline command::command ()
   : type_ (type_invalid),
     error_code_ (no_error),
     next_proposal_id_ (-1),
     highest_proposal_id_ (-1),
     lowest_proposal_id_ (-1)
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
command::set_next_proposal_id (
   int64_t     proposal_id)
{
   next_proposal_id_ = proposal_id;
}

inline int64_t
command::next_proposal_id () const
{
   return next_proposal_id_;
}

inline void
command::set_highest_proposal_id (
   int64_t     proposal_id)
{
   highest_proposal_id_ = proposal_id;
}

inline int64_t
command::highest_proposal_id () const
{
   return highest_proposal_id_;
}

inline void
command::set_lowest_proposal_id (
   int64_t     proposal_id)
{
   lowest_proposal_id_ = proposal_id;
}

inline int64_t
command::lowest_proposal_id () const
{
   return lowest_proposal_id_;
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

inline void
command::set_proposed_workload (
   std::map <int64_t, std::string> const &      proposed_workload)
{
   proposed_workload_ = proposed_workload;
}


inline  std::map <int64_t, std::string> const &
command::proposed_workload () const
{
   return proposed_workload_;
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

   ar & next_proposal_id_;
   ar & highest_proposal_id_;
   ar & lowest_proposal_id_;

   ar & workload_;
   ar & proposed_workload_;
}


}; };
