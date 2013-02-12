#!/bin/bash
eval `scramv1 runtime -sh`

#prefixarr=(default_bit1 default_bit1_mu4GeV)
prefixarr=(default_bit1_singleMuEtaAllMu4GeV default_bit1_singleMuEtaAll default_bit1_singleMuEtaBarrel default_bit1_mu4GeV)
#raparr=(-2.4-1.46 -2.4--0.47 -0.47-1.46 -2.4--2.014 -2.014--1.628 -1.628--1.242 -1.242--0.856 -0.856--0.47 -0.47--0.084 -0.084-0.302 0.302-0.688 0.688-1.074 1.074-1.46 1.46-1.93 1.93-2.4 1.46-2.4 -1.47-1.46 -1.47-0.53 -2.4--1.757 -1.757--1.113 -1.113--0.47 -0.47-0.173 0.173-0.817 0.817-1.46)
raparr=(-2.4-1.46 -2.4--0.47 -0.47-1.46 -1.47-1.46 -1.47-0.53 -2.4--1.757 -1.757--1.113 -1.113--0.47 -0.47-0.173 0.173-0.817 0.817-1.46 1.46-2.4)

for prefix in ${prefixarr[@]}; do
  rm -f "ctauErrRange_"$prefix".txt"
  for rap in ${raparr[@]}; do
    tar zxf $prefix\_$rap.tgz
  done
  for rap in ${raparr[@]}; do
    cat "ctauErrRange_"$prefix\_$rap".txt" >> "ctauErrRange_"$prefix".txt"
  done
done

rm -f *out
