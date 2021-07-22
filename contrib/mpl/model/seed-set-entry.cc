#include "seed-set-entry.h"

namespace ns3{
namespace mpl {

SeedSetEntry::SeedSetEntry()
{
  m_minSequenceNr = 0;
  m_seedId = 0;
  m_domain = Ipv6Address::GetAny();
}

SeedSetEntry::~SeedSetEntry()
{
}

uint8_t SeedSetEntry::GetMinSequenceNr()
{
  return m_minSequenceNr;
}
void SeedSetEntry::SetMinSequenceNr(uint8_t minSeqNr)
{
  m_minSequenceNr = minSeqNr;
}

uint64_t SeedSetEntry::GetSeedId()
{
  return m_seedId;
}

void SeedSetEntry::SetSeedId(uint64_t seedId)
{
  m_seedId = seedId;
}

Ipv6Address SeedSetEntry::GetDomain()
{
  return m_domain;
}

void SeedSetEntry::SetDomain(Ipv6Address domain)
{
  m_domain = domain;
}

Watchdog* SeedSetEntry::GetWatchdog()
{
  return &m_watchdog;
}

}
}

