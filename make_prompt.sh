#!/bin/bash
eval `scramv1 runtime -sh`

input=/afs/cern.ch/work/m/miheejo/private/TREE/pA/PRMC_Histos.root

prefixarr=(prompt_mu4GeV_bit1 prompt_singleMuEtaAll_bit1 prompt_singleMuEtaAllMu4GeV_bit1 prompt_singleMuEtaBarrel_bit1 prompt_default_bit1)
for prefix in ${prefixarr[@]}; do
  if [ ! -d Pbp -o ! -d pPb ]; then
    mkdir Pbp pPb
  fi
  if [ ! -d $prefix ]; then
    mkdir Pbp/$prefix pPb/$prefix
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
    exit
  fi
done

for prefix in ${prefixarr[@]}; do
  if [ "$prefix" == "${prefixarr[0]}" ]; then
    ./Tree2DatasetspPb =t 40100 =or 7 =f $input pPb/$prefix >& pPb/$prefix/log_cent40100 &
    ./Tree2DatasetsPbp =t 40100 =or 7 =f $input Pbp/$prefix >& Pbp/$prefix/log_cent40100 &
  elif [ "$prefix" == "${prefixarr[1]}" ]; then
    ./Tree2DatasetspPb =t 40100 =or 10 =f $input pPb/$prefix >& pPb/$prefix/log_cent40100 &
    ./Tree2DatasetsPbp =t 40100 =or 8 =f $input Pbp/$prefix >& Pbp/$prefix/log_cent40100 &
  elif [ "$prefix" == "${prefixarr[2]}" ]; then
    ./Tree2DatasetspPb =t 40100 =or 710 =f $input pPb/$prefix >& pPb/$prefix/log_cent40100 &
    ./Tree2DatasetsPbp =t 40100 =or 78 =f $input Pbp/$prefix >& Pbp/$prefix/log_cent40100 &
  elif [ "$prefix" == "${prefixarr[3]}" ]; then
    ./Tree2DatasetspPb =t 40100 =or 11 =f $input pPb/$prefix >& pPb/$prefix/log_cent40100 &
    ./Tree2DatasetsPbp =t 40100 =or 9 =f $input Pbp/$prefix >& Pbp/$prefix/log_cent40100 &
  elif [ "$prefix" == "${prefixarr[4]}" ]; then
    ./Tree2DatasetspPb =t 40100 =or 0 =f $input pPb/$prefix >& pPb/$prefix/log_cent40100 &
    ./Tree2DatasetsPbp =t 40100 =or 0 =f $input Pbp/$prefix >& Pbp/$prefix/log_cent40100 &
  fi

done
