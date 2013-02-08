#!/bin/bash
eval `scramv1 runtime -sh`

#prefixarr=(default_bit1 default_cowboy default_sailor)
#prefixarr=(default_bit1_mu4GeV)

#datasetarr=(datasets)
datasetarr=(/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/datasets)
prefixarr=(default_bit1 default_bit1_mu4GeV)

raparr=(-2.4-1.46 -2.4--0.47 -0.47-1.46 -2.4--2.014 -2.014--1.628 -1.628--1.242 -1.242--0.856 -0.856--0.47 -0.47--0.084 -0.084-0.302 0.302-0.688 0.688-1.074 1.074-1.46 1.46-1.93 1.93-2.4 1.46-2.4 -1.47-1.46 -1.47-0.53)

for dataset in ${datasetarr[@]}; do
  for prefix in ${prefixarr[@]}; do
    for rap in ${raparr[@]}; do
      awk -v p=$(pwd) -v p2=CTauErr -v p3=$prefix -v p4=$dataset -v p5=$rap -v p6=$prefix\_$rap.tgz '{gsub("_pwd_",p); gsub("_input_py_",p2); gsub("_prefix_",p3); gsub("_input_file_",p4); gsub("_rap_",p5); gsub("_prefixtar_",p6);  print;}' mjob.sh > $prefix\_$rap.sh;
      echo $prefix\_$rap.sh 
      chmod +x $prefix\_$rap.sh 
      bsub -R "pool>10000" -q 1nd -J $prefix\_$rap.sh < $prefix\_$rap.sh
    done
  done
done

