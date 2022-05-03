#!/bin/bash

# read file where the simulations to do are configured
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR
SIM_FILE="simConfigs/simConfig.csv"
COMAPRE_DIAGS="simConfigs/compareDiagsConfig.csv"

[ -z $1 ] || SIM_FILE="$1"
[ -z $2 ] || COMAPRE_DIAGS="$2"

IFS=","
#skip the first line with the headers
tail -n +2 $SIM_FILE | 
    while read simFile simRuns protocol simName nodeCfg mobility seedValue packetSize numPackets sendIntervall simEndTime txPower channel interferencePacketSize interferenceSendIntervall interferenceTxPower interferenceChannel \
    proactiveForwarding reactiveForwarding seedSetEntryLifetime dataMessageIMin dataMessageIMax dataMessageK dataMessageTimerExpirations controlMessageIMin controlMessageIMax controlMessageK controlMessageTimerExpirations numHops energyModelEnabled initialNodeEnergy; do
      [ -z $simFile ] && unset FILE || FILE="${simFile}"
      [ -z $simRuns ] && unset SIM_RUNS || SIM_RUNS=" --simRuns=${simRuns}"
      [ -z $protocol ] && unset PROTOCOL || PROTOCOL=" --protocol=${protocol}"
      [ -z $simName ] && unset NAME || NAME=" --simName=${simName}" 
      [ -z $nodeCfg ] && unset INPUT || INPUT=" --input=${nodeCfg}"
      [ -z $mobility ] && unset MOBILITY || MOBILITY=" --mobilityMode=${mobility}"
      [ -z $seedValue ] && unset RNG_INIT || RNG_INIT=" --rngInit=${seedValue}" 
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

      [ -z $energyModelEnabled ]  && unset ENABLE_ENERGY_MODEL || ENABLE_ENERGY_MODEL=" --energyModelEnabled=${energyModelEnabled}"
      [ -z $initialNodeEnergy ]  && unset  INITIAL_ENERGY || INITIAL_ENERGY=" --initialNodeEnergy=${initialNodeEnergy}"

      
      rm -r output/${simName} &> /dev/null
      mkdir -p output/${simName}
      CMD=--run=${FILE}${SIM_RUNS}${PROTOCOL}${NAME}${INPUT}${MOBILITY}${RNG_INIT}${PACKET_SIZE}${PACKET_NUM}${SEND_INTERVALL}${POWER}${CHANNEL}${TIME}${IN_PACKET_SIZE}${IN_PACKET_INTERVAL}${IN_POWER}${IN_CHANNEL}${PROACTIVE}${REACTIVE}${LIFETIME}${D_I_MIN}${D_I_MAX}${D_K}${D_EX}${C_I_MIN}${C_I_MAX}${C_K}${C_EX}${HOPS}${ENABLE_ENERGY_MODEL}${INITIAL_ENERGY}
      echo "./waf ${CMD}"
       ./waf $CMD

      # iterate over the subdirectories of the current simulation to unify the result-files and return various result types (mean, median, std)
      for directory in output/${simName}/*; do
          if [ -d "${directory}" ]; then
            python3 ./contrib/statistics/model/combineCsvFiles.py -f ${directory}
            python3 ./contrib/statistics/model/printDiagsCsvIntern.py -i ${directory}
          fi
      done
    done

rm -r output/DifferentCsvDiagrams &> /dev/null
mkdir -p output/DifferentCsvDiagrams

tail -n +2 $COMAPRE_DIAGS |
    while read outname diagType diagValue title xDescription yDescription simName1 dataFile1 legend1 simName2 dataFile2 legend2 simName3 dataFile3 legend3 simName4 dataFile4 legend4 simName5 dataFile5 legend5 simName6 dataFile6 legend6 ; do
      [ -z $outname ] && unset OUTNAME || OUTNAME=" -n $SCRIPT_DIR/output/DifferentCsvDiagrams/${outname}.png"
      [ -z $diagType ] && unset DIAG_TYPE || DIAG_TYPE=" -t ${diagType}"
      [ -z $diagValue ] && unset DIAG_VALUE || DIAG_VALUE=" -v ${diagValue}"

      [ -z $dataFile1 ] && unset DATA_FILE1 || DATA_FILE1=" -a $SCRIPT_DIR/output/${simName1}/${dataFile1}"
      [ -z $dataFile2 ] && unset DATA_FILE2 || DATA_FILE2=" -b $SCRIPT_DIR/output/${simName2}/${dataFile2}"
      [ -z $dataFile3 ] && unset DATA_FILE3 || DATA_FILE3=" -c $SCRIPT_DIR/output/${simName3}/${dataFile3}"
      [ -z $dataFile4 ] && unset DATA_FILE4 || DATA_FILE4=" -d $SCRIPT_DIR/output/${simName4}/${dataFile4}"
      [ -z $dataFile5 ] && unset DATA_FILE5 || DATA_FILE5=" -e $SCRIPT_DIR/output/${simName5}/${dataFile5}"
      [ -z $dataFile6 ] && unset DATA_FILE6 || DATA_FILE6=" -f $SCRIPT_DIR/output/${simName6}/${dataFile6}"

      [ -z $legend1 ] && unset LEGEND1 || LEGEND1=" -g '${legend1}'"
      [ -z $legend2 ] && unset LEGEND2 || LEGEND2=" -h '${legend2}'"
      [ -z $legend3 ] && unset LEGEND3 || LEGEND3=" -i '${legend3}'"
      [ -z $legend4 ] && unset LEGEND4 || LEGEND4=" -j '${legend4}'"
      [ -z $legend5 ] && unset LEGEND5 || LEGEND5=" -k '${legend5}'"
      [ -z $legend6 ] && unset LEGEND6 || LEGEND6=" -l '${legend6}'"

      [ -z $xDescription ] && unset X_DES || X_DES=" -x '${xDescription}'"
      [ -z $yDescription ] && unset Y_DES || Y_DES=" -y '${yDescription}'"
      [ -z $title ] && unset TITLE || TITLE=" -o '${title}'"

      CMD1="python3 ./contrib/statistics/model/printDiagsCsvDifferent.py ${OUTNAME}${DIAG_TYPE}${DIAG_VALUE}${TITLE}${X_DES}${Y_DES}${DATA_FILE1}${LEGEND1}${DATA_FILE2}${LEGEND2}${DATA_FILE3}${LEGEND3}${DATA_FILE4}${LEGEND4}${DATA_FILE5}${LEGEND5}${DATA_FILE6}${LEGEND6}"
      echo "${CMD1}"
      eval "${CMD1}"
    done
