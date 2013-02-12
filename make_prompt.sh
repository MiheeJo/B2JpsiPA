#!/bin/bash
eval `scramv1 runtime -sh`

#input=/afs/cern.ch/work/m/miheejo/private/TREE/NonPromptJpsi_MCTemplate_cms538HI.root
input=/afs/cern.ch/work/m/miheejo/private/TREE/PromptJpsi_MCTemplate_cms538HI.root

#prefixarr=(nonPrompt)
prefixarr=(prompt)
#prefixarr=(default_cowboy default_sailor)
#prefixarr=(default_bit1 default_cowboy default_sailor)
for prefix in ${prefixarr[@]}; do
  if [ ! -d $prefix ]; then
    mkdir $prefix
  else
    echo " "
    echo "===== Target directory exists! Check is it okay to delete or not.";
    exit
  fi
done

for prefix in ${prefixarr[@]}; do
#  ./Tree2DatasetspPb =t 40100 =s 1 =op -1 =ot 3 =or 0 =w 0 =f $input $prefix >& $prefix/log_cent40100 &
  ./Tree2DatasetsPbp =t 40100 =s 1 =op -1 =ot 3 =or 0 =w 0 =f $input $prefix >& $prefix/log_cent40100 &
done
