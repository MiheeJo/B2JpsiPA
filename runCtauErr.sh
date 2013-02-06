#!/bin/bash
eval `scramv1 runtime -sh`

#prefixarr=(default_bit1 default_cowboy default_sailor)
#prefixarr=(default_bit1_mu4GeV)

datasetarr=(datasets)
prefixarr=(default_bit1 default_bit1_mu4GeV)

raparr=(-2.4--0.47 -0.47-1.46 -2.4--2.014 -2.014--1.628 -1.628--1.242 -1.242--0.856 -0.856--0.47 -0.47--0.084 -0.084-0.302 0.302-0.688 0.688-1.074 1.074-1.46 1.46-1.93 1.93-2.4 1.46-2.4 -1.47-1.46 -1.47-0.53)

for dataset in ${datasetarr[@]}; do
  for prefix in ${prefixarr[@]}; do
    rm -rf $prefix
    mkdir $prefix
    for rap in ${raparr[@]}; do
      ./CTauErr $prefix $dataset $rap >& $prefix\_$rap\.log &
    done
  done
done

