/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007-2009 Strasbourg University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: David Gross <gdavid.devel@gmail.com>
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/header.h"
#include "ipv6-option-header.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("Ipv6OptionHeader");

NS_OBJECT_ENSURE_REGISTERED (Ipv6OptionHeader);

TypeId Ipv6OptionHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6OptionHeader")
    .AddConstructor<Ipv6OptionHeader> ()
    .SetParent<Header> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId Ipv6OptionHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6OptionHeader::Ipv6OptionHeader ()
  : m_type (0),
    m_length (0)
{
}

Ipv6OptionHeader::~Ipv6OptionHeader ()
{
}

void Ipv6OptionHeader::SetType (uint8_t type)
{
  m_type = type;
}

uint8_t Ipv6OptionHeader::GetType () const
{
  return m_type;
}

void Ipv6OptionHeader::SetLength (uint8_t length)
{
  m_length = length;
}

uint8_t Ipv6OptionHeader::GetLength () const 
{
  return m_length;
}

void Ipv6OptionHeader::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)m_type << " )";
}

uint32_t Ipv6OptionHeader::GetSerializedSize () const
{
  return m_length + 2;
}

void Ipv6OptionHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_type);
  i.WriteU8 (m_length);

  i.Write (m_data.Begin (), m_data.End ());
}

uint32_t Ipv6OptionHeader::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;

  m_type = i.ReadU8 ();
  m_length = i.ReadU8 ();

  m_data = Buffer ();
  m_data.AddAtEnd (m_length);
  Buffer::Iterator dataStart = i;
  i.Next (m_length);
  Buffer::Iterator dataEnd = i;
  m_data.Begin ().Write (dataStart, dataEnd);

  return GetSerializedSize ();
}

Ipv6OptionHeader::Alignment Ipv6OptionHeader::GetAlignment () const
{
  return (Alignment){ 1,0};
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6OptionPad1Header);

TypeId Ipv6OptionPad1Header::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6OptionPad1Header")
    .AddConstructor<Ipv6OptionPad1Header> ()
    .SetParent<Ipv6OptionHeader> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId Ipv6OptionPad1Header::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6OptionPad1Header::Ipv6OptionPad1Header ()
{
  SetType (0);
}

Ipv6OptionPad1Header::~Ipv6OptionPad1Header ()
{
}

void Ipv6OptionPad1Header::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)GetType () << " )";
}

uint32_t Ipv6OptionPad1Header::GetSerializedSize () const
{
  return 1;
}

void Ipv6OptionPad1Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
}

uint32_t Ipv6OptionPad1Header::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6OptionPadnHeader);

TypeId Ipv6OptionPadnHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6OptionPadnHeader")
    .AddConstructor<Ipv6OptionPadnHeader> ()
    .SetParent<Ipv6OptionHeader> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId Ipv6OptionPadnHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6OptionPadnHeader::Ipv6OptionPadnHeader (uint32_t pad)
{
  SetType (1);
  NS_ASSERT_MSG (pad >= 2, "PadN must be at least 2 bytes long");
  SetLength (pad - 2);
}

Ipv6OptionPadnHeader::~Ipv6OptionPadnHeader ()
{
}

void Ipv6OptionPadnHeader::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)GetType () << " length = " << (uint32_t)GetLength () << " )";
}

uint32_t Ipv6OptionPadnHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6OptionPadnHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());

  for (int padding = 0; padding < GetLength (); padding++)
    {
      i.WriteU8 (0);
    }
}

uint32_t Ipv6OptionPadnHeader::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6OptionJumbogramHeader);

TypeId Ipv6OptionJumbogramHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6OptionJumbogramHeader")
    .AddConstructor<Ipv6OptionJumbogramHeader> ()
    .SetParent<Ipv6OptionHeader> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId Ipv6OptionJumbogramHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6OptionJumbogramHeader::Ipv6OptionJumbogramHeader ()
{
  SetType (0xC2);
  SetLength (4);
  m_dataLength = 0;
}

Ipv6OptionJumbogramHeader::~Ipv6OptionJumbogramHeader ()
{
}

void Ipv6OptionJumbogramHeader::SetDataLength (uint32_t dataLength)
{
  m_dataLength = dataLength;
}

uint32_t Ipv6OptionJumbogramHeader::GetDataLength () const 
{
  return m_dataLength;
}

void Ipv6OptionJumbogramHeader::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)GetType () << " length = " << (uint32_t)GetLength () << " data length = " << (uint32_t)m_dataLength << " )"; 
}

uint32_t Ipv6OptionJumbogramHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6OptionJumbogramHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());
  i.WriteHtonU32 (m_dataLength);
}

uint32_t Ipv6OptionJumbogramHeader::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());
  m_dataLength = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

Ipv6OptionHeader::Alignment Ipv6OptionJumbogramHeader::GetAlignment () const
{
  return (Alignment){ 4,2}; //4n+2
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6OptionRouterAlertHeader);

TypeId Ipv6OptionRouterAlertHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6OptionRouterAlertHeader")
    .AddConstructor<Ipv6OptionRouterAlertHeader> ()
    .SetParent<Ipv6OptionHeader> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TypeId Ipv6OptionRouterAlertHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6OptionRouterAlertHeader::Ipv6OptionRouterAlertHeader ()
  : m_value (0)
{
  SetType (5);
  SetLength (2);
}

Ipv6OptionRouterAlertHeader::~Ipv6OptionRouterAlertHeader ()
{
}

void Ipv6OptionRouterAlertHeader::SetValue (uint16_t value)
{
  m_value = value;
}

uint16_t Ipv6OptionRouterAlertHeader::GetValue () const
{
  return m_value;
}

void Ipv6OptionRouterAlertHeader::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)GetType () << " length = " << (uint32_t)GetLength ()   << " value = " << (uint32_t)m_value << " )";
}

uint32_t Ipv6OptionRouterAlertHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6OptionRouterAlertHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());
  i.WriteHtonU16 (m_value);
}

uint32_t Ipv6OptionRouterAlertHeader::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());
  m_value = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

Ipv6OptionHeader::Alignment Ipv6OptionRouterAlertHeader::GetAlignment () const
{
  return (Alignment){ 2,0}; //2n+0
}

NS_OBJECT_ENSURE_REGISTERED (MplOptionHeader);

TypeId MplOptionHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MplOptionHeader")
        .SetParent<Ipv6OptionHeader> ()
        .SetGroupName ("Internet")
        .AddConstructor<MplOptionHeader>()
  ;
  return tid;
}
TypeId MplOptionHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

MplOptionHeader::MplOptionHeader() : Ipv6OptionHeader()
{
  SetParams(SEED_16_BIT_ADDRESSING, false, 0, 0);
}

void MplOptionHeader::SetParams (AddressingModes S, bool M, uint8_t sequence, uint64_t optSeedID)
{
  SetType (0x6D);
  m_seedID = optSeedID;
  m_flags = (S << 6) | (M << 5);
  m_sequence= sequence;

  switch(S) // check the first 2 bits
  {
    case IPV6_ADDRESSING:  // no seedID included
      SetLength (2);
      m_seedID = 0;
      break;
    case SEED_16_BIT_ADDRESSING:  // 16-bit seedID
      SetLength (4);
      break;
    case SEED_64_BIT_ADDRESSING:  // 64-bit seedID
      SetLength (10);
      break;
    case SEED_128_BIT_ADDRESSING:
    default:
      // 128-bit seed-ID currently not supported
      break;
  }
}


MplOptionHeader::~MplOptionHeader ()
{
}


uint8_t MplOptionHeader::GetS () const
{
  return m_flags >> 6;
}

uint8_t MplOptionHeader::GetSequence() const
{
  return m_sequence;
}

uint64_t MplOptionHeader::GetSeedID () const
{
  return m_seedID;
}

void MplOptionHeader::SetM(bool value)
{
  m_flags = value ? m_flags|(1 << 5) : m_flags&~(1<<5);
}

bool MplOptionHeader::GetM() const
{
  return m_flags&(0x20);
}

uint8_t MplOptionHeader::GetFlags() const
{
  return m_flags;
}


void MplOptionHeader::Print (std::ostream &os) const
{
  os << "( type = " << (uint32_t)GetType () << " length = " << (uint32_t)GetLength ()   << " S = " << (m_flags>>6) << " M = " << ((uint8_t)m_flags&(0x20)>>5) << " Sequence = " << m_sequence << " SeedID = " << m_seedID << " )";
}

uint32_t MplOptionHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void MplOptionHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());
  i.WriteU8 (m_flags);
  i.WriteU8 (m_sequence);

  switch((m_flags & 0xC0) >> 6) // check the first 2 bits
  {
    case 1:  // 16-bit seedID
      i.WriteHtonU16(m_seedID);
      break;
    case 2:  // 64-bit seedID
      i.WriteHtonU64(m_seedID);
      break;
    default:
      // 128-bit seed-ID currently not supported
      break;
  }
}

uint32_t MplOptionHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());
  m_flags = i.ReadU8 ();
  m_sequence = i.ReadU8();

  switch((m_flags & 0xC0) >> 6) // check the first 2 bits
  {
    case 1:  // 16-bit seedID
      m_seedID = i.ReadNtohU16();
      break;
    case 2:  // 64-bit seedID
      m_seedID = i.ReadNtohU64();
      break;
    default:
      // 128-bit seed-ID currently not supported
      break;
  }

  return GetSerializedSize ();
}

Ipv6OptionHeader::Alignment MplOptionHeader::GetAlignment () const
{
  switch((m_flags & 0xC0) >> 6) // check the first 2 bits
  {
    case 0:
      return (Alignment) { 1, 4};
      break;
    case 1:  // 16-bit seedID
      return (Alignment) { 1, 2};
      break;
    case 2:  // 64-bit seedID
      return (Alignment) {1, 0};
      break;
    default:
      // 128-bit seed-ID currently not supported
      break;
  }

  return (Alignment){ 1,0};
}

} /* namespace ns3 */

