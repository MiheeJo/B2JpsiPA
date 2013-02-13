#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <TROOT.h>
#include <TMath.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TGraphErrors.h>

using namespace std;

void SetGraphStyle(TGraph* h, int i, int j) {
//  int colorArr[6] = {kBlack, kBlue, kRed, kGreen+2, kOrange+1, kMagenta+2};
//  int colorArr[] = {kGray+2, kBlack, kRed, kRed+2, kOrange+1, kOrange+8, kGreen+2, kGreen+4, kAzure+1, kAzure+3, kViolet+5, kViolet+3, kMagenta, kMagenta+2};
  int colorArr[] = {kBlack, kRed+1, kOrange+1, kGreen+2, kAzure+1, kBlue+2, kViolet+5, kViolet-4, kMagenta, kMagenta+2};
  int markerFullArr[6] = {kFullCircle, kFullSquare, kFullStar, kFullTriangleUp, kFullTriangleDown, kFullDiamond};
//  int markerOpenArr[6] = {kOpenCircle, kOpenSquare, kOpenStar, kOpenTriangleUp, kOpenTriangleDown, kOpenDiamond};
  
  h->SetLineColor(colorArr[j]);
  h->SetLineWidth(2.4);
  h->SetMarkerColor(colorArr[j]);
  h->SetMarkerStyle(markerFullArr[i]);
  h->SetMarkerSize(1.5);
  if (i == 2) h->SetMarkerSize(2.5);
}

void SetHistStyle(TH1* h, int i, int j) {
//  int colorArr[6] = {kBlack, kBlue, kRed, kGreen+2, kOrange+1, kMagenta+2};
//  int colorArr[] = {kGray+2, kBlack, kRed, kRed+2, kOrange+1, kOrange+8, kGreen+2, kGreen+4, kAzure+1, kAzure+3, kViolet+5, kViolet+3, kMagenta, kMagenta+2};
//  int colorArr[] = {kRed+1, kOrange+1, kOrange-8, kGreen+2, kAzure+1, kBlue+2, kViolet+5, kViolet-4, kMagenta, kMagenta+2};
  int colorArr[] = {kBlack, kRed+1, kOrange+1, kGreen+2, kAzure+1, kBlue+2, kViolet+5, kViolet-4, kMagenta, kMagenta+2};
  int markerFullArr[6] = {kFullCircle, kFullSquare, kFullStar, kFullTriangleUp, kFullTriangleDown, kFullDiamond};
//  int markerOpenArr[6] = {kOpenCircle, kOpenSquare, kOpenStar, kOpenTriangleUp, kOpenTriangleDown, kOpenDiamond};
  
  h->SetLineColor(colorArr[j]);
  h->SetLineWidth(2.4);
  h->SetMarkerColor(colorArr[j]);
  h->SetMarkerStyle(markerFullArr[i]);
  h->SetMarkerSize(1.5);
  if (i == 2) h->SetMarkerSize(2.5);
}

void SetLegendStyle(TLegend* l) {
  l->SetFillColor(0);
  l->SetFillStyle(4000);
  l->SetBorderSize(0);
  l->SetMargin(0.1);
}


int FwBwRatio_rap() {

  string str_sig[] = {"Inclusive J/#psi", "Prompt J/#psi", "Non-prompt J/#psi"};
  string str_sig_filename[] = {"Inclusive_nRap3", "Prompt_nRap3", "Non-prompt_nRap3"};
  string str_rap[] = {
  "[-2.4, -1.757]", "[-1.757, -1.113]", "[-1.113, -0.47]", "[-0.47, 0.173]", "[0.173, 0.817]", "[0.817. 1.46]"
  };
//  string str_sig_filename[] = {"Inclusive_nRap5", "Prompt_nRap5", "Non-prompt_nRap5"};
//  string str_rap[] = {
//  "[-2.4, -2.014]", "[-2.014, -1.628]", "[-1.628, -1.242]", "[-1.242, -0.856]", "[-0.856, -0.47]", "[-0.47, -0.084]", "[-0.084, 0.302]", "[0.302. 0.688]", "[0.688, 1.074]", "[1.074, 1.46]", "[1.46, 1.93]", "[1.93, 2.4]"
//  };
  string str_pt[] = {"6.5-30"};
  string str_cent[] = {
    "50-100","30-50","10-30","0-10"
  };

  const int nBW = 3;  // Count number of backward bins
//  const int nBW = 5;  // Count number of backward bins
  const int nRap = sizeof(str_rap)/sizeof(string);
  const int nPt = sizeof(str_pt)/sizeof(string);
  const int nCent = sizeof(str_cent)/sizeof(string);
  const int nSig = sizeof(str_sig)/sizeof(string);

  // Number of binary collision is defined with Z raw yields for the moment
  double Ncoll[nCent] = {1.000, 0.742, 0.457, 0.552};  // 100, 50, 30, 10, 0 for 18.38/nb
  double NcollErr[nCent] = {0.032, 0.022, 0.014, 0.020};  // 100, 50, 30, 10, 0 for 18.38/nb

  // raw yields
  double forward[nSig][nRap][nCent];
  double backward[nSig][nRap][nCent];
  double forwardErr[nSig][nRap][nCent];
  double backwardErr[nSig][nRap][nCent];

  // R_cp of raw yields
  double forwardRcp[nSig][nRap][nCent];
  double backwardRcp[nSig][nRap][nCent];
  double forwardRcpErr[nSig][nRap][nCent];
  double backwardRcpErr[nSig][nRap][nCent];
  
  // Ratio of R_cp (forward/backward)
  double finalRatio[nSig][nRap][nCent];
  double finalRatioErr[nSig][nRap][nCent];

  // Get numbers from txt file
  ifstream finput;
  finput.open(Form("./fit_short_nRap3"));
//  finput.open(Form("./fit_short_nRap5_noSingleMuEtaCuts"));
  if (!finput.good()) {
    cout << "Cannot open input file: " << Form("./fit_short") << endl;
    return -1;
  }
  for (int rap=0; rap<nBW; rap++) {
    for (int cent=0; cent<nCent; cent++) {
      for (int sig=0; sig<nSig; sig++) {
        finput >> backward[sig][rap][cent] >> backwardErr[sig][rap][cent];
      }
    } // cent ends
  } // rap ends
  for (int rap=nBW; rap<nRap; rap++) {
    for (int cent=0; cent<nCent; cent++) {
      for (int sig=0; sig<nSig; sig++) {
        finput >> forward[sig][rap-nBW][cent] >> forwardErr[sig][rap-nBW][cent];
      }
    } // cent ends
  } // rap ends

  // Get Rcp and ratio of (forward/backward)
  for (int sig=0; sig<nSig; sig++) {
    for (int rap=0; rap<nRap; rap++) {
      for (int cent=0; cent<nCent; cent++) {

        forwardRcp[sig][rap][cent] = Ncoll[cent] * forward[sig][rap][cent] / forward[sig][rap][0];
        backwardRcp[sig][rap][cent] = Ncoll[cent] * backward[sig][rap][cent] / backward[sig][rap][0];

        double f_ncoll = NcollErr[cent]/Ncoll[cent];
        
        double f_central = forwardErr[sig][rap][cent] / forward[sig][rap][cent];
        double f_peri = forwardErr[sig][rap][0] / forward[sig][rap][0];
        forwardRcpErr[sig][rap][cent] = forwardRcp[sig][rap][cent] * TMath::Sqrt(TMath::Power(f_ncoll,2)+TMath::Power(f_central,2)+TMath::Power(f_peri,2));
        double b_central = backwardErr[sig][rap][cent] / backward[sig][rap][cent];
        double b_peri = backwardErr[sig][rap][0] / backward[sig][rap][0];
        backwardRcpErr[sig][rap][cent] = backwardRcp[sig][rap][cent] * TMath::Sqrt(TMath::Power(f_ncoll,2)+TMath::Power(b_central,2)+TMath::Power(b_peri,2));

        // Get Rcp ratio
        finalRatio[sig][rap][cent] = forwardRcp[sig][rap][cent]/backwardRcp[sig][rap][cent];
        
        double r_forw = forwardRcpErr[sig][rap][cent]/forwardRcp[sig][rap][cent];
        double r_back = backwardRcpErr[sig][rap][cent]/backwardRcp[sig][rap][cent];
        finalRatioErr[sig][rap][cent] = finalRatio[sig][rap][cent]*TMath::Sqrt(TMath::Power(r_forw,2)+TMath::Power(r_back,2));
      }
    }
  }

  double xaxis[nRap-nBW][nCent] = {{0}};
  double xaxis2[nCent+1] = {0};
  double yaxis[nCent] = {0};
  double yaxis2[nCent+1] = {0};
  for (int i=0; i<nCent; i++) {
    for (int j=0; j<nRap-nBW; j++) {
      xaxis[j][i] = i+0.35+(j*0.05);
    }
    yaxis[i] = 1;
  }
  for (int i=0; i<nCent+1; i++) {
    xaxis2[i] = i;
    yaxis2[i] = 1;
  }

  gROOT->Macro("~/JpsiStyle.C");
  TCanvas canv;  canv.Draw();
  TH1D *gr = new TH1D("ghost",";;",nCent,0,nCent);
  gr->GetYaxis()->SetRangeUser(0.5,3);
  gr->GetXaxis()->LabelsOption("h");
  gr->GetXaxis()->SetLabelSize(0.055);
  
  for (int i=0; i<nCent; i++) {
    gr->GetXaxis()->SetBinLabel(i+1,Form("%s%%",str_cent[i].c_str()));
  }
  gr->GetYaxis()->SetTitle("Forward R_{cp}");

  TLegend *leg;
  TLine *gline = new TLine();
  gline->SetLineWidth(1.2);
  gline->SetLineColor(kGray+1);

  TGraphErrors *gr_forwardRcp[nSig][nRap-nBW];
  for (int sig=0; sig<nSig; sig++) {
//    leg = new TLegend(0.15,0.58,0.93,0.93);
    leg = new TLegend(0.15,0.7,0.93,0.93);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);

    for (int rap=0; rap<nRap-nBW; rap++) {
      gr_forwardRcp[sig][rap] = new TGraphErrors(nCent,xaxis[rap],forwardRcp[sig][rap],0,forwardRcpErr[sig][rap]);
      
      SetGraphStyle(gr_forwardRcp[sig][rap],sig,rap);
      
      char name[512];
      sprintf(name,"y: %s",str_rap[rap+nBW].c_str());
      leg->AddEntry(gr_forwardRcp[sig][rap],name,"lp");

      gr_forwardRcp[sig][rap]->Draw("[lp]");
    }

    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_rap_fw_%s.pdf",str_sig_filename[sig].c_str()));
    delete leg;
    canv.Clear();
  }

  gr->GetYaxis()->SetTitle("Backward R_{cp}");
  TGraphErrors *gr_backwardRcp[nSig][nBW];
  for (int sig=0; sig<nSig; sig++) {
//    leg = new TLegend(0.15,0.65,0.93,0.93);
    leg = new TLegend(0.15,0.7,0.93,0.93);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);

    for (int rap=0; rap<nBW; rap++) {
      gr_backwardRcp[sig][rap] = new TGraphErrors(nCent,xaxis[rap],backwardRcp[sig][rap],0,backwardRcpErr[sig][rap]);
      
      SetGraphStyle(gr_backwardRcp[sig][rap],sig,(nBW-1)-rap);
      
      char name[512];
      sprintf(name,"y: %s",str_rap[rap].c_str());
      leg->AddEntry(gr_backwardRcp[sig][rap],name,"lp");

      gr_backwardRcp[sig][rap]->Draw("[lp]");
    }
    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_rap_bw_%s.pdf",str_sig_filename[sig].c_str()));
    canv.Clear();
    delete leg;
  }


/*  gr->GetYaxis()->SetTitle("Ratio of forward R_{cp}/backward R_{cp}");
  TGraphErrors *gr_finalRatio[nSig][nRap];
  for (int sig=0; sig<nSig; sig++) {
    leg = new TLegend(0.3,0.7,0.93,0.9);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);
    for (int rap=0; rap<nRap; rap++) {
      gr_finalRatio[sig][rap]= new TGraphErrors(nCent,xaxis[rap],finalRatio[sig][rap],0,finalRatioErr[sig][rap]);
      
      SetGraphStyle(gr_finalRatio[sig][rap],sig,rap);
      
      char name[512];
      sprintf(name,"%s, y: %s",str_sig[sig].c_str(),str_rap[rap].c_str());
      leg->AddEntry(gr_finalRatio[sig][rap],name,"lp");

      gr_finalRatio[sig][rap]->Draw("[lp]");
    }
    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_rap_fwbw_%s.pdf",str_sig_filename[sig].c_str()));
    canv.Clear();
    delete leg;
  }*/

  return 0;
}



