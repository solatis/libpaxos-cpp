namespace paxos { namespace detail { namespace util {

inline log4cxx_helper::log4cxx_helper ()
   : logger_ (log4cxx::Logger::getLogger ("paxos.detail.util.debug"))
{
   log4cxx::BasicConfigurator::configure (
      new log4cxx::ConsoleAppender (new log4cxx::PatternLayout ("\%-5p [\%t] [\%F:\%L]: \%m\%n")));
}

inline log4cxx::LoggerPtr
log4cxx_helper::logger ()
{
   return logger_;
}


}; }; };
