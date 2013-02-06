#!/bin/bash
eval `scramv1 runtime -sh`

prefixarr=(default_bit1_psi2s)
labelarr=(50 20 3050)

for prefix in ${prefixarr[@]}; do
  for label in ${labelarr[@]}; do
    mv $prefix/cent$label/* $prefix/
    rmdir $prefix/cent$label 
  done
done

