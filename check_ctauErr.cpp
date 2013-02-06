#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>

#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2F.h>
#include <TMath.h>
#include <TLatex.h>
#include <TCanvas.h>

#include <RooFit.h>
#include <RooGlobalFunc.h>
#include <RooCategory.h>
#include <RooWorkspace.h>
#include <RooBinning.h>
#include <RooHistPdf.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooAbsData.h>
#include <RooHist.h>
#include <RooPlot.h>

using namespace RooFit;
using namespace std;

void getOptRange(string &ran,float *min,float *max);
RooBinning setCtBinning(float lmin,float lmax);
RooDataHist* subtractSidebands(RooWorkspace* ws, RooDataHist* subtrData, RooDataHist* all, RooDataHist* side, float scalefactor, string varName);

int main (int argc, char* argv[]) {

  gROOT->Macro("/afs/cern.ch/user/m/miheejo/public/JpsiV2/JpsiStyle.C");

//  string rapbins[] =  {"-2.4--0.47","-0.47-1.46","-2.4--2.014","-2.014--1.628","-1.628--1.242","-1.242--0.856","-0.856--0.47","-0.47--0.084","-0.084-0.302","0.302-0.688","0.688-1.074","1.074-1.46", "1.46-1.93","1.93-2.4", "1.46-2.4","-1.47-1.46","-1.47-0.53"};
  vector<string> rapbins;
  for (int i=3; i<argc; i++) rapbins.push_back(argv[i]);
  if (argc > 4) {
    cout << "Put 1 rapidity region only. exit." << endl;
    return -2;
  }
  string ptbins[] = {"0.0-6.5", "6.5-30.0", "10.0-30.0", "6.5-10.0", "3.0-6.5", "6.5-8.0", "8.0-10.0", "10.0-13.0", "13.0-30.0", "6.5-7.5", "7.5-8.5", "8.5-9.5", "9.5-10.5", "10.5-11.5", "11.5-13.0", "13.0-18.0", "18.0-30.0"};
  string ptbinscorser[] = {"0.0-6.5", "6.5-30.0", "10.0-30.0", "6.5-10.0", "3.0-6.5", "6.5-8.0", "8.0-10.0", "10.0-13.0", "13.0-30.0"};
  string ptbinsfiner[] = {"6.5-7.5", "7.5-8.5", "8.5-9.5", "9.5-10.5", "10.5-11.5", "11.5-13.0", "13.0-18.0", "18.0-30.0"};
  vector<string> ptVectorFiner(ptbinsfiner, ptbinsfiner+sizeof(ptbinsfiner)/sizeof(string));
  vector<string> ptVectorCorser(ptbinscorser, ptbinscorser+sizeof(ptbinscorser)/sizeof(string));
  string centbins[] = {"0-100", "0-5", "5-10", "10-15", "15-20", "20-25", "25-30", "30-35", "35-40", "40-45", "45-50", "50-55", "55-60", "60-65", "65-70", "70-100", "0-10", "10-20", "20-30", "30-40", "40-50", "50-100", "50-60", "60-70", "60-100", "70-100", "0-1","1-2","2-4","4-6","6-8","8-10","10-12","12-15","15-18","18-21","21-24","24-27","27-30","30-33","33-36","36-39","39-42","42-45","45-48","48-51","51-54","54-57","57-60","60-65","65-70","70-75","75-80","80-100","70-80","50-70"};
  string centbinscorser[] = {"0-100", "0-5", "5-10", "10-15", "15-20", "20-25", "25-30", "30-35", "35-40", "40-45", "45-50", "50-55", "55-60", "60-65", "65-70", "70-100", "0-10", "10-20", "20-30", "30-40", "40-50", "50-100", "50-60", "60-70", "60-100", "70-100","70-80","80-100","50-70"};
  string centbinsfiner[] = {"0-1","1-2","2-4","4-6","6-8","8-10","10-12","12-15","15-18","18-21","21-24","24-27","27-30","30-33","33-36","36-39","39-42","42-45","45-48","48-51","51-54","54-57","57-60","60-65","65-70","70-75","75-80"};
  vector<string> centVectorFiner(centbinsfiner, centbinsfiner+sizeof(centbinsfiner)/sizeof(string));
  vector<string> centVectorCorser(centbinscorser, centbinscorser+sizeof(centbinscorser)/sizeof(string));
  string dphibins[] = {"0.000-1.571"};

  string dirPre = argv[1];
  string outputFile = "ctauErrRange_" +dirPre+ "_" +rapbins[0]+ ".txt";
  ofstream output(outputFile.c_str());
  if (!output.good()) {cout << "Fail to open result file." << endl; return 1;}

//  const int nRaps = sizeof(rapbins)/sizeof(string);
  const int nRaps = rapbins.size();
  const int nPt = sizeof(ptbins)/sizeof(string);
  const int nCent = sizeof(centbins)/sizeof(string);
  const int ndPhi = sizeof(dphibins)/sizeof(string);
  cout << "nCent: " << nCent << endl;

  // prange, yrange: loop over these variables!
  for (int rapbin=0; rapbin < nRaps; rapbin++) {
    for (int ptbin=0; ptbin < nPt; ptbin++) {
      for (int centbin=0; centbin < nCent; centbin++) {
        for (int dphibin=0; dphibin < ndPhi; dphibin++) {

      string prange, lrange, yrange, crange, phirange, errrange;
      prange = ptbins[ptbin];
      yrange = rapbins[rapbin];
      crange = centbins[centbin];
      phirange = dphibins[dphibin];

      float pmin=0, pmax=0, ymin=0, ymax=0, cmin=0, cmax=0, psmax=0, psmin=0;
      getOptRange(prange,&pmin,&pmax);
      getOptRange(crange,&cmin,&cmax);
      getOptRange(yrange,&ymin,&ymax);
      getOptRange(phirange,&psmin,&psmax);

      // skip finer cent binnings, pT binnings for finer rapidity binnigs
      if ( (yrange.compare("-2.4--0.47") && yrange.compare("-0.47-1.46") && yrange.compare("-2.4-1.46") ) &&
            (find(centVectorFiner.begin(), centVectorFiner.end(), crange)!=centVectorFiner.end() || 
             find(ptVectorFiner.begin(), ptVectorFiner.end(), prange)!=ptVectorFiner.end())
         ) continue;
      // skip finer cent binnings if pT range is NOT [6.5, 30.0]
      if ( prange.compare("6.5-30.0") && (find(centVectorFiner.begin(), centVectorFiner.end(), crange)!=centVectorFiner.end()) ) continue;
      // skip finer cent binnings + finer pT binnings in the integrated rapidity bins
      if ( !yrange.compare("-2.4--0.47") && !yrange.compare("-0.47-1.46") && !yrange.compare("-2.4-1.46") && 
           find(centVectorFiner.begin(), centVectorFiner.end(), crange)!=centVectorFiner.end() &&
           find(ptVectorFiner.begin(), ptVectorFiner.end(), prange)!=ptVectorFiner.end() 
         ) continue;

      // *** Read Data files
      string dataset = argv[2];
      string FileName = "../" +dataset+ "/" +dirPre+ "/Data2013_cent" +crange+ "_dPhi" +phirange+ ".root";
      TFile fInData(FileName.c_str());
      cout << FileName.c_str() << endl;
      if (fInData.IsZombie()) { cout << "### CANNOT open data root file\n"; continue; }
      fInData.cd();
      RooDataSet *data = (RooDataSet*)fInData.Get("dataJpsi");
      data->SetName("data");

      // Create workspace to play with
      RooWorkspace *ws = new RooWorkspace("workspace");

      // Reduce "dataMC" with given ranges/cuts
      char reduceDS[300];
      char funct[300];
      sprintf(reduceDS,"Jpsi_Pt>%.2f && Jpsi_Pt<%.2f && Jpsi_Y>%.2f && Jpsi_Y<%.2f",pmin,pmax,ymin,ymax);
    //  sprintf(reduceDS,"Jpsi_CtErr > %.2f && Jpsi_CtErr < %.2f",errmin,errmax);
      cout << "reduceDS for data: " << reduceDS << endl;

      RooDataSet *redData = (RooDataSet*)data->reduce(reduceDS);
      ws->import(*redData);

      string titlestr;

      ws->var("Jpsi_CtErr")->setBins(62); //bin width == 0.008
//      ws->var("Jpsi_CtErr")->setBins(32); //bin width == 0.00625
      ws->var("Jpsi_CtErr")->setRange(0.0,0.496);

      // Additional cuts on data and get sub-datasets/histograms
//      RooDataSet *redDataCut = (RooDataSet*)redData->reduce("Jpsi_Type == Jpsi_Type::GG");
      RooDataSet *redDataCut = (RooDataSet*)redData->Clone();
      RooDataHist *binData = new RooDataHist("binData","binData",RooArgSet( *(ws->var("Jpsi_Mass")),*(ws->var("Jpsi_Ct")),*(ws->var("Jpsi_CtErr")) ), *redDataCut);
      RooDataHist *binDataCtErr = new RooDataHist("binDataCtErr","binDataCtErr",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataCut);
      cout << "DATA :: N events to fit: " << binData->sumEntries() << endl;

      // SYSTEMATICS 1 (very sidebands)
      RooDataSet *redDataSB = (RooDataSet*) redDataCut->reduce("Jpsi_Mass < 2.9 || Jpsi_Mass > 3.3");
      RooDataHist *binDataSB = new RooDataHist("binDataSB","Data distribution for background",RooArgSet( *(ws->var("Jpsi_Mass")),*(ws->var("Jpsi_Ct")) ),*redDataSB);
      RooDataSet *redDataSIG = (RooDataSet*)redDataCut->reduce("Jpsi_Mass > 2.9 && Jpsi_Mass < 3.3");

      RooDataHist *binDataCtErrSB = new RooDataHist("binDataCtErrSB","Data ct error distribution for bkg",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSB);
      RooDataHist *binDataCtErrSIG = new RooDataHist("binDataCtErrSIG","Data ct error distribution for sig",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSIG);
      RooHistPdf errPdfBkg("errPdfBkg","Error PDF bkg",RooArgSet(*(ws->var("Jpsi_CtErr"))),*binDataCtErrSB);  ws->import(errPdfBkg);

      // J/psi mass parameterization
      ws->factory("Exponential::expFunct(Jpsi_Mass,coefExp[-1.,-3.,1.])");
      ws->factory("Gaussian::signalG1(Jpsi_Mass,meanSig1[3.0975,3.05,3.15],sigmaSig1[0.1,0.02,0.2])");
      ws->factory("CBShape::signalCB2WN(Jpsi_Mass,meanSig1,sigmaSig2[0.03,0.01,0.06],alpha[1.,0.,3.],enneW[5.,1.,50.])");
      ws->factory("SUM::sigCB2WNG1(coeffGaus[0.1,0.0,1.0]*signalG1,signalCB2WN)");

      // Binning for invariant mass distribution
      RooBinning rbm(2.6,3.5);
      rbm.addUniform(45,2.6,3.5);

      struct PARAM {
        double coeffGaus; double coeffGausErr;
        double meanSig1;  double meanSig1Err;
        double sigmaSig1; double sigmaSig1Err;
        double sigmaSig2; double sigmaSig2Err;
        double sigmaSig3; double sigmaSig3Err;
        double alpha;     double alphaErr;
        double enne;      double enneErr;
        double enneW;     double enneWErr;
      };
      double cutValue_merged;

      double inputN[2] = {0};  //Number of yield/background in the 0-1.571 rad bin
      string inputFN, inputFNcb;
      bool centest = true;  //False: fit w/o any fixed parameters (centrality dep.)
      bool dPhitest = true;  //False: fit w/o any fixed parameters (dPhi dep.)
      if (!crange.compare("0-100")) { centest = false; }
      else {
        centest = true;
        inputFNcb = dirPre + "/rap" + yrange + "_pT" + prange + "_cent0-100_dPhi0.000-1.571.txt";
      }
      if (!phirange.compare("0.000-1.571")) { dPhitest = false; }
      else {
        dPhitest = true;
        inputFN = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi0.000-1.571.txt";
      }

      if (centest) {
        ifstream input(inputFNcb.c_str());
        if (!input.good()) { cout << "Failed to open: " <<inputFNcb << endl; return 1; }
        string tmp;
        double inputNS[2] = {0};
        double inputTmp[2] = {0};
        PARAM inputP;
        input >> tmp >> inputNS[0] >> inputNS[1]; //NSig
        input >> tmp >> inputTmp[0] >> inputTmp[1]; //NBkg
        while (input.good()) {   //Mass signal parameters
          input >> tmp >> inputTmp[0] >> inputTmp[1];
          cout << tmp << " " << inputTmp[0] << endl;
          if (!tmp.compare("coeffGaus")) inputP.coeffGaus = inputTmp[0];
          else if (!tmp.compare("meanSig1")) inputP.meanSig1 = inputTmp[0];
          else if (!tmp.compare("sigmaSig1")) inputP.sigmaSig1 = inputTmp[0];
          else if (!tmp.compare("sigmaSig2")) inputP.sigmaSig2 = inputTmp[0];
          else if (!tmp.compare("alpha")) inputP.alpha = inputTmp[0];
          else if (!tmp.compare("enneW")) inputP.enneW = inputTmp[0];
          else if (!tmp.compare("cutValue")) cutValue_merged = inputTmp[0];
        }

        ws->var("sigmaSig1")->setVal(inputP.sigmaSig1);
        ws->var("coeffGaus")->setVal(inputP.coeffGaus);
        ws->var("meanSig1")->setVal(inputP.meanSig1);
        ws->var("alpha")->setVal(inputP.alpha);
        ws->var("enneW")->setVal(inputP.enneW);

        ws->var("sigmaSig1")->setConstant(kTRUE);
        ws->var("coeffGaus")->setConstant(kTRUE);
        ws->var("meanSig1")->setConstant(kTRUE);
        ws->var("alpha")->setConstant(kTRUE);
        ws->var("enneW")->setConstant(kTRUE);

        input.close();
      }

      if (dPhitest) {
        ifstream input(inputFN.c_str());
        if (!input.good()) { cout <<"Failed to open: " <<  inputFN << endl; return 1; }
        string tmp;
        PARAM inputP;
        double inputNS[2] = {0};
        double inputTmp[2]={0};
        input >> tmp >> inputNS[0] >> inputNS[1]; //NSig
        input >> tmp >> inputTmp[0] >> inputTmp[1]; //NBkg
        while (input.good()) {   //Mass signal parameters
          input >> tmp >> inputTmp[0] >> inputTmp[1];
          cout << tmp << " " << inputTmp[0] << endl;
          if (!tmp.compare("coeffGaus")) inputP.coeffGaus = inputTmp[0];
          else if (!tmp.compare("meanSig1")) inputP.meanSig1 = inputTmp[0];
          else if (!tmp.compare("sigmaSig1")) inputP.sigmaSig1 = inputTmp[0];
          else if (!tmp.compare("sigmaSig2")) inputP.sigmaSig2 = inputTmp[0];
          else if (!tmp.compare("alpha")) inputP.alpha = inputTmp[0];
          else if (!tmp.compare("enneW")) inputP.enneW = inputTmp[0];
          else if (!tmp.compare("cutValue")) cutValue_merged = inputTmp[0];
        }

        ws->var("sigmaSig2")->setVal(inputP.sigmaSig2);
        ws->var("sigmaSig1")->setVal(inputP.sigmaSig1);
        ws->var("coeffGaus")->setVal(inputP.coeffGaus);
        ws->var("meanSig1")->setVal(inputP.meanSig1);
        ws->var("alpha")->setVal(inputP.alpha);
        ws->var("enneW")->setVal(inputP.enneW);

        ws->var("sigmaSig2")->setConstant(kTRUE);
        ws->var("sigmaSig1")->setConstant(kTRUE);
        ws->var("coeffGaus")->setConstant(kTRUE);
        ws->var("meanSig1")->setConstant(kTRUE);
        ws->var("alpha")->setConstant(kTRUE);
        ws->var("enneW")->setConstant(kTRUE);

        input.close();
      }

      double initBkg = redDataSB->sumEntries()*9.0/5.0;
      double initSig = redDataCut->sumEntries() - initBkg;
      cout << initBkg << " " << initSig << endl;
      sprintf(funct,"SUM::sigMassPDF(NSig[%f,1.0,5000000.0]*sigCB2WNG1,NBkg[%f,1.0,50000000.0]*expFunct)",initSig,initBkg);
      ws->factory(funct);
      ws->pdf("sigMassPDF")->fitTo(*redDataCut,Extended(1),Minos(0),Save(1),SumW2Error(kTRUE),NumCPU(8));

      // ** Test Ct error distribution on the sideband region
      RooPlot *errframe2 = ws->var("Jpsi_CtErr")->frame();
      binDataCtErrSB->plotOn(errframe2,DataError(RooAbsData::SumW2));
      ws->pdf("errPdfBkg")->plotOn(errframe2,LineColor(kBlue),Normalization(binDataCtErrSB->sumEntries(),RooAbsReal::NumEvent));

      TCanvas c0("ctau_err","ctau_err",500,500);
      c0.cd(); c0.SetLogy(0); errframe2->Draw();
      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange  + "_dPhi" + phirange + "_testErrPdfBkg_Lin.pdf";
      c0.SaveAs(titlestr.c_str());
      c0.SetLogy(1); errframe2->Draw();
      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi" + phirange + "_testErrPdfBkg_Log.pdf";
      c0.SaveAs(titlestr.c_str());
      delete errframe2;

      // *** scaleF is defined to scale down ct err dist in 2.9-3.3 GeV/c2 signal region
      float bc = ws->var("coefExp")->getVal();
      float scaleF = (exp(2.9*bc)-exp(3.3*bc))/(exp(2.6*bc)-exp(2.9*bc)+exp(3.3*bc)-exp(3.5*bc));
      RooDataHist* subtrData = new RooDataHist("subtrData","Subtracted data",RooArgSet(*(ws->var("Jpsi_CtErr")))); 
      RooDataHist* scaledBkg = subtractSidebands(ws,subtrData,binDataCtErrSIG,binDataCtErrSB,scaleF,"Jpsi_CtErr");
      subtrData->SetName("subtrData");
      RooHistPdf errPdfSig("errPdfSig","Error PDF signal",RooArgSet(*(ws->var("Jpsi_CtErr"))),*subtrData);  ws->import(errPdfSig);

      // ** Check scaled ct err dist curves and data points in signal region are matched
      errframe2 = ws->var("Jpsi_CtErr")->frame();
      subtrData->plotOn(errframe2,DataError(RooAbsData::SumW2));
      ws->pdf("errPdfSig")->plotOn(errframe2,LineColor(kBlue),Normalization(subtrData->sumEntries(),RooAbsReal::NumEvent));
      binDataCtErrSIG->plotOn(errframe2,DataError(RooAbsData::SumW2),LineColor(kRed),MarkerColor(kRed));  //Not subtracted D_sig
      
      c0.Clear(); c0.SetLogy(0); errframe2->Draw();
      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi" + phirange + "_testErrPdfSig_Lin.pdf";
      c0.SaveAs(titlestr.c_str());
      c0.Clear(); c0.SetLogy(1); errframe2->Draw();
      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi" + phirange + "_testErrPdfSig_Log.pdf";
      c0.SaveAs(titlestr.c_str());
      delete errframe2;

      // ** Check the minimum and maximum of the ctau error in signal and background regions
      TH1* hSig = subtrData->createHistogram("hSig",*ws->var("Jpsi_CtErr"));
      TH1* hBkg = scaledBkg->createHistogram("hBkg",*ws->var("Jpsi_CtErr"));
      
      double minSig = 0.5, maxSig = 0.0, minBkg = 0.5, maxBkg = 0.0;
      double cutValue = 0.2;

      int maxBinSig = hSig->GetMaximumBin();
      int maxBinBkg = hBkg->GetMaximumBin();
      
      minSig = hSig->GetBinLowEdge(maxBinSig);
      minBkg = hBkg->GetBinLowEdge(maxBinBkg);
      // pick up lower bound next to other non-zero bins
      for (int xbins = maxBinSig; xbins > 0; xbins--) {
        if (hSig->GetBinContent(xbins) > cutValue) {
          minSig = hSig->GetBinLowEdge(xbins);
//          cout << "SIG binContent: " << hSig->GetBinContent(xbins) << endl;
//          cout << "SIG low edge: " << hSig->GetBinLowEdge(xbins) << endl;
        } else break;
      }
      for (int xbins = maxBinBkg; xbins > 0; xbins--) {
        if (hBkg->GetBinContent(xbins) > cutValue) {
          minBkg = hBkg->GetBinLowEdge(xbins);
//          cout << "BKG binContent: " << hBkg->GetBinContent(xbins) << endl;
//          cout << "BKG low edge: " << hBkg->GetBinLowEdge(xbins) << endl;
        } else break;
      }

      // pick up upper bound next to other non-zero bins
      maxSig = hSig->GetBinLowEdge(maxBinSig+1);
      maxBkg = hBkg->GetBinLowEdge(maxBinBkg+1);
      for (int xbins = maxBinSig; xbins < hSig->GetNbinsX() ; xbins++) {
        if (hSig->GetBinContent(xbins) > cutValue) {
          maxSig = hSig->GetBinLowEdge(xbins+1);
//          cout << "SIG binContent: " << hSig->GetBinContent(xbins) << endl;
//          cout << "SIG upper edge: " << hSig->GetBinLowEdge(xbins+1) << endl;
        } else break;
      }
      for (int xbins = maxBinSig; xbins < hBkg->GetNbinsX() ; xbins++) {
        if (hBkg->GetBinContent(xbins) > cutValue) {
          maxBkg = hBkg->GetBinLowEdge(xbins+1);
//          cout << "BKG binContent: " << hBkg->GetBinContent(xbins) << endl;
//          cout << "BKG upper edge: " << hBkg->GetBinLowEdge(xbins+1) << endl;
        } else break;
      }

      // choose the higher lower limit, lower upper limit
      double tmpMin = 0, tmpMax = 0;
      if (minSig > minBkg) tmpMin = minSig; else tmpMin = minBkg;
      if (maxSig < maxBkg) tmpMax = maxSig; else tmpMax = maxBkg;

      // round off lower limit -> allow more entries on the lower limits
      tmpMin = TMath::Floor(tmpMin*1000);
      tmpMin = tmpMin/(double)1000.0;

      // round up upper limit -> allow more entries on the upper limits
      tmpMax = TMath::Ceil(tmpMax*1000);
      tmpMax = tmpMax/(double)1000.0;

      sprintf(reduceDS,"Jpsi_CtErr > %.3f && Jpsi_CtErr < %.3f",tmpMin,tmpMax);
      RooDataSet *redDataTmp = (RooDataSet*)redData->reduce(reduceDS);
      if (redDataTmp->sumEntries() < redData->sumEntries()*0.9) { // if ctau error range cuts off >10% events
        delete redDataTmp;
        sprintf(reduceDS,"Jpsi_CtErr > %.3f && Jpsi_CtErr < %.3f",minSig,maxSig);
        redDataTmp = (RooDataSet*)redData->reduce(reduceDS);
      }

      if ((tmpMax - tmpMin) < 0.008) {
          cout << "Maximum range is less than minimum! Maybe too few events in this bin.\n";
      }


      // *** Final ctau error plot
      c0.Clear(); c0.Draw(); c0.cd(); c0.SetLogy(1); 
      errframe2 = ws->var("Jpsi_CtErr")->frame();
      binDataCtErrSIG->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kRed),LineColor(kRed));
      binDataCtErrSB->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kGreen+2),LineColor(kGreen+2),MarkerStyle(24));
      scaledBkg->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kBlue),MarkerStyle(24),LineColor(kBlue));
      subtrData->plotOn(errframe2,DataError(RooAbsData::SumW2));
      errframe2->Draw();

      TLatex *t = new TLatex();
      t->SetNDC(); t->SetTextAlign(12);
      t->SetTextSize(0.04);
      titlestr = "|y|: " + yrange + "    p_{T}: " + prange;
      t->DrawLatex(0.5,0.9,titlestr.c_str());
      titlestr = "Cent. " + crange + "%";
      t->DrawLatex(0.5,0.85,titlestr.c_str());
      titlestr = "|#phi^{J/#psi}-#Psi_{RP}|: " + phirange;
      t->DrawLatex(0.5,0.78,titlestr.c_str());
      char comment[200];
      sprintf(comment,"Range: %.3f-%.3f mm",tmpMin,tmpMax);
      t->DrawLatex(0.5,0.72,comment);

      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi" + phirange + "_testErrPdfSigDetail_Log.pdf";
      c0.SaveAs(titlestr.c_str());

      char cerrange[100];
      sprintf(cerrange,"%0.3f-%0.3f",tmpMin,tmpMax);
      output << yrange << " " << prange << " " << crange << " " << phirange << " " << cerrange << endl;

      titlestr = dirPre + "/rap" + yrange + "_pT" + prange + "_cent" + crange + "_dPhi" + phirange + ".txt";
      ofstream outputFile(titlestr.c_str());
      if (!outputFile.good()) {cout << "Fail to open result file." << endl; return 1;}
      outputFile
      << "NSig "         << ws->var("NSig")->getVal()         << " " << ws->var("NSig")->getError() << "\n"
      << "NBkg "         << ws->var("NSig")->getVal()         << " " << ws->var("NBkg")->getError()<< "\n"
      << "coefExp "      << ws->var("coefExp")->getVal()      << " " << ws->var("coefExp")->getError() << "\n"
      << "coeffGaus "    << ws->var("coeffGaus")->getVal()    << " " << ws->var("coeffGaus")->getError() << "\n"
      << "meanSig1 "     << ws->var("meanSig1")->getVal()     << " " << ws->var("meanSig1")->getError() << "\n"
      << "sigmaSig1 "    << ws->var("sigmaSig1")->getVal()    << " " << ws->var("sigmaSig1")->getError() << "\n"
      << "sigmaSig2 "    << ws->var("sigmaSig2")->getVal()    << " " << ws->var("sigmaSig2")->getError()<< "\n"
      << "alpha "        << ws->var("alpha")->getVal()        << " " << ws->var("alpha")->getError() << "\n"
      << "enneW "        << ws->var("enneW")->getVal()        << " " << ws->var("enneW")->getError() << "\n"
      << "cutValue "     << cutValue << "\n";

      outputFile.close();
      fInData.Close();

      delete ws;
      delete data;
      delete binData;
      delete binDataCtErr;
      delete binDataSB;
      delete binDataCtErrSB;
      delete binDataCtErrSIG;
      delete subtrData;
      delete scaledBkg;
      delete t;

        } // end of dphi loop
      } // end of cent loop
    } // end of for loop for pt
  } // end of for loop for rapidity

  output.close();

  return 0;
}




/////////////////////////////////////////////////////////
//////////////////// Sub-routines ///////////////////////
/////////////////////////////////////////////////////////
void getOptRange(string &ran, float *min, float *max) {
  if (sscanf(ran.c_str(), "%f-%f", min, max) == 0) {
    cout << ran.c_str() << ": not valid!" << endl;
    assert(0);
  }
  return ;
}

RooBinning setCtBinning(float lmin,float lmax) {
  RooBinning rb2(-lmin,lmax);

  if (lmax+lmin>4.9) {
    rb2.addBoundary(-1.5);
    rb2.addBoundary(-1.0);
    rb2.addBoundary(-0.8);
    rb2.addBoundary(-0.6);
    rb2.addBoundary(-0.5);
    rb2.addUniform(6,-0.5,-0.2);
    rb2.addUniform(18,-0.2,0.2);
    rb2.addUniform(9,0.2,0.5);
    rb2.addUniform(5,0.5,1.0);
    rb2.addUniform(15,1.0,lmax);
  } else if (lmax+lmin > 4.4) {
    rb2.addBoundary(-1.5);
    rb2.addBoundary(-1.0);
    rb2.addBoundary(-0.8);
    rb2.addBoundary(-0.6);
    rb2.addBoundary(-0.5);
    rb2.addUniform(9,-0.5,-0.2);
    rb2.addUniform(36,-0.2,0.2);
    rb2.addUniform(12,0.2,0.5);
    rb2.addUniform(5,0.5,1.0);
    rb2.addUniform(5,1.0,lmax);
  } else {
    rb2.addBoundary(-1.0);
    rb2.addBoundary(-0.7);
    rb2.addBoundary(-0.6);
    rb2.addBoundary(-0.5);
    rb2.addUniform(9,-0.5,-0.2);
    rb2.addUniform(40,-0.2,0.2);
    rb2.addUniform(12,0.2,0.5);
    rb2.addUniform(10,0.5,1.0);
    rb2.addUniform(4,1.0,lmax);
  }
  return rb2;
}

RooDataHist* subtractSidebands(RooWorkspace* ws, RooDataHist* subtrData, RooDataHist* all, RooDataHist* side, float scalefactor, string varName = "Jpsi_CtErr") {
  const RooArgSet* aRow;
  const RooArgSet* aRowS;
 
  if (all->numEntries() != side->numEntries()) {
    cout << "ERROR subtractSidebands : different binning!" << endl;
    return 0;
  }

  RooDataHist* scaledBkg = new RooDataHist("scaledBkg","Scale applied sideband data",RooArgSet(*(ws->var(varName.c_str())))); 

  for (Int_t i=0; i<all->numEntries(); i++) {
    aRow = all->get(i);
    aRowS = side->get(i);
    RooRealVar* thisVar = (RooRealVar*)aRow->find(varName.c_str());
    ws->var(varName.c_str())->setVal(thisVar->getVal());
    float sfBkg = scalefactor*side->weight(*aRowS,0,false);
    float newWeight = all->weight(*aRow,0,false) - scalefactor*side->weight(*aRowS,0,false);
    if (newWeight <= 0.1) newWeight = 0.1;
    subtrData->add(RooArgSet(*(ws->var(varName.c_str()))),newWeight);
    scaledBkg->add(RooArgSet(*(ws->var(varName.c_str()))),sfBkg);
  }

  return scaledBkg;
}

