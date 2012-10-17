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
   PAXOS_ASSERT (queue.request_being_processed () == false);
   queue_.request_being_processed () = true;
}


template <typename Type>
inline queue <Type>::guard::~guard ()
{
   PAXOS_ASSERT (queue_.queue_.empty () == false);
   PAXOS_ASSERT (queue_.request_being_processed () == true);

   queue_.queue_.pop ();
   queue_.request_being_processed () = false;

   if (queue_.queue_.empty () == false)
   {
      /*!
        This means we still have requests waiting in line
       */
      Type & request = queue_.queue_.front ();

      queue_.callback_ (request,
                        guard::create (queue_));
   }
}

template <typename Type>
inline queue <Type>::queue (
   callback  callback)
   : callback_ (callback),
     request_being_processed_ (false)
{
}

template <typename Type>
inline bool &
queue <Type>::request_being_processed ()
{
   return request_being_processed_;
}

template <typename Type>
inline void
queue <Type>::push (
   Type &&      request)
{
   queue_.push (request);

   if (request_being_processed () == false)
   {
      callback_ (request,
                 guard::create (*this));
   }
}


}; }; };
