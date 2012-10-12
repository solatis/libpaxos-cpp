#include "configuration.hpp"

namespace paxos {

uint32_t configuration::heartbeat_interval     = 3000;
uint32_t configuration::timeout                = 3000;
float    configuration::quorum_majority_factor = 0.5;

};
