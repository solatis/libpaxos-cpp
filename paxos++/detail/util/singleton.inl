namespace paxos { namespace detail { namespace util {

template <class T> 
/*! static */ T &
singleton <T>::instance ()
{
   /*! 
     Because we use function-local static initialization, this is guaranteed
     to be created before main ().
   */
   static T obj;
   return obj;
}

}; }; };
