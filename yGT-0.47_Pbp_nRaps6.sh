#!/bin/bash
if [ $# -ne 3 ]; then
  echo "Usage: $0 [Executable] [Input directory] [Prefix]"
  exit;
fi

executable=$(pwd)/$1
datasets=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/$2
prefix=$3

################################################################ 
########## Script parameter setting
################################################################ 
# Analysis home directory
home=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/JpsiRpA/
# Storage for batch jobs (should be under /afs)
storage=$(pwd)/Results/$prefix
if [ ! -d $storage ]; then
  mkdir $storage
fi
if [ ! -d $(pwd)/Scripts ]; then
  mkdir $(pwd)/Scripts
fi

ctaurange=2.0-3.0
fracfree=0
ispbpb=2
is2Widths=1
isPEE=1

# Non-prompt MC
mc1=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/datasets_mc_538hi/Pbp/nonPrompt/Data2013_cent0-100_dPhi0.000-1.571.root
# Prompt MC
mc2=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/datasets_mc_538hi/Pbp/prompt/Data2013_cent0-100_dPhi0.000-1.571.root

mSigF="sigCB2WNG1" # Mass signal function name (options: sigCB2WNG1 (default), signalCB3WN)
mBkgF="expFunct" # Mass background function name (options: expFunct (default), polFunct)

weight=0  #0: Do NOT weight, 1: Do weight
eventplane="etHFm" # Name of eventplane (etHFp, etHFm, etHF(default))
runOpt=4 # Inclusive mass fit (options: 4(default), 3(Constrained fit), 5(_mb in 2010 analysis))
anaBct=1 #0: do b-fit(not-analytic fit for b-lifetime), 1: do b-fit(analytic fit for b-lifetime), 2: do NOT b-fit
#0: 2 Resolution functions & fit on data, 1: 1 Resolution function & fit on data,
#2: 2 Resolution functions & fit on PRMC, 3: 1 Resolution function & fit on PRMC
resOpt=2
ctauBkg=0 #0: 1 ctau bkg, 1: 2 ctau bkg with signal region fitting, 2: 2 ctau bkg with step function

rapbins=(-0.47-1.46 -0.47-0.173 0.173-0.817 0.817-1.46 1.46-2.4)
dphibins=(0.000-1.571) #0.000-0.393 0.393-0.785 0.785-1.178 1.178-1.571)
cent0024=(0-5 5-10 10-15 15-20 20-25 25-30 30-35 35-40 40-45 45-50 50-55 55-60 60-65 65-70 70-100) #0-1 1-2 2-4 4-6 6-8 8-10 10-12 12-15 15-18 18-21 21-24 24-27 27-30 30-33 33-36 36-39 39-42 42-45 45-48 48-51 51-54 54-57 57-60 60-65 65-70 70-75 75-80 80-90 90-100)
centcorser=(0-10 10-20 20-30 30-40 40-50 50-60 60-70 70-80 70-100 60-100 50-100 50-70 80-100 10-30 30-50)
centcorser2=(0-10 10-30 30-50 50-100)
#pt0024=(6.5-30.0 6.5-7.5 7.5-8.5 8.5-9.5 9.5-10.5 10.5-11.5 11.5-13.0 13.0-18.0 18.0-30.0)
pt0024=(0.0-3.0 3.0-6.5)
ptcorser=(0.0-6.5 6.5-10.0 10.0-30.0 6.5-8.0 8.0-10.0 10.0-13.0 13.0-30.0)
ptcorser2=(0.0-6.5 6.5-10.0 10.0-30.0)


################################################################ 
########## Information print out
################################################################ 
# Choose ctau error file
if [ "$ispbpb" == "1" ]; then
  ctauErrF=$(echo $prefix | awk 'BEGIN{FS="_"}; {print $2}')
  if [ "$ctauErrF" == "bit1" -o "$ctauErrF" == "cowboy" -o "$ctauErrF" == "sailor" ]; then
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/ctauErrCheck/ctauErrRange_default_$ctauErrF\_oldctau.txt;
  else
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/ctauErrCheck/ctauErrRange_default_bit1_oldctau.txt;
  fi
elif [ "$ispbpb" == "2" ]; then
  ctauErrF=$(echo $prefix | awk 'BEGIN{FS="_"}; {print $2}')
  if [ "$ctauErrF" == "singleMuEtaAllMu4GeV" -o "$ctauErrF" == "singleMuEtaBarrel" -o "$ctauErrF" == "mu4GeV" ]; then
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/ctauErrCheck/ctauErrRange_default_bit1_$ctauErrF.txt;
  else
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/pA/ctauErrCheck/ctauErrRange_default_bit1_singleMuEtaAll.txt;
  fi
else
  ctauErrF=$(echo $prefix | awk 'BEGIN{FS="_"}; {print $2}')
  if [ "$ctauErrF" == "bit1" -o "$ctauErrF" == "cowboy" -o "$ctauErrF" == "sailor" ]; then
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/ctauErrCheck_pp/ctauErrRange_default_$ctauErrF\_oldctau.txt;
  else
    ctauErrFile=/afs/cern.ch/work/m/miheejo/private/cms442_Jpsi/src/ctauErrCheck_pp/ctauErrRange_default_bit1_oldctau.txt;
  fi
fi
echo "#### ctauErrFile: " $ctauErrFile

txtrst=$(tput sgr0)
txtred=$(tput setaf 2)  #1 Red, 2 Green, 3 Yellow, 4 Blue, 5 Purple, 6 Cyan, 7 White
txtbld=$(tput bold)     #dim (Half-bright mode), bold (Bold characters)

echo "Run fits with ${txtbld}${txtred}$executable${txtrst} on ${txtbld}${txtred}$datasets${txtrst}."
if [ "$storage" != "" ]; then
  echo "Results will be placed on ${txtbld}${txtred}$storage${txtrst}."
fi

################################################################ 
########## Function for progream running
################################################################ 
function program {
  ### Arguments
  rap=$1
  pt=$2
  shift; shift;
  centarr=(${@})

  for cent in ${centarr[@]}; do
    for dphi in ${dphibins[@]}; do
      work=$prefix"_rap"$rap"_pT"$pt"_cent"$cent"_dPhi"$dphi; # Output file name has this prefix
      workMB=$prefix"_rap"$rap"_pT"$pt"_cent0-100_dPhi0.000-1.571"; 
      workPHI=$prefix"_rap"$rap"_pT"$pt"_cent"$cent"_dPhi0.000-1.571"; 
      file=$datasets"/Data2013_cent"$cent"_dPhi"$dphi".root"; # Input datasets have this name structure
      fileMB=$datasets"/Data2013_cent0-100_dPhi0.000-1.571.root";
      filePHI=$datasets"/Data2013_cent"$cent"_dPhi0.000-1.571.root";

      echo "Processing: "$work
      printf "#!/bin/csh\n" > $(pwd)/Script/$work.csh
#      printf "source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.csh; source /afs/cern.ch/user/m/miheejo/thisroot.csh\n" >> $work.csh
      printf "cd %s\n" $(pwd) >> $(pwd)/Script/$work.csh
      printf "eval \`scramv1 runtime -csh\`\n" >> $(pwd)/Script/$work.csh
      printf "cd -\n" >> $(pwd)/Script/$work.csh
      printf "cp %s/%s %s/fit2DData_pbpb.cpp .\n" $(pwd) Script/$work.csh $(pwd) >> $(pwd)/Script/$work.csh

      ctauerrMB=$(awk -v rap=$rap -v pt=$pt  '{if ($1==rap && $2==pt && $3=="0-100" && $4=="0.000-1.571") {print $5}  }' < $ctauErrFile)
      ctauerrPhi=$(awk -v rap=$rap -v pt=$pt -v cent=$cent -v dphi=0.000-1.571 '{if ($1==rap && $2==pt && $3==cent && $4==dphi) {print $5}  }' < $ctauErrFile)
      ctauerr=$(awk -v rap=$rap -v pt=$pt -v cent=$cent -v dphi=$dphi '{if ($1==rap && $2==pt && $3==cent && $4==dphi) {print $5}  }' < $ctauErrFile)

      if [ "$ctauerrMB" == "" -o "$ctauerrPhi" == "" -o "$ctauerr" == "" ]; then
        echo "This bin doesn't have pre-defined ctau error range. Set ctau error range to 0.01-0.16"
        ctauerrMB=0.01-0.16
        ctauerrPhi=0.01-0.16
        ctauerr=0.01-0.16
      fi

      if [ "$cent" == "0-100" ]; then
        if [ "$dphi" == "0.000-1.571" ]; then
          script="$executable -f $file $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a $anaBct $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s $dphi -l $ctaurange -e $ctauerr -x $runOpt -z $fracfree >& $work.log;"
          echo $script >> $(pwd)/Script/$work.csh
        elif [ "$dphi" != "0.000-1.571" ]; then
          script="$executable -f $filePHI $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a 2 $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s 0.000-1.571 -l $ctaurange -e $ctauerrPhi -x $runOpt -z $fracfree >& $workPHI.log;"
          echo $script >> $(pwd)/Script/$work.csh
          script="$executable -f $file $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a $anaBct $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s $dphi -l $ctaurange -e $ctauerr -x $runOpt -z $fracfree >& $work.log;"
          echo $script >> $(pwd)/Script/$work.csh
        fi
      elif [ "$cent" != "0-100" ]; then
        if [ "$dphi" == "0.000-1.571" ]; then
          script="$executable -f $fileMB $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a 2 $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t 0-100 -s 0.000-1.571 -l $ctaurange -e $ctauerrMB -x $runOpt -z $fracfree >& $workMB.log;"
          echo $script >> $(pwd)/Script/$work.csh
          script="$executable -f $file $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a $anaBct $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s $dphi -l $ctaurange -e $ctauerr -x $runOpt -z $fracfree >& $work.log;"
          echo $script >> $(pwd)/Script/$work.csh
        elif [ "$dphi" != "0.000-1.571" ]; then
          script="$executable -f $fileMB $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a 2 $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t 0-100 -s 0.000-1.571 -l $ctaurange -e $ctauerrMB -x $runOpt -z $fracfree >& $workMB.log;"
          echo $script >> $(pwd)/Script/$work.csh
          script="$executable -f $filePHI $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a 2 $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s 0.000-1.571 -l $ctaurange -e $ctauerrPhi -x $runOpt -z $fracfree >& $workPHI.log;"
          echo $script >> $(pwd)/Script/$work.csh
          script="$executable -f $file $weight -m $mc1 $mc2 -v $mSigF $mBkgF -d $prefix -r $eventplane -u $resOpt -a $anaBct $ctauBkg -b $ispbpb $isPEE $is2Widths -p $pt -y $rap -t $cent -s $dphi -l $ctaurange -e $ctauerr -x $runOpt -z $fracfree >& $work.log;"
          echo $script >> $(pwd)/Script/$work.csh
        fi
      fi

      printf "tar zcvf %s.tgz %s* fit2DData_pbpb.cpp\n" $work $work >> $(pwd)/Script/$work.csh
      printf "cp %s.tgz %s\n" $work $storage >> $(pwd)/Script/$work.csh
      bsub -R "pool>10000" -u mihee.jo@cer.c -q 1nd -J $work < $(pwd)/Script/$work.csh
    done
  done
}

################################################################ 
########## Running script with pre-defined binnings
################################################################ 
for rap in ${rapbins[@]}; do
  if [ "$rap" == "-2.4-1.46" -o "$rap" == "-2.4--0.47" -o "$rap" == "-0.47-1.46" ]; then
    # pt dependence
    for pt in ${pt0024[@]}; do
      program $rap $pt 0-100
      program $rap $pt ${centcorser2[@]}
    done
    program $rap 6.5-30.0 0-100
    # fine cent dependence
#    program $rap 6.5-30.0 ${cent0024[@]}
    program $rap 6.5-30.0 ${centcorser[@]}
    # corser pt & corser cent dependence
    for pt in ${ptcorser[@]}; do
      program $rap $pt ${centcorser[@]}
      program $rap $pt 0-100
    done

  else
    program $rap 6.5-30.0 0-100
    program $rap 6.5-30.0 ${centcorser[@]}
    for pt in ${ptcorser2[@]}; do
#      program $rap $pt ${centcorser[@]}
      program $rap $pt 0-100
    done


  fi
done
