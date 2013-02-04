#!/bin/bash
eval `scramv1 runtime -sh`

#inputf=root://eoscms//eos/cms
#inputf=$inputf/store/group/phys_heavyions/dileptons/Data2013/pPb/Prompt/TTrees/Jpsi_Histos_Runs_210498-210837.root

#inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-211032.root
inputf=/afs/cern.ch/work/m/miheejo/private/TREE/pA/Mass2-5GeV_Jpsi_Histos_Runs_210498-210909.root

prefixarr=(default_bit1)
#prefixarr=(default_cowboy default_sailor)
for prefix in ${prefixarr[@]}; do
  if [ ! -d $prefix ]; then
    mkdir $prefix
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
#    exit
  fi

  ./Tree2Datasets =t 5070 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent5070 &
  ./Tree2Datasets =t 60 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent60 &
  ./Tree2Datasets =t 50 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent50 &
  ./Tree2Datasets =t 70 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent70 &
  ./Tree2Datasets =t 80 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent80 &
  ./Tree2Datasets =t 805 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent805 &
  ./Tree2Datasets =t 801235 =s 1 =op -1 =ot 3 =or 0 =oc 1 =w 0 =f $inputf $prefix >& $prefix/log_cent801235 &

done


