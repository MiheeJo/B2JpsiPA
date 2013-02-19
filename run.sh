#!/bin/bash

#./Pbp_nRaps4.sh Fit2DDataPbp datasets/Pbp18.38nb/default_bit1                 Pbp_default_bit1 >& log_Pbp_default_bit1 &
#./Pbp_mu4GeV.sh Fit2DDataPbp datasets/Pbp18.38nb/default_bit1_mu4GeV          Pbp_default_mu4GeV >& log_Pbp_default_mu4GeV &
./Pbp_nRaps4_etaAll.sh Fit2DDataPbp datasets/Pbp18.38nb/default_bit1_singleMuEtaAll  Pbp_default_singleMuEtaAll >& log_Pbp_default_singleMuEtaAll &

#./pPb_nRaps4.sh Fit2DDatapPb datasets/pPb12.55nb/default_bit1                 pPb_default_bit1 >& log_pPb_default_bit1 &
#./pPb_mu4GeV.sh Fit2DDatapPb datasets/pPb12.55nb/default_bit1_mu4GeV          pPb_default_mu4GeV >& log_pPb_default_mu4GeV &
./pPb_nRaps4_etaAll.sh Fit2DDatapPb datasets/pPb12.55nb/default_bit1_singleMuEtaAll  pPb_default_singleMuEtaAll >& log_pPb_default_singleMuEtaAll &


