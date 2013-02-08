#!/bin/bash
# Submit batch jobs
# This must be used with MkRunBat.sh

cd _pwd_
eval `scramv1 runtime -sh`
cd -
mkdir _prefix_
_pwd_/_input_py_ _prefix_ _input_file_ _rap_
tar zcvf _prefixtar_ *
cp _prefixtar_ _pwd_
