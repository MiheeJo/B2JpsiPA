#!/bin/bash

#./Pbp_nRaps4.sh Fit2DDataPbp datasets/Pbp18.38nb/default_bit1                 Pbp_default_bit1 >& log_Pbp_default_bit1 &
./Pbp_nRaps4_etaAll.sh Fit2DDataPbp datasets/Pbp18.38nb/singleMuEtaAll_bit1 Pbp_singleMuEtaAll_bit1 >& log_Pbp_singleMuEtaAll_bit1 &

#./pPb_nRaps4.sh Fit2DDatapPb datasets/pPb12.55nb/default_bit1                 pPb_default_bit1 >& log_pPb_default_bit1 &
./pPb_nRaps4_etaAll.sh Fit2DDatapPb datasets/pPb12.55nb/singleMuEtaAll_bit1 pPb_singleMuEtaAll_bit1 >& log_pPb_singleMuEtaAll_bit1 &


