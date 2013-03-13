#include <iostream>
#include <math.h>
#include <fstream>

#include <TROOT.h>
#include "TSystem.h"
#include <TFile.h>
#include <TGraphErrors.h>
#include <TVector3.h>
#include "TH2.h"
#include "TH3.h"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TClonesArray.h"
#include "TChain.h"

#include "RooFit.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"

#include <TCanvas.h>
#include "TStyle.h"
#include <TPaveStats.h>


//-1: etHFp+etHFm combined without auto-correlation
//-2: etHFp+etHFm combined with enhanced auto-correlation
//-3: *NOT* flattening + etHFp+etHFm combined without auto-correlation
//otherwise, indicates event plane method
static int RPNUM = -1;

//0: Nominal (one of the mu trig), 1: bit1 & Cowboy, 2: bit1 & Sailor, 3: bit1, 4: bit2, 5: random generation & bit1
//6: one of the single mu trig, 7: (one of the single mu trig) & cowboy, 8: (one of the single mu trig) & sailor
//9: (one of the single mu trig) && (one of the double mu trig)
//10: bit1,2,4 (HLT_HIL1DoubleMu0_NHitQ || HLT_HIL2DoubleMu3 || HLT_HIL3DoubleMuOpen_Mgt2_OS_NoCowboy)
static int trigType = 3;
static bool randChoice = true; //Only valid trigType == 5, true: randx > 0.5, false: randx < 0.5

//runType can be combined with trigType
//0: Don't use this option (default!!) 
//1: nMuValHits > 12, 2: select J/psi with |single mu_eta| < 1.2, 3: |zVtx| < 10 cm
//4: numOfMatch > 1 with a J/psi mass closest dimuon per event
//5: numOfMatch > 2 with a J/psi mass closest dimuon per event
//6: various single muon cuts for both single muons
//7: both single mu pT > 4
//8: -2.4 < both single mu eta < 1.46
//9: -1.47 < both single mu eta < 0.53
static int runType = 0;

//0 : DO NOT weight, 1: Apply weight
static bool doWeighting = false;

//0: don't care about RPAng, 1: Pick events with RPAng != -10
static bool checkRPNUM = false;

static const double Jpsi_MassMin=2.6;
static const double Jpsi_MassMax=3.5;
static const double Jpsi_PtMin=0.0;
static const double Jpsi_PtMax=100;
static const double Jpsi_YMin=0;
static const double Jpsi_YMax=2.4;
static const double Jpsi_CtMin = -3.0;
static const double Jpsi_CtMax = 5.0;
static const double Jpsi_CtErrMin = 0.0;
static const double Jpsi_CtErrMax = 0.5;
static const double Jpsi_PhiMin=-3.14;
static const double Jpsi_PhiMax=3.14;
static const double Jpsi_dPhiMin=-6.28;
static const double Jpsi_dPhiMax=6.28;

using namespace RooFit;
using namespace std;

static const double PDGJpsiM = 3.096916;

// Variables for a dimuon
struct Condition {
  double theMass, theRapidity, thePt, theCentrality;
  double thePhi, thedPhi, thedPhi22, thedPhi23;
  double vprob, theCt, theCtErr, zVtx, theEff;
  int HLTriggers, Reco_QQ_trig, theCat,Jq;
  int mupl_nMuValHits, mumi_nMuValHits;
  int mupl_numOfMatch, mumi_numOfMatch;
  int mupl_nTrkHits, mumi_nTrkHits;
  int mupl_nTrkWMea, mumi_nTrkWMea;
  double mupl_norChi2_inner, mumi_norChi2_inner, mupl_norChi2_global, mumi_norChi2_global;
  double theCtTrue, genType;
} ;

bool checkTriggers(const struct Condition Jpsi, bool cowboy, bool sailor, double var);
bool checkRunType(const struct Condition Jpsi, const TLorentzVector* m1P, const TLorentzVector* m2P);
bool isAccept(const TLorentzVector* aMuon);
double reducedPhi(double thedPhi);

int main(int argc, char* argv[]) {
  unsigned int centInput = 5;
  unsigned int centRegions = 5;
  unsigned int dPhiRegions = 1;

  string fileName, outputDir;
  int initev = 0;
  int nevt = -1;

  if (argc == 1) {
    cout << "====================================================================\n";
    cout << "Use the program with this commend:" << endl;
    cout << "./Tree2Datasets =t [centrality array] =s [dPhi array] =op [RP number] =ot [trigType] =or [runType] =w [Weighting] =f [input TTree file] [output directory]" << endl;
    cout << "./Tree2Datasets =t 5 =s 4 =op -1 =ot 3 =or 0 =w 0 =f /tmp/miheejo/mini_Jpsi_Histos_may202012_m25gev.root ." << endl;
    cout << "====================================================================\n";
    return 0;
  }
  
  for (int i=1; i<argc; i++) {
    char *tmpargv = argv[i];
    switch (tmpargv[0]) {
      case '=':{
        switch (tmpargv[1]) {
          case 't':
            centInput = atoi(argv[i+1]);
            cout << "centRegions: " << centInput << endl;
            break;
          case 's':
            dPhiRegions = atoi(argv[i+1]);
            cout << "dPhiRegions: " << dPhiRegions << endl;
            break;
          case 'f':
            fileName = argv[i+1];
            cout << "fileName: " << fileName << endl;
            outputDir = argv[i+2];
            cout << "output directory: " << outputDir << endl;
            break;
          case 'o':
            switch (tmpargv[2]) {
              case 'p':
                RPNUM = atoi(argv[i+1]);
                cout << "RPNUM: " << RPNUM << endl;
                break;
              case 't':
                trigType = atoi(argv[i+1]);
                cout << "trigType: "<< trigType << endl;
                break;
              case 'r':
                runType = atoi(argv[i+1]);
                cout << "runType: " << runType << endl;
                break;
              case 'c':
                if (0 == atoi(argv[i+1])) checkRPNUM = false;
                else checkRPNUM = true;
                cout << "checkRPNUM: " << checkRPNUM << endl;
                break;
            }
            break;
          case 'w':
            if (atoi(argv[i+1]) == 0) doWeighting = false;
            else doWeighting =true;
            break;
          case 'e':
            initev = atoi(argv[i+1]);
            cout << "start event #: " << initev << endl;
            nevt = atoi(argv[i+2]);
            cout << "end event #: " << nevt << endl;
            break;
        }
      }
    } // end of checking switch loop
  } // end of checking options

  // centrality array: at least 3 element is necessary for each array!
  // pA has 100 centrality bins
  int cent50Arr[] = {0,10,20,30,40,50,100};
  int cent60Arr[] = {0,10,20,30,40,50,60,100};
  int cent305070Arr[] = {0,10,30,50,70,100};
  int cent70Arr[] = {0,10,20,30,40,50,60,70,100};
  int cent80Arr[] = {0,10,20,30,40,50,60,70,80,100};
  int cent805Arr[] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,80,100};
  int cent901235Arr[] = {0,1,2,4,6,8,10,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,65,70,75,80,90,100};
  
  int cent1Arr[] = {-1,0}; //pp
  vector<int> centLimits;

  // dPhi array: at least 3 element is necessary for each array!
  float phi1Arr[] = {-1*TMath::Pi(), TMath::Pi()};
  float phi8Arr[] = {-1*TMath::Pi()*4/4, -1*TMath::Pi()*3/4, -1*TMath::Pi()*2/4, -1*TMath::Pi()*1/4, 0.0, TMath::Pi()*1/4, TMath::Pi()*2/4, TMath::Pi()*3/4, TMath::Pi()*4/4};
  float phi12Arr[] = {-1*TMath::Pi()*6/6, -1*TMath::Pi()*5/6, -1*TMath::Pi()*4/6, -1*TMath::Pi()*3/6, -1*TMath::Pi()*2/6, -1*TMath::Pi()*1/6, 0.0, TMath::Pi()*1/6, TMath::Pi()*2/6, TMath::Pi()*3/6, TMath::Pi()*4/6, TMath::Pi()*5/6, TMath::Pi()*6/6,};
  float dPhi1Arr[] = {0.0, TMath::PiOver2()};
  float dPhi2Arr[] = {0.0, TMath::PiOver2()/2, TMath::PiOver2()/2*2};
  float dPhi4Arr[] = {0.0, TMath::PiOver2()/4, TMath::PiOver2()/4*2, TMath::PiOver2()/4*3, TMath::PiOver2()/4*4};
  float dPhi6Arr[] = {0.0, TMath::PiOver2()/6, TMath::PiOver2()/6*2, TMath::PiOver2()/6*3, TMath::PiOver2()/6*4, TMath::PiOver2()/6*5, TMath::PiOver2()/6*6};
  vector<float> dPhiLimits;

  if (centInput == 50) {
    int arrSize = sizeof(cent50Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent50Arr[i]);
  } else if (centInput == 305070) {
    int arrSize = sizeof(cent305070Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent305070Arr[i]);
  } else if (centInput == 60) {
    int arrSize = sizeof(cent60Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent60Arr[i]);
  } else if (centInput == 70) {
    int arrSize = sizeof(cent70Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent70Arr[i]);
  } else if (centInput == 80) {
    int arrSize = sizeof(cent80Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent80Arr[i]);
  } else if (centInput == 805) {
    int arrSize = sizeof(cent805Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent805Arr[i]);
  } else if (centInput == 901235) {
    int arrSize = sizeof(cent901235Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent901235Arr[i]);
  } else if (centInput == 1) {
    int arrSize = sizeof(cent1Arr)/sizeof(int);
    for (int i=0; i<arrSize; i++) centLimits.push_back(cent1Arr[i]);
  }
  centRegions = centLimits.size()-1;
  cout << "\ncentRegions: " << centRegions << endl;
  for (unsigned int i=0; i<centLimits.size(); i++) {
    cout << centLimits[i] << " ";
  }

  if (checkRPNUM) { //Use J/psi dPhi
    if (dPhiRegions == 1) {
      int arrSize = sizeof(dPhi1Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi1Arr[i]);
    } else if (dPhiRegions == 2) {
      int arrSize = sizeof(dPhi2Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi2Arr[i]);
    } else if (dPhiRegions == 4) {
      int arrSize = sizeof(dPhi4Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi4Arr[i]);
    } else if (dPhiRegions == 6) {
      int arrSize = sizeof(dPhi6Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi6Arr[i]);
    }
  } else {  //Use J/psi phi
/*    if (dPhiRegions == 1) {
      int arrSize = sizeof(phi1Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(phi1Arr[i]);
    } else if (dPhiRegions == 8) {
      int arrSize = sizeof(phi8Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(phi8Arr[i]);
    } else if (dPhiRegions == 12) {
      int arrSize = sizeof(phi12Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(phi12Arr[i]);
    }*/
    if (dPhiRegions == 1) {
      int arrSize = sizeof(dPhi1Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi1Arr[i]);
    } else if (dPhiRegions == 2) {
      int arrSize = sizeof(dPhi2Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi2Arr[i]);
    } else if (dPhiRegions == 4) {
      int arrSize = sizeof(dPhi4Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi4Arr[i]);
    } else if (dPhiRegions == 6) {
      int arrSize = sizeof(dPhi6Arr)/sizeof(float);
      for (int i=0; i<arrSize; i++) dPhiLimits.push_back(dPhi6Arr[i]);
    }
  }
  cout << "\ndPhiRegions: ";
  for (unsigned int i=0; i<dPhiLimits.size(); i++) {
    cout << dPhiLimits[i] << " ";
  }
  cout << endl;

  TFile *file=TFile::Open(fileName.c_str());
  TTree *Tree=(TTree*)file->Get("myTree");

/*  // 4D efficiency files
  string dirPath = "/afs/cern.ch/user/d/dmoon/public/ForJpsiV2/4DEff/0512/";
  char effHistname[1000];
  unsigned int nCentEff = 0;
  int centEff5[] = {0, 4, 8, 12, 24, 40};  //0 10 20 30 60 100
  int centEff2060[] = {0, 4, 8, 24};
  int centEff1060[] = {0, 4, 24};
  vector<int>centEff;

  if (centInput == 2060) {
    for (int i=0; i<4; i++) centEff.push_back(centEff2060[i]);
  } else if (centInput == 1060) {
    for (int i=0; i<3; i++) centEff.push_back(centEff1060[i]);
  } else {
    for (int i=0; i<6; i++) centEff.push_back(centEff5[i]);
  }
  nCentEff = centEff.size()-1;
  cout << "centInput: " <<centInput << " nCentEff: " << nCentEff << endl;

  TFile *effFileNominal[nCentEff], *effFileNominal_LowPt[nCentEff];
  TFile *effFileCowboy[nCentEff], *effFileCowboy_LowPt[nCentEff];
  TFile *effFileSailor[nCentEff], *effFileSailor_LowPt[nCentEff];
  for (int idx=0; idx <nCentEff ; idx++) {
    if (trigType == 3) {
      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_default.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_default.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileNominal[idx] = new TFile(effHistname);

      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_LowPt_default.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_LowPt_default.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileNominal_LowPt[idx] = new TFile(effHistname);

    } else if (trigType == 1) {
      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_cowboy.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_cowboy.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileCowboy[idx] = new TFile(effHistname);

      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_LowPt_cowboy.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_LowPt_cowboy.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileCowboy_LowPt[idx] = new TFile(effHistname);

    } else if (trigType == 2) {
      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_sailor.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_sailor.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileSailor[idx] = new TFile(effHistname);

      if (centEff[idx] == 0) {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent00%.0f_LowPt_sailor.root",dirPath.c_str(),centEff[idx+1]);
      } else {
        sprintf(effHistname,"%s/PrJpsi_PtRapdPhi_Cent%.0f%.0f_LowPt_sailor.root",dirPath.c_str(),centEff[idx],centEff[idx+1]);
      }
      cout << effHistname << endl;
      effFileSailor_LowPt[idx] = new TFile(effHistname);
    }
  }

  TH3D *heffCentNom[nCentEff], *heffCentNom_LowPt[nCentEff];
  TH3D *heffCentCow[nCentEff], *heffCentCow_LowPt[nCentEff];
  TH3D *heffCentSai[nCentEff], *heffCentSai_LowPt[nCentEff];
  for (int idx=0; idx<nCentEff; idx++) {
    if (trigType == 3) {
      heffCentNom[idx] = (TH3D*)effFileNominal[idx]->Get("eff_default");
      heffCentNom_LowPt[idx] = (TH3D*)effFileNominal_LowPt[idx]->Get("eff_default");
    } else if (trigType == 1) {
      heffCentCow[idx] = (TH3D*)effFileCowboy[idx]->Get("eff_cowboy");
      heffCentCow_LowPt[idx] = (TH3D*)effFileCowboy_LowPt[idx]->Get("eff_cowboy");
    } else if (trigType == 2) {
      heffCentSai[idx] = (TH3D*)effFileSailor[idx]->Get("eff_sailor");
      heffCentSai_LowPt[idx] = (TH3D*)effFileSailor_LowPt[idx]->Get("eff_sailor");
    }
  }
*/

  if (doWeighting &&  (trigType != 3 && trigType != 1 && trigType != 2)) {
    cout << "##########################################################\n";
    cout << "You chose trigType " << trigType << endl;
    cout << "This trigType do NOT work with efficiency weighting.\n";
    cout << "Efficiency weighting will NOT be applied!\n";
    cout << "##########################################################\n";
    doWeighting = false;
  }

  UInt_t          eventNb;
  Int_t           Centrality;
  Int_t           Reco_QQ_size;
  Int_t           Reco_QQ_type[20];   //[Reco_QQ_size]
  Int_t           Reco_QQ_sign[20];   //[Reco_QQ_size]
  Int_t           Reco_QQ_trig[20];   //[Reco_QQ_size]
  Int_t           Reco_QQ_mupl_nMuValHits[20];
  Int_t           Reco_QQ_mumi_nMuValHits[20];
  Int_t           Reco_QQ_mupl_nTrkHits[20];  // track hits plus global muons
  Int_t           Reco_QQ_mumi_nTrkHits[20];  // track hits minus global muons
  Int_t           Reco_QQ_mupl_nTrkWMea[20];  // tracker layers with measurement for plus inner track muons
  Int_t           Reco_QQ_mumi_nTrkWMea[20];  // tracker layers with measurement for minus inner track muons
  Int_t           Reco_QQ_mupl_numOfMatch[20];  // number of matched segments for plus inner track muons
  Int_t           Reco_QQ_mumi_numOfMatch[20];  // number of matched segments for minus inner track muons
  Float_t         Reco_QQ_mupl_norChi2_inner[20];  // chi2/ndof for plus inner track muons
  Float_t         Reco_QQ_mumi_norChi2_inner[20];  // chi2/ndof for minus inner track muons
  Float_t         Reco_QQ_mupl_norChi2_global[20];  // chi2/ndof for plus global muons
  Float_t         Reco_QQ_mumi_norChi2_global[20];  // chi2/ndof for minus global muons
  TClonesArray    *Reco_QQ_4mom;
  TClonesArray    *Reco_QQ_mupl_4mom;
  TClonesArray    *Reco_QQ_mumi_4mom;
  Float_t         Reco_QQ_ctau[20];   //[Reco_QQ_size]
  Float_t         Reco_QQ_ctauErr[20];   //[Reco_QQ_size]
  Float_t         Reco_QQ_VtxProb[20];   //[Reco_QQ_size]
  Float_t         rpAng[38];   //[Reco_QQ_size]
  Float_t         zVtx;         //Primary vertex position
  int             HLTriggers; 
//  Int_t           Gen_QQ_size;
//  Int_t           Gen_QQ_type[20];
//  Float_t         Reco_QQ_ctauTrue[20];   //[Reco_QQ_size]

  TBranch        *b_eventNb;
  TBranch        *b_Centrality;   //!
  TBranch        *b_Reco_QQ_size;   //!
  TBranch        *b_Reco_QQ_type;   //!
  TBranch        *b_Reco_QQ_sign;   //!
  TBranch        *b_Reco_QQ_mupl_nMuValHits;   //!
  TBranch        *b_Reco_QQ_mumi_nMuValHits;   //!
  TBranch        *b_Reco_QQ_mupl_nTrkHits;
  TBranch        *b_Reco_QQ_mumi_nTrkHits;
  TBranch        *b_Reco_QQ_mupl_nTrkWMea;
  TBranch        *b_Reco_QQ_mumi_nTrkWMea;
  TBranch        *b_Reco_QQ_mupl_norChi2_inner;
  TBranch        *b_Reco_QQ_mumi_norChi2_inner;
  TBranch        *b_Reco_QQ_mupl_norChi2_global;
  TBranch        *b_Reco_QQ_mumi_norChi2_global;
  TBranch        *b_Reco_QQ_mupl_numOfMatch;   //!
  TBranch        *b_Reco_QQ_mumi_numOfMatch;   //!
  TBranch        *b_HLTriggers;   //!
  TBranch        *b_Reco_QQ_trig;   //!
  TBranch        *b_Reco_QQ_4mom;   //!
  TBranch        *b_Reco_QQ_mupl_4mom;   //!
  TBranch        *b_Reco_QQ_mumi_4mom;   //!
  TBranch        *b_Reco_QQ_ctau;   //!
  TBranch        *b_Reco_QQ_ctauErr;   //!
  TBranch        *b_Reco_QQ_VtxProb;   //!
  TBranch        *b_rpAng;   //!
  TBranch        *b_zVtx;
//  TBranch        *b_Gen_QQ_size;   //!
//  TBranch        *b_Gen_QQ_type;
//  TBranch        *b_Reco_QQ_ctauTrue;   //!

  TLorentzVector* JP= new TLorentzVector;
  TLorentzVector* m1P= new TLorentzVector;
  TLorentzVector* m2P= new TLorentzVector;


  TH1I *PassingEvent[centRegions+1][dPhiRegions+1];
  TH1D *hSig[centRegions+1][dPhiRegions+1];
  TH1D *hBkg[centRegions+1][dPhiRegions+1];
  // Normal datasets
  RooDataSet* dataJpsi[centRegions+1][dPhiRegions+1];
  RooDataSet* dataJpsiSame[centRegions+1][dPhiRegions+1];
  RooDataSet* dataPsip[centRegions+1][dPhiRegions+1];
  // Have efficiency for every events
  RooDataSet* dataJpsiW[centRegions+1][dPhiRegions+1];
  RooDataSet* dataJpsiSameW[centRegions+1][dPhiRegions+1];
  RooDataSet* dataPsipW[centRegions+1][dPhiRegions+1];
  // Efficiencies are applied to datasets as a weight
  RooDataSet* dataJpsiWeight[centRegions+1][dPhiRegions+1];
  RooDataSet* dataJpsiSameWeight[centRegions+1][dPhiRegions+1];
  RooDataSet* dataPsipWeight[centRegions+1][dPhiRegions+1];
  
  RooRealVar* Jpsi_Mass;
  RooRealVar* Psip_Mass;      
  RooRealVar* Jpsi_Pt;
  RooRealVar* Jpsi_Ct;
  RooRealVar* Jpsi_CtErr;
  RooRealVar* Jpsi_Y;
  RooRealVar* Jpsi_Phi;
  RooRealVar* Jpsi_dPhi;
  RooCategory* Jpsi_Type;
  RooCategory* Jpsi_Sign;
  RooRealVar* Jpsi_3DEff; //3D efficiency
//  RooRealVar* Jpsi_CtTrue;
//  RooCategory* MCType;

  Jpsi_Mass = new RooRealVar("Jpsi_Mass","J/#psi mass",Jpsi_MassMin,Jpsi_MassMax,"GeV/c^{2}");
  Psip_Mass = new RooRealVar("Psip_Mass","#psi' mass",3.3,Jpsi_MassMax,"GeV/c^{2}");
  Jpsi_Pt = new RooRealVar("Jpsi_Pt","J/#psi pt",Jpsi_PtMin,Jpsi_PtMax,"GeV/c");
  Jpsi_Y = new RooRealVar("Jpsi_Y","J/#psi y",-Jpsi_YMax,Jpsi_YMax);
  Jpsi_Phi = new RooRealVar("Jpsi_Phi","J/#psi phi",-Jpsi_PhiMax,Jpsi_PhiMax);
  Jpsi_dPhi = new RooRealVar("Jpsi_dPhi","J/#psi phi - rpAng",-Jpsi_dPhiMax,Jpsi_dPhiMax);
  Jpsi_Type = new RooCategory("Jpsi_Type","Category of Jpsi_");
  Jpsi_Sign = new RooCategory("Jpsi_Sign","Charge combination of Jpsi_");
  Jpsi_Ct = new RooRealVar("Jpsi_Ct","J/#psi c#tau",Jpsi_CtMin,Jpsi_CtMax,"mm");
  Jpsi_CtErr = new RooRealVar("Jpsi_CtErr","J/#psi c#tau error",Jpsi_CtErrMin,Jpsi_CtErrMax,"mm");
  Jpsi_3DEff = new RooRealVar("Jpsi_3DEff","J/#psi efficiency weight",1.,100.);
//  MCType = new RooCategory("MCType","Type of generated Jpsi_");
//  Jpsi_CtTrue = new RooRealVar("Jpsi_CtTrue","J/#psi c#tau true",Jpsi_CtMin,Jpsi_CtMax,"mm");

  Jpsi_Type->defineType("GG",0);
  Jpsi_Type->defineType("GT",1);
  Jpsi_Type->defineType("TT",2);

  Jpsi_Sign->defineType("OS",0);
  Jpsi_Sign->defineType("PP",1);
  Jpsi_Sign->defineType("MM",2);

//  MCType->defineType("PR",0);
//  MCType->defineType("NP",1);

  Reco_QQ_4mom = 0;
  Reco_QQ_mupl_4mom = 0;
  Reco_QQ_mumi_4mom = 0;

  Tree->SetBranchAddress("eventNb", &eventNb, &b_eventNb);
  Tree->SetBranchAddress("Centrality", &Centrality, &b_Centrality);
  if (RPNUM == -3) {  //Not-flatten
    Tree->SetBranchAddress("NfRpAng", rpAng, &b_rpAng);
  } else {  //Flatten reaction plane
    Tree->SetBranchAddress("rpAng", rpAng, &b_rpAng);
  }
  Tree->SetBranchAddress("Reco_QQ_size", &Reco_QQ_size, &b_Reco_QQ_size);
  Tree->SetBranchAddress("HLTriggers", &HLTriggers, &b_HLTriggers);
  Tree->SetBranchAddress("Reco_QQ_trig", Reco_QQ_trig, &b_Reco_QQ_trig);
  Tree->SetBranchAddress("Reco_QQ_type", Reco_QQ_type, &b_Reco_QQ_type);
  Tree->SetBranchAddress("Reco_QQ_sign", Reco_QQ_sign, &b_Reco_QQ_sign);
  Tree->SetBranchAddress("Reco_QQ_mupl_nMuValHits", Reco_QQ_mupl_nMuValHits, &b_Reco_QQ_mupl_nMuValHits);
  Tree->SetBranchAddress("Reco_QQ_mumi_nMuValHits", Reco_QQ_mumi_nMuValHits, &b_Reco_QQ_mumi_nMuValHits);
  Tree->SetBranchAddress("Reco_QQ_mupl_numOfMatch", Reco_QQ_mupl_numOfMatch, &b_Reco_QQ_mupl_numOfMatch);
  Tree->SetBranchAddress("Reco_QQ_mumi_numOfMatch", Reco_QQ_mumi_numOfMatch, &b_Reco_QQ_mumi_numOfMatch);
  Tree->SetBranchAddress("Reco_QQ_mupl_nTrkHits", Reco_QQ_mupl_nTrkHits, &b_Reco_QQ_mupl_nTrkHits);
  Tree->SetBranchAddress("Reco_QQ_mumi_nTrkHits", Reco_QQ_mumi_nTrkHits, &b_Reco_QQ_mumi_nTrkHits);
  Tree->SetBranchAddress("Reco_QQ_mupl_nTrkWMea", Reco_QQ_mupl_nTrkWMea, &b_Reco_QQ_mupl_nTrkWMea);
  Tree->SetBranchAddress("Reco_QQ_mumi_nTrkWMea", Reco_QQ_mumi_nTrkWMea, &b_Reco_QQ_mumi_nTrkWMea);
  Tree->SetBranchAddress("Reco_QQ_mupl_norChi2_inner", Reco_QQ_mupl_norChi2_inner, &b_Reco_QQ_mupl_norChi2_inner);
  Tree->SetBranchAddress("Reco_QQ_mumi_norChi2_inner", Reco_QQ_mumi_norChi2_inner, &b_Reco_QQ_mumi_norChi2_inner);
  Tree->SetBranchAddress("Reco_QQ_mupl_norChi2_global", Reco_QQ_mupl_norChi2_global, &b_Reco_QQ_mupl_norChi2_global);
  Tree->SetBranchAddress("Reco_QQ_mumi_norChi2_global", Reco_QQ_mumi_norChi2_global, &b_Reco_QQ_mumi_norChi2_global);
  Tree->SetBranchAddress("Reco_QQ_4mom", &Reco_QQ_4mom, &b_Reco_QQ_4mom);
  Tree->SetBranchAddress("Reco_QQ_mupl_4mom", &Reco_QQ_mupl_4mom, &b_Reco_QQ_mupl_4mom);
  Tree->SetBranchAddress("Reco_QQ_mumi_4mom", &Reco_QQ_mumi_4mom, &b_Reco_QQ_mumi_4mom);
  Tree->SetBranchAddress("Reco_QQ_ctau", Reco_QQ_ctau, &b_Reco_QQ_ctau);
  Tree->SetBranchAddress("Reco_QQ_ctauErr", Reco_QQ_ctauErr, &b_Reco_QQ_ctauErr);
  Tree->SetBranchAddress("Reco_QQ_VtxProb", Reco_QQ_VtxProb, &b_Reco_QQ_VtxProb);
  Tree->SetBranchAddress("zVtx",&zVtx,&b_zVtx);
//  Tree->SetBranchAddress("Gen_QQ_size", &Gen_QQ_size, &b_Gen_QQ_size);
//  Tree->SetBranchAddress("Gen_QQ_type", Gen_QQ_type, &b_Gen_QQ_type);
//  Tree->SetBranchAddress("Reco_QQ_ctauTrue", Reco_QQ_ctauTrue, &b_Reco_QQ_ctauTrue);

  // Without weighting
  RooArgList varlist(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlistSame(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlist2(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
  // With weighting
  RooArgList varlistW(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlistSameW(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlist2W(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
/*  RooArgList varlist(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlistSame(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*Jpsi_Ct,*Jpsi_CtErr);
  RooArgList varlist2(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*Jpsi_Ct,*Jpsi_CtErr);
*/
/*  RooArgList varlist(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*MCType,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_CtTrue);
  RooArgList varlistSame(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*MCType,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_CtTrue);
  RooArgList varlist2(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*MCType,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_CtTrue);
*/
  for (unsigned int j = 0; j <= centRegions; j++) {
    for (unsigned int k = 0; k <= dPhiRegions; k++) {
      char namefile[200] = {0};
      if (j==centRegions && k==dPhiRegions) {
        sprintf(namefile,"cent%d-%d_dPhi%.3f-%.3f",
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[0],dPhiLimits[k]);
      }
      else if (j==centRegions) {
        sprintf(namefile,"cent%d-%d_dPhi%.3f-%.3f",
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[k],dPhiLimits[k+1]);
      }
      else if (k==dPhiRegions) {
        sprintf(namefile,"cent%d-%d_dPhi%.3f-%.3f",
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[0],dPhiLimits[k]);
      }
      else {
        sprintf(namefile,"cent%d-%d_dPhi%.3f-%.3f",
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[k],dPhiLimits[k+1]);
      }
      cout << "J: " << j << " k: " << k << endl;
      PassingEvent[j][k] = new TH1I(namefile,namefile,1,1,2);
      sprintf(namefile,"%s_nsig",namefile);
      hSig[j][k] = new TH1D(namefile,namefile,25,0,Jpsi_CtErrMax);
      sprintf(namefile,"%s_nbkg",namefile);
      hBkg[j][k] = new TH1D(namefile,namefile,25,0,Jpsi_CtErrMax);
      dataJpsi[j][k] = new RooDataSet("dataJpsi","A sample",varlist);
      dataJpsiSame[j][k] = new RooDataSet("dataJpsiSame","A sample",varlistSame);
      dataPsip[j][k] = new RooDataSet("dataPsip","A sample",varlist2);
      if (doWeighting) {
        dataJpsiW[j][k] = new RooDataSet("dataJpsiW","A sample",varlistW);
        dataJpsiSameW[j][k] = new RooDataSet("dataJpsiSameW","A sample",varlistSameW);
        dataPsipW[j][k] = new RooDataSet("dataPsipW","A sample",varlist2W);
      }
    }
  }

  TH1D *JpsiPt = new TH1D("JpsiPt","JpsiPt",35,0,140);

  ifstream input("../randomNumbers");
  if (trigType == 5) {
    if (!input.is_open()) { cout << "cannot open randomNumbers. exit\n"; return -1; }
  } else {
    input.close();
  }

  if (nevt == -1) nevt = Tree->GetEntries();
  for (int ev=initev; ev<nevt; ++ev) {
    if (ev%100000==0) cout << ">>>>> EVENT " << ev << " / " << Tree->GetEntries() <<  endl;

    Tree->GetEntry(ev);
    
    float theRPAng=0, theRPAng22=0, theRPAng23=0;

    // Normal HI event plane setting
/*    if (RPNUM >= 0) theRPAng = rpAng[RPNUM];
    else theRPAng = rpAng[22];  //will not be used, put arbitary number

    theRPAng22 = rpAng[22]; //etHFp
    theRPAng23 = rpAng[23]; //etHFm*/


    //Loop over all dimuons in this event and find the most J/psi mass closest dimuon (runType == 4)
    double diffMass = 1.0;
    bool passMostJpsi = false;
    struct Condition mostJpsi; //Contains all condition variables for the most possible J/psi, used for runType == 4

    for (int i=0; i<Reco_QQ_size; ++i) {
      struct Condition Jpsi; //Contains all condition variables

      JP = (TLorentzVector*) Reco_QQ_4mom->At(i);
      m1P = (TLorentzVector*) Reco_QQ_mupl_4mom->At(i);
      m2P = (TLorentzVector*) Reco_QQ_mumi_4mom->At(i);
      Jpsi.theCentrality = Centrality;
      Jpsi.vprob = Reco_QQ_VtxProb[i];
      Jpsi.theCat = Reco_QQ_type[i];
      Jpsi.Jq = Reco_QQ_sign[i];
      Jpsi.theCt = Reco_QQ_ctau[i];
      Jpsi.theCtErr = Reco_QQ_ctauErr[i];
      Jpsi.mupl_nMuValHits = Reco_QQ_mupl_nMuValHits[i];      
      Jpsi.mumi_nMuValHits = Reco_QQ_mumi_nMuValHits[i];      
      Jpsi.mupl_numOfMatch = Reco_QQ_mupl_numOfMatch[i];
      Jpsi.mumi_numOfMatch = Reco_QQ_mumi_numOfMatch[i];
      Jpsi.mupl_nTrkHits = Reco_QQ_mupl_nTrkHits[i];
      Jpsi.mumi_nTrkHits = Reco_QQ_mumi_nTrkHits[i];
      Jpsi.mupl_nTrkWMea = Reco_QQ_mupl_nTrkWMea[i];
      Jpsi.mumi_nTrkWMea = Reco_QQ_mumi_nTrkWMea[i];
      Jpsi.mupl_norChi2_inner = Reco_QQ_mupl_norChi2_inner[i];
      Jpsi.mumi_norChi2_inner = Reco_QQ_mumi_norChi2_inner[i];
      Jpsi.mupl_norChi2_global = Reco_QQ_mupl_norChi2_global[i];
      Jpsi.mumi_norChi2_global = Reco_QQ_mumi_norChi2_global[i];
//      Jpsi.genType = Gen_QQ_type[i];
//      Jpsi.theCtTrue = Reco_QQ_ctauTrue[i];

      Jpsi.theMass =JP->M();
      Jpsi.theRapidity=JP->Rapidity();
      Jpsi.thePt=JP->Pt();
      Jpsi.thePhi = JP->Phi();

      Jpsi.HLTriggers = HLTriggers;
      Jpsi.Reco_QQ_trig = Reco_QQ_trig[i];
      Jpsi.zVtx = zVtx;

      if (checkRPNUM) {
        if (theRPAng > -9) Jpsi.thedPhi=JP->Phi()-theRPAng;
        Jpsi.thedPhi = TMath::Abs(reducedPhi(Jpsi.thedPhi));
        if (theRPAng22 > -9) Jpsi.thedPhi22=JP->Phi()-theRPAng22;
        Jpsi.thedPhi22 = TMath::Abs(reducedPhi(Jpsi.thedPhi22));
        if (theRPAng23 > -9) Jpsi.thedPhi23=JP->Phi()-theRPAng23;
        Jpsi.thedPhi23 = TMath::Abs(reducedPhi(Jpsi.thedPhi23));
   
        if (RPNUM == -1 || RPNUM == -3) {
          if (JP->Eta() < 0) Jpsi.thedPhi = Jpsi.thedPhi22;
          else Jpsi.thedPhi = Jpsi.thedPhi23;
        } else if (RPNUM == -2) {
          if (JP->Eta() < 0) Jpsi.thedPhi = Jpsi.thedPhi23;
          else Jpsi.thedPhi = Jpsi.thedPhi22;
        }

      } else {
        theRPAng = 0;
        theRPAng22 = 0;
        theRPAng23 = 0;
        Jpsi.thedPhi = JP->Phi();
        Jpsi.thedPhi22 = Jpsi.thedPhi;
        Jpsi.thedPhi23 = Jpsi.thedPhi;
      }

      // get delta Phi between 2 muons to cut out cowboys
      double dPhi2mu = m1P->Phi() - m2P->Phi();
      while (dPhi2mu > TMath::Pi()) dPhi2mu -= 2*TMath::Pi();
      while (dPhi2mu <= -TMath::Pi()) dPhi2mu += 2*TMath::Pi();

      bool cowboy = false, sailor = false;
      if ( 1*dPhi2mu > 0. ) cowboy = true;
      else sailor = true;

      // Check trigger conditions
      double var=0;
      if (trigType == 5)  input >> var; //Read a random number for trigType == 5
      bool triggerCondition = checkTriggers(Jpsi, cowboy, sailor, var);

      bool isAcceptedEP = false;
      if (checkRPNUM) { // for Jpsi v2
        if (RPNUM < 0) {  //combined etHFp+etHFm datasets
          if (RPNUM == -1 || RPNUM == -3) {
            if ((JP->Eta()<0 && theRPAng22 != -10) || (JP->Eta()>=0 && theRPAng23 != -10)) isAcceptedEP = true;
            else isAcceptedEP = false;
          } else if (RPNUM == -2) {
            if ((JP->Eta()<=0 && theRPAng23 != -10) || (JP->Eta()>0 && theRPAng22 != -10)) isAcceptedEP = true;
            else isAcceptedEP = false;
          } else {
            cout << "Wrong RPNUM!\n" << endl;
            return -1;
          }

        } else {  //indivisual event plane datasets
          if ( (theRPAng != -10) && ((RPNUM==22 && JP->Eta()<0) || (RPNUM==23 && JP->Eta()>= 0)) ) isAcceptedEP = true;   //auto-correlation removed
          else if (theRPAng != -10 && RPNUM != 22 && RPNUM !=23) isAcceptedEP = true;
          else isAcceptedEP = false;
        }

      } else {  // for Jpsi raa
        isAcceptedEP = true;
      }

      bool passRunType = checkRunType(Jpsi,m1P,m2P);
      double theEff = 0;

      if (Jpsi.theMass > Jpsi_MassMin && Jpsi.theMass < Jpsi_MassMax && 
          Jpsi.thePt > Jpsi_PtMin && Jpsi.thePt < Jpsi_PtMax && 
          Jpsi.theCt > Jpsi_CtMin && Jpsi.theCt < Jpsi_CtMax && 
          Jpsi.theCtErr > Jpsi_CtErrMin && Jpsi.theCtErr < Jpsi_CtErrMax && 
//          fabs(theRapidity) > Jpsi_YMin && fabs(theRapidity) < Jpsi_YMax &&
          passRunType &&
          triggerCondition &&
          isAcceptedEP &&
          Jpsi.vprob > 0.001
          ) {

        if (doWeighting) {
          double tmpPt = Jpsi.thePt;
          if (tmpPt >= 40.0) tmpPt = 39.9;

          // 4D efficiency
/*          int tmpCent = -1;
          for (int centidx =0; centidx < nCentEff; centidx++){
            if ((Jpsi.theCentrality >= centEff[centidx]) && (Jpsi.theCentrality < centEff[centidx+1])) {
              tmpCent = centidx;
              cout << "theCentrality: " << Jpsi.theCentrality << endl;
              cout << "tmpCent: " << tmpCent << " centidx " << centidx << endl;
            }
          }
          if (tmpCent == -1) {
            theEff = 1.0;
          } else {
            int bin;
            if (trigType == 3){  //bit 1 case
              if (tmpPt < 6.5) {
                bin = heffCentNom_LowPt[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentNom_LowPt[tmpCent]->GetBinContent(bin);
                cout << "low pt" << endl;
              } else {
                bin = heffCentNom[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentNom[tmpCent]->GetBinContent(bin);
              }
            } else if (trigType == 1 && cowboy) { //cowboy
              if (tmpPt < 6.5) {
                bin = heffCentCow_LowPt[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentCow_LowPt[tmpCent]->GetBinContent(bin);
                cout << "low pt" << endl;
              } else {
                bin = heffCentCow[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentCow[tmpCent]->GetBinContent(bin);
              }
            } else if (trigType == 2 && sailor) {  //sailor
              if (tmpPt < 6.5) {
                bin = heffCentSai_LowPt[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentSai_LowPt[tmpCent]->GetBinContent(bin);
                cout << "low pt" << endl;
              } else {
                bin = heffCentSai[tmpCent]->FindBin(Jpsi.thedPhi,tmpPt,fabs(Jpsi.theRapidity));
                theEff = heffCentSai[tmpCent]->GetBinContent(bin);
              }
            }
            cout << "thedPhi: " << Jpsi.thedPhi << " thePt: " << tmpPt << " theRapidity: " << Jpsi.theRapidity << endl;
            cout << "bin: " << bin << " theEff: " << theEff << endl;
          }
*/
        } else { theEff = 1.0; }  // end of the weighting condition
        Jpsi.theEff = 1.0/theEff;

        double tmpDiff = TMath::Abs(PDGJpsiM - Jpsi.theMass);
        if (runType == 4) {
          if (Jpsi.mupl_numOfMatch > 1 && Jpsi.mumi_numOfMatch > 1 && diffMass > tmpDiff) {
            diffMass = tmpDiff;
            mostJpsi = Jpsi;
            passMostJpsi = true;
          }
        } else if (runType == 5) {
          if (Jpsi.mupl_numOfMatch > 2 && Jpsi.mumi_numOfMatch > 2 && diffMass > tmpDiff) {
            diffMass = tmpDiff;
            mostJpsi = Jpsi;
            passMostJpsi = true;
          }
        } else {
          JpsiPt->Fill(Jpsi.thePt);
          Jpsi_Pt->setVal(Jpsi.thePt); 
          Jpsi_Y->setVal(Jpsi.theRapidity); 
          Jpsi_Phi->setVal(Jpsi.thePhi);
          Jpsi_Mass->setVal(Jpsi.theMass);
          Psip_Mass->setVal(Jpsi.theMass);
          Jpsi_Ct->setVal(Jpsi.theCt);
          Jpsi_CtErr->setVal(Jpsi.theCtErr);
          Jpsi_dPhi->setVal(Jpsi.thedPhi);
          Jpsi_3DEff->setVal(Jpsi.theEff);
          Jpsi_Type->setIndex(Jpsi.theCat,kTRUE);
          if (Jpsi.Jq == 0){ Jpsi_Sign->setIndex(Jpsi.Jq,kTRUE); }
          else { Jpsi_Sign->setIndex(Jpsi.Jq,kTRUE); }
  //        Jpsi_CtTrue->setVal(Jpsi.theCtTrue);
  //        MCType->setIndex(Jpsi.genType,kTRUE);

  //        RooArgList varlist_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*MCType,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_CtTrue);
  //        RooArgList varlist2_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*MCType,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_CtTrue);
  /*        RooArgList varlist_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*Jpsi_Ct,*Jpsi_CtErr);
          RooArgList varlist2_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Type,*Jpsi_Sign,*Jpsi_Ct,*Jpsi_CtErr);*/
          // Without weighting
          RooArgList varlist_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
          RooArgList varlistSame_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
          RooArgList varlist2_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
          // With weighting
          RooArgList varlistW_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
          RooArgList varlistSameW_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
          RooArgList varlist2W_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);


          for (unsigned int j = 0; j <= centRegions; j++) {
            if ( (j==centRegions && Jpsi.theCentrality < centLimits[j] && Jpsi.theCentrality >= centLimits[0]) ||
                (j!=centRegions && Jpsi.theCentrality < centLimits[j+1] && Jpsi.theCentrality >= centLimits[j]) ) {
              for(unsigned int k = 0; k <= dPhiRegions; k++){
                if( (k==dPhiRegions && Jpsi_dPhi->getVal() < dPhiLimits[k] && Jpsi_dPhi->getVal() >= dPhiLimits[0]) ||
                    (k!=dPhiRegions && Jpsi_dPhi->getVal() < dPhiLimits[k+1] && Jpsi_dPhi->getVal() >= dPhiLimits[k]) ) {
                  if (Jpsi.Jq == 0) {
                    if (Jpsi.theMass < 3.5) {
                      dataJpsi[j][k]->add(varlist_tmp);
                      if (doWeighting) dataJpsiW[j][k]->add(varlistW_tmp);
                      PassingEvent[j][k]->Fill(1);
                    }
                    if (Jpsi.theMass > 3.3) {
                      dataPsip[j][k]->add(varlist2_tmp);
                      if (doWeighting) dataPsipW[j][k]->add(varlist2W_tmp);
                    }
                    if (Jpsi.theMass > 2.9 && Jpsi.theMass < 3.3)
                      hSig[j][k]->Fill(Jpsi.theCtErr);
                    else if (Jpsi.theMass < 2.9 || Jpsi.theMass > 3.3)
                      hBkg[j][k]->Fill(Jpsi.theCtErr);
                  } else {
                    if (Jpsi.theMass < 3.5) {
                      dataJpsiSame[j][k]->add(varlist_tmp);
                      if (doWeighting) dataJpsiSameW[j][k]->add(varlistW_tmp);
                    }
                  }
                }
              } // End of k loop
            }
          } // End of j, k loop

        } // runType == 4 or 5 condition
      } // End of if() statement for cuts

    } // End of Reco_QQ_size loop

    // Fill up the most J/psi mass closest dimuon per event
    if ((runType == 4 || runType == 5) && passMostJpsi) {
      Jpsi_Pt->setVal(mostJpsi.thePt); 
      Jpsi_Y->setVal(mostJpsi.theRapidity); 
      Jpsi_Phi->setVal(mostJpsi.thePhi);
      Jpsi_Mass->setVal(mostJpsi.theMass);
      Psip_Mass->setVal(mostJpsi.theMass);
      Jpsi_Ct->setVal(mostJpsi.theCt);
      Jpsi_CtErr->setVal(mostJpsi.theCtErr);
      Jpsi_dPhi->setVal(mostJpsi.thedPhi);
      Jpsi_3DEff->setVal(mostJpsi.theEff);
      Jpsi_Type->setIndex(mostJpsi.theCat,kTRUE);
      if (mostJpsi.Jq == 0){ Jpsi_Sign->setIndex(mostJpsi.Jq,kTRUE); }
      else { Jpsi_Sign->setIndex(mostJpsi.Jq,kTRUE); }

      // Without weighting
      RooArgList varlist_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
      RooArgList varlistSame_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
      RooArgList varlist2_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_Ct,*Jpsi_CtErr);
      // With weighting
      RooArgList varlistW_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
      RooArgList varlistSameW_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);
      RooArgList varlist2W_tmp(*Psip_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Phi,*Jpsi_Type,*Jpsi_3DEff,*Jpsi_Ct,*Jpsi_CtErr);

      for (unsigned int j = 0; j <= centRegions; j++) {
        if ( (j==centRegions && mostJpsi.theCentrality < centLimits[j] && mostJpsi.theCentrality >= centLimits[0]) ||
            (j!=centRegions && mostJpsi.theCentrality < centLimits[j+1] && mostJpsi.theCentrality >= centLimits[j]) ) {
          for(unsigned int k = 0; k <= dPhiRegions; k++){
            if( (k==dPhiRegions && Jpsi_dPhi->getVal() < dPhiLimits[k] && Jpsi_dPhi->getVal() >= dPhiLimits[0]) ||
                (k!=dPhiRegions && Jpsi_dPhi->getVal() < dPhiLimits[k+1] && Jpsi_dPhi->getVal() >= dPhiLimits[k]) ) {
              if (mostJpsi.Jq == 0) {
                if (mostJpsi.theMass < 3.5) {
                  dataJpsi[j][k]->add(varlist_tmp);
                  if (doWeighting) dataJpsiW[j][k]->add(varlistW_tmp);
                  PassingEvent[j][k]->Fill(1);
                }
                if (mostJpsi.theMass > 3.3) {
                  dataPsip[j][k]->add(varlist2_tmp);
                  if (doWeighting) dataPsipW[j][k]->add(varlist2W_tmp);
                }
                if (mostJpsi.theMass > 2.9 && mostJpsi.theMass < 3.3)
                  hSig[j][k]->Fill(mostJpsi.theCtErr);
                else if (mostJpsi.theMass < 2.9 || mostJpsi.theMass > 3.3)
                  hBkg[j][k]->Fill(mostJpsi.theCtErr);
              } else {
                if (mostJpsi.theMass < 3.5) {
                  dataJpsiSame[j][k]->add(varlist_tmp);
                  if (doWeighting) dataJpsiSameW[j][k]->add(varlistW_tmp);
                }
              }
            }
          } // End of k loop
        }
      } // End of j, k loop
    } // End of runType == 4 or 5
    passMostJpsi = false;
  } // End of tree event loop



  char namefile[200];
  TCanvas *canv = new TCanvas("canv","canv",800,600);
  canv->cd();
  JpsiPt->Draw("text");
  sprintf(namefile,"%s/JpsiPt.pdf",outputDir.c_str());
  canv->SaveAs(namefile);

  delete canv;

  canv = new TCanvas("canv","canv",20000,15000);
  canv->Divide(dPhiRegions+1,centRegions+1);
  canv->Draw();

  /// *** Fill TFiles with RooDataSet
  TFile* Out[centRegions+1][dPhiRegions+1];
  int padnum = 1;
  for (unsigned int j = 0; j <= centRegions; j++) {
    for (unsigned int k = 0; k <= dPhiRegions; k++) {
      char tmpc_name[300];
      if (j==centRegions && k==dPhiRegions) {
        sprintf(namefile,"%s/Data2013_cent%d-%d_dPhi%.3f-%.3f.root",outputDir.c_str(),
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[0],dPhiLimits[k]);
        sprintf(tmpc_name,"%s/cent%d-%d_dPhi%.3f-%.3f.png",outputDir.c_str(),
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[0],dPhiLimits[k]);
      }
      else if (j==centRegions) {
        sprintf(namefile,"%s/Data2013_cent%d-%d_dPhi%.3f-%.3f.root",outputDir.c_str(),
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[k],dPhiLimits[k+1]);
        sprintf(tmpc_name,"%s/cent%d-%d_dPhi%.3f-%.3f.png",outputDir.c_str(),
            int(centLimits[0]),int(centLimits[j]),dPhiLimits[k],dPhiLimits[k+1]);
      }
      else if (k==dPhiRegions) {
        sprintf(namefile,"%s/Data2013_cent%d-%d_dPhi%.3f-%.3f.root",outputDir.c_str(),
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[0],dPhiLimits[k]);
        sprintf(tmpc_name,"%s/cent%d-%d_dPhi%.3f-%.3f.png",outputDir.c_str(),
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[0],dPhiLimits[k]);
      }
      else {
        sprintf(namefile,"%s/Data2013_cent%d-%d_dPhi%.3f-%.3f.root",outputDir.c_str(),
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[k],dPhiLimits[k+1]);
        sprintf(tmpc_name,"%s/cent%d-%d_dPhi%.3f-%.3f.png",outputDir.c_str(),
            int(centLimits[j]),int(centLimits[j+1]),dPhiLimits[k],dPhiLimits[k+1]);
      }

      if (doWeighting) {
        dataJpsiWeight[j][k] = new RooDataSet("dataJpsiWeight","A sample",*dataJpsiW[j][k]->get(),Import(*dataJpsiW[j][k]),WeightVar(*Jpsi_3DEff));
        dataJpsiSameWeight[j][k] = new RooDataSet("dataJpsiSameWeight","A sample",*dataJpsiSameW[j][k]->get(),Import(*dataJpsiSameW[j][k]),WeightVar(*Jpsi_3DEff));
        dataPsipWeight[j][k] = new RooDataSet("dataPsipWeight","A sample",*dataPsipW[j][k]->get(),Import(*dataPsipW[j][k]),WeightVar(*Jpsi_3DEff));
      }

//      if (0!=gSystem->Which(Form("%s/",outputDir.c_str()),namefile)) continue;  //skip duplicate cases
      Out[j][k] = new TFile(namefile,"RECREATE");
      Out[j][k]->cd();
      dataJpsi[j][k]->Write();
      dataJpsiSame[j][k]->Write();
      dataPsip[j][k]->Write();
      if (doWeighting) {
        dataJpsiW[j][k]->Write();
        dataJpsiSameW[j][k]->Write();
        dataPsipW[j][k]->Write();
        dataJpsiWeight[j][k]->Write();
        dataJpsiSameWeight[j][k]->Write();
        dataPsipWeight[j][k]->Write();
      }
      Out[j][k]->Close();

      cout << namefile << endl;
      cout << "PassingEvent[" << j << "][" << k << "]: " << PassingEvent[j][k]->GetEntries() << endl;
      
      canv->cd(padnum);
      gPad->SetLogy(1);
      gStyle->SetOptTitle(kTRUE);
      gStyle->SetTitleFontSize(0.1);
      hSig[j][k]->SetLineColor(kBlack);
      hSig[j][k]->GetXaxis()->SetLabelSize(0.075);
      hSig[j][k]->GetYaxis()->SetLabelSize(0.1);
      hSig[j][k]->SetMaximum(hSig[j][k]->GetMaximum()*5);
      hSig[j][k]->Scale(1.0/(double)hSig[j][k]->GetEntries());
      hSig[j][k]->Draw();
      hBkg[j][k]->SetFillColor(kRed);
      hBkg[j][k]->SetFillStyle(3944);
      hBkg[j][k]->Scale(1.0/(double)hBkg[j][k]->GetEntries());
      hBkg[j][k]->Draw("sames");

      gPad->Update();
      gStyle->SetOptTitle(kTRUE);
      gStyle->SetOptStat("emr");
      TPaveStats *s = (TPaveStats*)hSig[j][k]->FindObject("stats");
      s->SetX1NDC(s->GetX1NDC()-0.12);
      s->SetY2NDC(s->GetY2NDC()-0.035);
      s->SetY1NDC(s->GetY2NDC()-0.20);
      TPaveStats *sr = (TPaveStats*)hBkg[j][k]->FindObject("stats");
      sr->SetX1NDC(sr->GetX1NDC()-0.12);
      sr->SetY2NDC(s->GetY1NDC()-0.01);
      sr->SetY1NDC(sr->GetY2NDC() - (s->GetY2NDC()-s->GetY1NDC()) );
      sr->SetLineColor(kRed);
      sr->SetTextColor(kRed);
      gPad->Update();

      TText txt1;
      txt1.SetTextSize(0.08);
      txt1.DrawTextNDC(0.35,0.82,"Signal");
      TText txt2;
      txt2.SetTextSize(0.08);
      txt2.SetTextColor(kRed);
      txt2.DrawTextNDC(0.35,0.75,"Sideband");
      gPad->Update();
      padnum++;

      TCanvas tmpc("tmpc","tmpc",1000,800);
      tmpc.Draw();
      RooPlot *mframe = Jpsi_Mass->frame(Bins(45));
      if (doWeighting) {
        dataJpsiW[j][k]->plotOn(mframe,LineColor(kRed),MarkerColor(kRed),MarkerStyle(kOpenCircle),DataError(RooAbsData::SumW2));
        dataJpsiWeight[j][k]->plotOn(mframe,LineColor(kBlue),MarkerColor(kBlue),MarkerStyle(kFullTriangleUp),DataError(RooAbsData::SumW2));
      }
      dataJpsi[j][k]->plotOn(mframe,MarkerStyle(kOpenSquare),DataError(RooAbsData::SumW2));

      gStyle->SetTitleFont(42,"xyz");
      gStyle->SetTitleSize(0.048,"xyz");
      gStyle->SetPadBottomMargin(0.12);
      gStyle->SetPadRightMargin(0.035);
      gStyle->SetPadLeftMargin(0.148);
      gStyle->SetPadTopMargin(0.03);
      gStyle->SetTitleYOffset(1.6);

      mframe->SetTitle("");
      mframe->Draw();
      tmpc.SaveAs(tmpc_name);

    }
  }

  sprintf(namefile,"%s/CtauErr_centBins%d.pdf",outputDir.c_str(),centRegions);
  canv->SaveAs(namefile);

  if (trigType == 5) input.close();

  delete canv;
  delete JpsiPt;
  for (unsigned int j = 0; j <= centRegions; j++) {
    for (unsigned int k = 0; k <= dPhiRegions; k++) {
      delete PassingEvent[j][k];
      delete hSig[j][k];
      delete hBkg[j][k];
    }
  }
  return 0;

}




////////// sub-routines
double reducedPhi(double thedPhi) {
  if(thedPhi < -TMath::Pi()) thedPhi += 2.*TMath::Pi();
  if(thedPhi > TMath::Pi()) thedPhi -= 2.*TMath::Pi();
  if(thedPhi < -TMath::Pi()/2) thedPhi +=TMath::Pi();
  if(thedPhi > TMath::Pi()/2) thedPhi -=TMath::Pi();

  return thedPhi;
}

bool isAccept(const TLorentzVector* aMuon) {
  if (fabs(aMuon->Eta()) < 1.2) return true;
  else return false;
}

bool checkRunType(const struct Condition Jpsi, const TLorentzVector* m1P, const TLorentzVector* m2P) {
  if (runType == 1) {
    if (Jpsi.mupl_nMuValHits > 12 && Jpsi.mumi_nMuValHits > 12) return true;
    else return false;
  }
  else if (runType == 2) {
    if (isAccept(m1P) && isAccept(m2P)) return true;
    else return false;
  }
  else if (runType == 3) {
    if (fabs(Jpsi.zVtx) < 10.0) return true;
    else return false;
  }
  else if (runType == 6) {
    bool Matches      = Jpsi.mupl_numOfMatch > 1 && Jpsi.mumi_numOfMatch > 1;
    bool InnerChiMeas = (Jpsi.mupl_norChi2_inner/Jpsi.mupl_nTrkWMea < 0.15) &&
                        (Jpsi.mumi_norChi2_inner/Jpsi.mumi_nTrkWMea < 0.15);
    bool MuHits       = Jpsi.mupl_nMuValHits > 12 && Jpsi.mumi_nMuValHits > 12; 
    bool TrkHits      = Jpsi.mupl_nTrkHits > 12 && Jpsi.mumi_nTrkHits > 12;
    bool GlobalChi    = (Jpsi.mupl_norChi2_global/(Jpsi.mupl_nMuValHits+Jpsi.mupl_nTrkHits) < 0.15) &&
                        (Jpsi.mumi_norChi2_global/(Jpsi.mumi_nMuValHits+Jpsi.mumi_nTrkHits) < 0.15);

    if (TrkHits && Matches && InnerChiMeas && MuHits && GlobalChi) return true;
    else return false;
  }
  else if (runType == 7) {
    if (m1P->Pt() > 4.0 && m2P->Pt() > 4.0) return true;
    else return false;
  }
  // For Pbp (first run period)
  else if (runType == 8) {
    if (m1P->Eta() > -2.4 && m2P->Eta() > -2.4 && m1P->Eta() < 1.47 && m2P->Eta() < 1.47) return true;
    else return false;
  }
  else if (runType == 9) {
    if (m1P->Eta() > -1.47 && m2P->Eta() > -1.47 && m1P->Eta() < 0.53 && m2P->Eta() < 0.53) return true;
    else return false;
  }
  else if (runType == 78) {
    if ((m1P->Eta() > -2.4 && m2P->Eta() > -2.4 && m1P->Eta() < 1.47 && m2P->Eta() < 1.47) && (m1P->Pt() > 4.0 && m2P->Pt() > 4.0)) return true;
    else return false;
  }
  else if (runType == 79) {
    if ((m1P->Eta() > -1.47 && m2P->Eta() > -1.47 && m1P->Eta() < 0.53 && m2P->Eta() < 0.53) && (m1P->Pt() > 4.0 && m2P->Pt() > 4.0)) return true;
    else return false;
  }
  // For pPb (second run period)
  else if (runType == 10) {
    if (m1P->Eta() > -1.47 && m2P->Eta() > -1.47 && m1P->Eta() < 2.4 && m2P->Eta() < 2.4) return true;
    else return false;
  }
  else if (runType == 11) {
    if (m1P->Eta() > -0.53 && m2P->Eta() > -0.53 && m1P->Eta() < 1.47 && m2P->Eta() < 1.47) return true;
    else return false;
  }
  else if (runType == 710) {
    if ((m1P->Eta() > -1.47 && m2P->Eta() > -1.47 && m1P->Eta() < 2.4 && m2P->Eta() < 2.4) && (m1P->Pt() > 4.0 && m2P->Pt() > 4.0)) return true;
    else return false;
  }
  else if (runType == 711) {
    if ((m1P->Eta() > -0.53 && m2P->Eta() > -0.53 && m1P->Eta() < 1.47 && m2P->Eta() < 1.47) && (m1P->Pt() > 4.0 && m2P->Pt() > 4.0)) return true;
    else return false;
  }

  return true;
}

bool checkTriggers(const struct Condition Jpsi, bool cowboy, bool sailor, double var) {
  bool singleMu = false, doubleMu = false;
  bool triggerCondition = false;
  if ( ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 ) ||
       ( (Jpsi.HLTriggers&2)==2 && (Jpsi.Reco_QQ_trig&2)==2 ) ||
       ( (Jpsi.HLTriggers&4)==4 && (Jpsi.Reco_QQ_trig&4)==4 ) ||
       ( (Jpsi.HLTriggers&8)==8 && (Jpsi.Reco_QQ_trig&8)==8 ) ) {
    doubleMu = true;
  } else { doubleMu = false; }

  if ( ( (Jpsi.HLTriggers&16)==16 && (Jpsi.Reco_QQ_trig&16)==16 ) ||
       ( (Jpsi.HLTriggers&32)==32 && (Jpsi.Reco_QQ_trig&32)==32 ) ||
       ( (Jpsi.HLTriggers&64)==64 && (Jpsi.Reco_QQ_trig&64)==64 ) ||
       ( (Jpsi.HLTriggers&128)==128 && (Jpsi.Reco_QQ_trig&128)==128 ) ) {
    singleMu = true;
  } else { singleMu = false; }

  if (trigType == 0) { //nominal case
    if ( singleMu || doubleMu ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==1) { //HLT_HIL1DoubleMu0_HighQ, cowboy
    if ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 && cowboy) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==2) {  //HLT_HIL1DoubleMu0_HighQ, sailor
    if ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 && sailor ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==3) {  //HLT_HIL1DoubleMu0_HighQ
    if ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==4) {  //HLT_HIL2DoubleMu3
    if ( (Jpsi.HLTriggers&2)==2 && (Jpsi.Reco_QQ_trig&2)==2 ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if ( trigType ==5 ) { // HLT_HIL1DoubleMu0_HighQ && Random selection for cowboy, sailor
    if ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 ) {
      if ( (randChoice && (0.5 > var)) || (!randChoice && (0.5 < var)) ) { triggerCondition = true; }
      else { triggerCondition = false; }

    } else {
      triggerCondition = false;
    }

  } else if (trigType ==6) {  //Single muon triggers only
    if ( singleMu ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==7) {  //Single muon triggers only & cowboy
    if ( singleMu && cowboy ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==8) {  //Single muon triggers only & sailor
    if ( singleMu && sailor ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==9) {  //one of the single muon trig && one of the double muon trig
    if (singleMu && doubleMu ) { triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==10) {  // HLT_HIL1DoubleMu0_NHitQ || HLT_HIL2DoubleMu3 || HLT_HIL3DoubleMuOpen_Mgt2_OS_NoCowboy
    if ( ( (Jpsi.HLTriggers&1)==1 && (Jpsi.Reco_QQ_trig&1)==1 ) ||
         ( (Jpsi.HLTriggers&2)==2 && (Jpsi.Reco_QQ_trig&2)==2 ) ||
         ( (Jpsi.HLTriggers&8)==8 && (Jpsi.Reco_QQ_trig&8)==8 ) ) {
      triggerCondition = true; }
    else { triggerCondition = false; }

  } else if (trigType ==11) {  // HLT_HIL3DoubleMuOpen_Mgt2_OS_NoCowboy
    if ( (Jpsi.HLTriggers&8)==8 && (Jpsi.Reco_QQ_trig&8)==8 ) {
      triggerCondition = true; }
    else { triggerCondition = false; }

  } else {
    cout << "Not valid trigType!\n";
    triggerCondition = false;
  }

  return triggerCondition;
}
