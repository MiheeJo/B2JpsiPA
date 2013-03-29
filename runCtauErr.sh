#!/bin/bash
eval `scramv1 runtime -sh`

datasetarr=(/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/datasets/Pbp18.38nb/)
prefixarr=(mu4GeV_bit1 singleMuEtaAll_bit1 singleMuEtaAllMu4GeV_bit1 singleMuEtaBarrel_bit1 default_bit1)

# Pb+p (18.38/nb)
raparr=(-2.4--1.47 -1.47--0.47 -0.47-0.53 0.53-1.47 1.47-2.4 -2.4--0.47 -0.47-1.47 -1.47-0.53)
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

