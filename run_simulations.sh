#!/bin/bash

# read file where the simulations to do are configured
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR
SIM_FILE="simConfig.csv"

[ -z $1 ] || SIM_FILE="$1"

IFS=","

#skip the first line with the headers
tail -n +2 $SIM_FILE | 
    while read simFile protocol simName nodeCfg packetSize numPackets sendIntervall simEndTime txPower channel interferencePacketSize interferenceSendIntervall interferenceTxPower interferenceChannel \
    proactiveForwarding reactiveForwarding seedSetEntryLifetime dataMessageIMin dataMessageIMax dataMessageK dataMessageTimerExpirations controlMessageIMin controlMessageIMax controlMessageK controlMessageTimerExpirations numHops ; do
      [ -z $simFile ] && unset FILE|| FILE="${simFile}"
      [ -z $protocol ] && unset PROTOCOL|| PROTOCOL=" --protocol=${protocol}"
      [ -z $simName ] && unset NAME || NAME=" --simName=${simName}" 
      [ -z $nodeCfg ] && unset INPUT || INPUT=" --input=${nodeCfg}" 
      [ -z $packetSize ] && unset PACKET_SIZE || PACKET_SIZE=" --packetSize=${packetSize}" 
      [ -z $numPackets ] && unset PACKET_NUM || PACKET_NUM=" --numPackets=${numPackets}"
      [ -z $sendIntervall ] && unset SEND_INTERVALL || SEND_INTERVALL=" --sendIntervall=${sendIntervall}"
      [ -z $txPower ]  && unset POWER || POWER=" --txPower=${txPower}"
      [ -z $channel ]  && unset CHANNEL || CHANNEL=" --channel=${channel}"
      [ -z $simEndTime ]  && unset TIME || TIME=" --simEndTime=${simEndTime}"
      [ -z $interferencePacketSize ]  && unset IN_PACKET_SIZE || IN_PACKET_SIZE=" --interferencePacketSize=${interferencePacketSize}"
      [ -z $interferenceSendIntervall ]  && unset IN_PACKET_INTERVAL || IN_PACKET_INTERVAL=" --interferenceSendIntervall=${interferenceSendIntervall}"
      [ -z $interferenceTxPower ]  && unset IN_POWER || IN_POWER=" --interferenceTxPower=${interferenceTxPower}"
      [ -z $interferenceChannel ]  && unset IN_CHANNEL || IN_CHANNEL=" --interferenceChannel=${interferenceChannel}"

      [ -z $proactiveForwarding ] && unset PROACTIVE || PROACTIVE=" --proactiveForwarding=${proactiveForwarding}" 
      [ -z $reactiveForwarding ] && unset REACTIVE || REACTIVE=" --reactiveForwarding=${reactiveForwarding}" 
      [ -z $seedSetEntryLifetime ] && unset LIFETIME || LIFETIME=" --seedSetEntryLifetime=${seedSetEntryLifetime}" 
      [ -z $dataMessageIMin ] && unset D_I_MIN || D_I_MIN=" --dataMessageIMin=${dataMessageIMin}"
      [ -z $dataMessageIMax ] && unset D_I_MAX || D_I_MAX=" --dataMessageIMax=${dataMessageIMax}"
      [ -z $dataMessageK ]  && unset D_K || D_K=" --dataMessageK=${dataMessageK}"
      [ -z $dataMessageTimerExpirations ]  && unset D_EX || D_EX=" --dataMessageTimerExpirations=${dataMessageTimerExpirations}"
      [ -z $controlMessageIMin ]  && unset C_I_MIN || C_I_MIN=" --controlMessageIMin=${controlMessageIMin}"
      [ -z $controlMessageIMax ]  && unset C_I_MAX || C_I_MAX=" --controlMessageIMax=${controlMessageIMax}"
      [ -z $controlMessageK ]  && unset C_K || C_K=" --controlMessageK=${controlMessageK}"
      [ -z $controlMessageTimerExpirations ]  && unset C_EX || C_EX=" --controlMessageTimerExpirations=${controlMessageTimerExpirations}"
      [ -z $numHops ]  && unset  HOPS || HOPS=" --numHops=${numHops}"

      rm -r output/${simName} #&> /dev/null
      mkdir -p output/${simName}

      CMD=--run=${FILE}${PROTOCOL}${NAME}${INPUT}${PACKET_SIZE}${PACKET_NUM}${SEND_INTERVALL}${POWER}${CHANNEL}${TIME}${IN_PACKET_SIZE}${IN_PACKET_INTERVAL}${IN_POWER}${IN_CHANNEL}${PROACTIVE}${REACTIVE}${LIFETIME}${D_I_MIN}${D_I_MAX}${D_K}${D_EX}${C_I_MIN}${C_I_MAX}${C_K}${C_EX}${HOPS}
      echo "./waf ${CMD}"
      ./waf $CMD
    done