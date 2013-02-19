#!/bin/bash
eval `scramv1 runtime -sh`

#prefixarr=(default_bit1 default_cowboy default_sailor)
#prefixarr=(default_bit1_mu4GeV)

#datasetarr=(datasets)
datasetarr=(/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/datasets/Pbp18.38nb/)
prefixarr=(default_bit1 default_bit1_singleMuEtaAll default_bit1_singleMuEtaAllMu4GeV default_bit1_singleMuEtaBarrel default_bit1_mu4GeV)

#raparr=(-2.4-1.46 -2.4--0.47 -0.47-1.46 -2.4--1.757 -1.757--1.113 -1.113--0.47 -0.47-0.173 0.173-0.817 0.817-1.46 1.46-1.93 1.93-2.4 1.46-2.4 -1.47-1.46 -1.47-0.53)
# Pb+p (18.38/nb)
#raparr=(-2.4--1.47 -1.47--0.47 -0.47-0.53 0.53-1.47 1.47-2.4 -2.4--0.47 -0.47-1.47 -1.47-0.53)
raparr=(-1.47-0.53)
# p+Pb (12.55/nb)
#raparr=(-2.4--1.47 -1.47--0.53 -0.53-0.47 0.47-1.47 1.47-2.4 -1.47-0.47 0.47-2.4 -0.53-1.47)

for dataset in ${datasetarr[@]}; do
  for prefix in ${prefixarr[@]}; do
    for rap in ${raparr[@]}; do
      awk -v p=$(pwd) -v p2=CTauErr -v p3=$prefix -v p4=$dataset -v p5=$rap -v p6=$prefix\_$rap.tgz '{gsub("_pwd_",p); gsub("_input_py_",p2); gsub("_prefix_",p3); gsub("_input_file_",p4); gsub("_rap_",p5); gsub("_prefixtar_",p6);  print;}' runCtauErrJob.sh > $prefix\_$rap.sh;
      echo $prefix\_$rap.sh 
      chmod +x $prefix\_$rap.sh 
      bsub -R "pool>10000" -u miheejo@cern.c -q 1nd -J $prefix\_$rap.sh < $prefix\_$rap.sh
    done
  done
done

