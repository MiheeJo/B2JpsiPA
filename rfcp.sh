#!/bin/bash -f

########## Directory where the job submittion performed
submitdir=$(pwd)
########## Castor directory that contains results
indir_="$submitdir/Result/"
########## Directory where python & root scripts are located
workdir="/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/JpsiRaa/"
########## Prefix of jobs
prefixarr=(yGT-0.47_singleMuEtaAll yLT-0.47_singleMuEtaAll yGT-0.47_singleMuEtaBarrel yLT-0.47_singleMuEtaBarrel yGT-0.47_mu4GeV yLT-0.47_mu4GeV)

############################################################
eval `scramv1 runtime -csh`

############################################################
########## Copy files from castor and extract it. Run python & root scripts over all files
for prefix in ${prefixarr[@]}; do
  mkdir /tmp/miheejo/$prefix
  cd /tmp/miheejo/$prefix
#  mkdir $indir/$prefix
#  cd $indir/$prefix

  indir=$indir_/$prefix
  echo "indir: "$indir
  list=$(ls $indir | grep tgz)

  for file in $list; do   # Get files from castor using prefix
    if echo $file | grep -q $prefix; then 
      echo $file
      cp $indir/$file .
      tar zxvf ./$file
      rm -f ./$file
#      rm -rf summary
    fi
  done

  # Run python & root script for 1 prefix
  python2.6 $workdir/extract.py $prefix .
#  root -l $workdir/savehisto.cpp

  # Summarize results
  mkdir /tmp/miheejo/$prefix/summary
#  mv fit_cppnumbers fit_parameters fit_table saved_histo.root $indir/$prefix/summary
  mv fit_cppnumbers fit_parameters fit_table /tmp/miheejo/$prefix/summary

  ls $submitdir | grep $prefix | awk 'BEGIN{FS=".csh"}; {print $1}' > $submitdir/$prefix\_submit
  ls /tmp/miheejo/$prefix | grep txt | awk 'BEGIN{FS=".txt"}; {print $1}' > $submitdir/$prefix\_txt
  diff $submitdir/$prefix\_submit $submitdir/$prefix\_txt > $submitdir/diff_$prefix

  tar zcvf $indir_/$prefix.tgz /tmp/miheejo/$prefix
done
