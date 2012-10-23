namespace paxos { namespace detail { namespace request_queue {


template <typename Type>
/*! static */ typename queue <Type>::guard::pointer
queue <Type>::guard::create (
   queue <Type> &       queue)
{
   return pointer (new guard (queue));
}

template <typename Type>
inline queue <Type>::guard::guard (
   queue <Type> &       queue)
   : queue_ (queue)
{
}


template <typename Type>
inline queue <Type>::guard::~guard ()
{
   queue_.pop ();
}

template <typename Type>
inline queue <Type>::queue (
   callback  callback)
   : callback_ (callback),
     request_being_processed_ (false)
{
}


template <typename Type>
inline void
queue <Type>::push (
   Type &&      input)
{
   boost::optional <Type const &> request;

   {
      PAXOS_DEBUG ("push acquiring lock");
      boost::mutex::scoped_lock lock (mutex_);
      request = push_locked (std::forward <Type &&> (input));
      PAXOS_DEBUG ("push releasing lock");
   }

   if (request)
   {
      callback_ (*request,
                 guard::create (*this));
   }
}


template <typename Type>
inline boost::optional <Type const &>
queue <Type>::push_locked (
   Type &&      request)
{
   queue_.push (request);

   if (request_being_processed_ == false)
   {
      this->start_request_locked (request);
      return request;
   }

   return boost::none;
}



template <typename Type>
inline void
queue <Type>::pop ()
{
   boost::optional <Type const &> request;
   {
      PAXOS_DEBUG ("pop acquiring lock");
      boost::mutex::scoped_lock lock (mutex_);
      request = pop_locked ();
      PAXOS_DEBUG ("pop releasing lock");
   }

   if (request)
   {
      callback_ (*request,
                 guard::create (*this));
   }
}

template <typename Type>
inline boost::optional <Type const &>
queue <Type>::pop_locked ()
{
   PAXOS_ASSERT (queue_.empty () == false);
   PAXOS_ASSERT (request_being_processed_ == true);

   queue_.pop ();
   request_being_processed_ = false;

   if (queue_.empty () == false)
   {
      /*!
        This means we still have requests waiting in line
      */
      Type & request = queue_.front ();

      this->start_request_locked (request);

      return request;
   }

   return boost::none;
}
template <typename Type>
inline void
queue <Type>::start_request_locked (
   Type &       request)
{
   PAXOS_ASSERT (request_being_processed_ == false);

   request_being_processed_ = true;     
}

}; }; };
