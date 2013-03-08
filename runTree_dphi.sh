#!/bin/bash
eval `scramv1 runtime -sh`

#inputf=root://eoscms//eos/cms
#inputf=$inputf/store/group/phys_heavyions/dileptons/Data2013/pPb/Prompt/TTrees/Jpsi_Histos_Runs_210498-210837.root

inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_211371-211460.root
#inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-211256.root

prefixarr=(default_bit1 default_cowboy default_sailor)

labelarr=(3060)

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
    echo "trigType == 0"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =s 4 =or 0 =ot 3 =oc 1 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  elif [ "$prefix" == "${prefixarr[1]}" ]; then
    echo "trigType == 2"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =s 4 =or 0 =ot 1 =oc 1 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  elif [ "$prefix" == "${prefixarr[2]}" ]; then
    echo "trigType == 3"
    for label in ${labelarr[@]}; do
      ./Tree2Datasets =t $label =s 4 =or 0 =ot 2 =oc 1 =f $inputf $prefix/cent$label >& $prefix/log_cent$label &
    done
  fi

done

