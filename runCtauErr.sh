#!/bin/bash
eval `scramv1 runtime -sh`

#prefixarr=(default_cowboy default_sailor default_bit1 default_bit124 zVtxLT10_cowboy zVtxLT10_sailor zVtxLT10_bit1 autoCorr_cowboy autoCorr_sailor autoCorr_bit1 noFlat_cowboy noFlat_sailor noFlat_bit1)
#prefixarr=(default_bit1 default_cowboy default_sailor)
prefixarr=(default_bit1)

datasetarr=(datasets) # datasets_newctau)

for dataset in ${datasetarr[@]}; do
  for prefix in ${prefixarr[@]}; do
    rm -rf $prefix
    mkdir $prefix
    ./CTauErr $prefix $dataset >& $dataset\_$prefix\.log &
  done
done

