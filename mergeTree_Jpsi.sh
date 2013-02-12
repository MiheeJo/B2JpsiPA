#!/bin/bash
eval `scramv1 runtime -sh`

prefixarr=(default_bit1_singleMuEtaAll default_bit1_singleMuEtaAllMu4GeV default_bit1_mu4GeV default_bit1_singleMuEtaBarrel)
labelarr=(305070 50 60 70 80 805 901235)

for prefix in ${prefixarr[@]}; do
  for label in ${labelarr[@]}; do
    mv $prefix/cent$label/* $prefix/
    rmdir $prefix/cent$label 
  done
done

