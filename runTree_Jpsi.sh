#!/bin/bash
eval `scramv1 runtime -sh`

#inputf=root://eoscms//eos/cms
#inputf=$inputf/store/group/phys_heavyions/dileptons/Data2013/pPb/Prompt/TTrees/Jpsi_Histos_Runs_210498-210837.root

inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_211371-211631.root
#inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-211256.root

prefixarr=(default_bit1 mu4GeV_bit1 singleMuEtaAll_bit1 singleMuEtaAllMu4GeV_bit1 singleMuEtaBarrel_bit1)

labelarr=(305070 50 60 70 80 805 901235)

for prefix in ${prefixarr[@]}; do
  if [ ! -d $prefix ]; then
    mkdir $prefix
    for label in ${labelarr[@]}; do
      mkdir $prefix/cent$label
    done
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
    exit
  fi

  if [ "$prefix" == "${prefixarr[0]}" ]; then
    echo "runType == 7"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =or 7 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  elif [ "$prefix" == "${prefixarr[1]}" ]; then
    echo "runType == 8"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =or 8 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  elif [ "$prefix" == "${prefixarr[2]}" ]; then
    echo "runType == 78"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =or 78 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  elif [ "$prefix" == "${prefixarr[3]}" ]; then
    echo "runType == 9"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =or 9 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  fi

done
