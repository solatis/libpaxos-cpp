namespace paxos { namespace detail { namespace util {

////////////////////////////// PUBLIC ////////////////////////////////////////
   
//============================ LIFECYCLE =================================

//============================ OPERATORS =================================

//============================ OPERATIONS ================================   


template <typename T> /* static */ inline std::string  
conversion::to_byte_array (
   T input) 
{
   typedef union 
   {
      T t;
      unsigned char c[sizeof(T)];
   } helper_t;

   helper_t helper;
   helper.t = input;

   std::string bytes;
   bytes.reserve (sizeof (T));
      
   /*!
     Codeblock below ensures the correct byte ordering. For example, for an
     int32_t we want the following unrolled loop:

     bytes += helper.c[3]
     bytes += helper.c[2]
     bytes += helper.c[1]
     bytes += helper.c[0]
     
     So, we will start with sizeof(T) - 1 and add to bytes until we reach 0.
    */
   
   for (int16_t i = sizeof (T) - 1; i >= 0; --i)
   {
      bytes += static_cast <char> (helper.c[i]);
   }

   PAXOS_ASSERT (bytes.size () == sizeof (T));

   return bytes;
}

template <typename T> /*! static */ inline T
conversion::from_byte_array (
   std::string const &  input) 
{
   PAXOS_ASSERT (input.size () == sizeof (T));

   typedef union 
   {
      T t;
      unsigned char c[sizeof(T)];
   } helper_t;

   helper_t helper;

   /*!
     Same trick as above, but now vice versa: we want the unrolled loop to look like this:

     *ptr++ = input.at (3);
     *ptr++ = input.at (2)
     *ptr++ = input.at (1)
     *ptr++ = input.at (0)

    */
   unsigned char * ptr  = helper.c;

   for (int16_t i = sizeof (T) - 1; i >= 0; --i)
   {
      *ptr++ = input.at (i);
   }

   PAXOS_ASSERT (ptr - helper.c == sizeof (T));

   return helper.t;
}


//============================ ACCESS ====================================
   
//============================ INQUIRY ===================================
   
////////////////////////////// PROTECTED /////////////////////////////////////

////////////////////////////// PRIVATE ///////////////////////////////////////
   
}; }; };

