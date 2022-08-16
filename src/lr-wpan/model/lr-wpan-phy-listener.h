/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef LRWPAN_PHY_LISTENER_H
#define LRWPAN_PHY_LISTENER_H

namespace ns3 {

class Time;

/**
 * \brief receive notifications about PHY events.
 */
class LrWpanPhyListener
{
public:
  /****************************************************************
  *       This destructor is needed.
  ****************************************************************/

  virtual ~LrWpanPhyListener ()
  {
  }

  virtual void NotifyRxStart (Time duration) = 0;
  virtual void NotifyRxEndOk (void) = 0;
  virtual void NotifyRxEndError (void) = 0;

  virtual void NotifyTxStart (Time duration, double txPowerDbm) = 0;

  virtual void NotifyTxOffRxOff (void) = 0;
  virtual void NotifyTxOffRxOffByForce (void) = 0;
  virtual void NotifyRxOn (void) = 0;
  virtual void NotifyTxOn (void) = 0;
};

} //namespace ns3

#endif /* LRWPAN_PHY_LISTENER_H */
