#!/bin/bash
# Do you have "pdfnup" installed in your machine?
# Otherwise some of functionalities will not work.

ispp=false
thisdirectory=$(pwd)

prefixarr=$(ls *tgz | awk 'BEGIN{FS=".tgz"}; {print $1}')

for prefix in ${prefixarr[@]}; do
  ##### Un-tar files
  tar zxvf $prefix.tgz
  mv tmp/miheejo/* .
  rm -rf tmp/
 
  ##### Get result table
  if $ispp; then
    prefix=pp_$prefix
  fi

  python extract.py $prefix $prefix;

  ##### Matrix formatted mass plots
  fullpath=""
  filelist=$(ls $prefix | grep massfit.pdf)
  for file in $filelist;
  do
    fullpath=$fullpath" "$prefix"/"$file
  done
  fullpath="pdfnup --nup 5x3 --papersize {18cm,20cm} --outfile "$prefix"_massfit.pdf "$fullpath
  $fullpath

  ##### Matrix formatted ctau plots
  fullpath=""
  filelist=$(ls $prefix | grep timefit_Log.pdf)
  for file in $filelist;
  do
    fullpath=$fullpath" "$prefix"/"$file
  done
  fullpath="pdfnup --nup 5x3 --papersize {18cm,20cm} --outfile "$prefix"_timefit_Log.pdf "$fullpath
  $fullpath

  ##### Matrix formatted ctau sideband plots
  fullpath=""
  filelist=$(ls $prefix | grep timeside_Log.pdf)
  for file in $filelist;
  do
    fullpath=$fullpath" "$prefix"/"$file
  done
  fullpath="pdfnup --nup 5x3 --papersize {18cm,20cm} --outfile "$prefix"_timeside_Log.pdf "$fullpath
  $fullpath

  ##### Matrix formatted ctau sideband plots
  fullpath=""
  filelist=$(ls $prefix | grep timesideR_Log.pdf)
  for file in $filelist;
  do
    fullpath=$fullpath" "$prefix"/"$file
  done
  fullpath="pdfnup --nup 5x3 --papersize {18cm,20cm} --outfile "$prefix"_timesideR_Log.pdf "$fullpath
  $fullpath

  ##### Matrix formatted ctau sideband plots
  fullpath=""
  filelist=$(ls $prefix | grep timesideL_Log.pdf)
  for file in $filelist;
  do
    fullpath=$fullpath" "$prefix"/"$file
  done
  fullpath="pdfnup --nup 5x3 --papersize {18cm,20cm} --outfile "$prefix"_timesideL_Log.pdf "$fullpath
  $fullpath

  mv fit_parameters fit_table fit_cppnumbers fit_errorbins $prefix\_timefit_Log.pdf $prefix\_timeside_Log.pdf $prefix\_timesideR_Log.pdf $prefix\_timesideL_Log.pdf $prefix\_massfit_wob.pdf $prefix\_massfit.pdf $prefix/summary;

#  cd $prefix;
#  for file in `ls *massfit.pdf *timefit_Log.pdf *timeside_Log.pdf`; do
#    if [ -f $file ]; then
#      echo $file
#      newfile=$(echo $file | awk 'BEGIN{FS=".pdf"}; {print $1".png"}')
#      convert -gravity center -define pdf:use-cropbox=true $file $newfile
#    fi
#  done

#  mkdir PNGFigs
#  mv *png PNGFigs
#  cd $thisdirectory

done


