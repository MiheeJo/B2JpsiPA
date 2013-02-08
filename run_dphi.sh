#!/bin/bash
eval `scramv1 runtime -sh`

#inputf=root://eoscms//eos/cms
#inputf=$inputf/store/group/phys_heavyions/dileptons/Data2013/pPb/Prompt/TTrees/Jpsi_Histos_Runs_210498-210837.root

inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-211256.root
#inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-210909.root

prefixarr=(default_bit1 default_bit1_mu4GeV)
#prefixarr=(default_bit1)
#prefixarr=(default_cowboy default_sailor)

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
#    exit
  fi

  if [ "$prefix" == "${prefixarr[1]}" ]; then
    echo "runType == 4"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =or 4 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  else
    echo "runType == 1"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  fi

done

