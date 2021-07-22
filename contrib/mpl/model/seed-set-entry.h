#ifndef SEED_SET_ENTRY_H
#define SEED_SET_ENTRY_H

#include "ns3/watchdog.h"
#include "ns3/ipv6-address.h"

namespace ns3 {
namespace mpl {

class SeedSetEntry
{
public:
  SeedSetEntry();
  ~SeedSetEntry();

  uint8_t GetMinSequenceNr();
  void SetMinSequenceNr(uint8_t minSeqNr);

  uint64_t GetSeedId();
  void SetSeedId(uint64_t seedId);

  Ipv6Address GetDomain();
  void SetDomain(Ipv6Address domain);

  Watchdog* GetWatchdog();

private:

  Watchdog m_watchdog;
  uint8_t m_minSequenceNr;
  Ipv6Address m_domain;
  uint64_t m_seedId;

};

}
}


#endif
