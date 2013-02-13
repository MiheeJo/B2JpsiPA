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
using namespace TMath;

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


int FwBwRatio_cent() {

  string str_sig[] = {"Inclusive J/#psi", "Prompt J/#psi", "Non-prompt J/#psi"};
  string str_sig_filename[] = {"Inclusive", "Prompt", "Non-prompt"};
  string str_rap[] = {"-2.4--0.47","-0.47-1.46"};
  string str_pt[] = {"3-5","5-6.5","6.5-8","8-10","10-13","13-30"};
  string str_cent[] = {"50-100", "50-30", "30-10","0-10"};
//  "70-100", "50-70", "40-50", "30-40", "20-30", "10-20", "0-10" 

  const int nRap = sizeof(str_rap)/sizeof(string);
  const int nPt = sizeof(str_pt)/sizeof(string);
  const int nCent = sizeof(str_cent)/sizeof(string);
  const int nSig = sizeof(str_sig)/sizeof(string);

  // Number of binary collision is defined with Z raw yields for the moment
//  double Ncoll[nCent] = {1.000, 0.326, 0.422, 0.321, 0.256, 0.200, 0.136};  // 100, 70, 50, 40, 30, 20, 10, 0 for 18.38/nb
//  double NcollErr[nCent] = {0.000, 0.010, 0.012, 0.007, 0.009, 0.004, 0.004};  // 100, 70, 50, 40, 30, 20, 10, 0
  double Ncoll[nCent] = {1.000, 0.742, 0.457, 0.552};// 100, 50, 30, 10, 0 for 18.38/nb
  double NcollErr[nCent] = {0.032, 0.022, 0.014, 0.020};// 100, 50, 30, 10, 0 for 18.38/nb

  // raw yields
  double forward[nSig][nPt][nCent];
  double backward[nSig][nPt][nCent];
  double forwardErr[nSig][nPt][nCent];
  double backwardErr[nSig][nPt][nCent];

  // R_cp of raw yields
  double forwardRcp[nSig][nPt][nCent];
  double backwardRcp[nSig][nPt][nCent];
  double forwardRcpErr[nSig][nPt][nCent];
  double backwardRcpErr[nSig][nPt][nCent];
  
  // Ratio of R_cp (forward/backward)
  double finalRatio[nSig][nPt][nCent];
  double finalRatioErr[nSig][nPt][nCent];

  // Get numbers from txt file
  ifstream finput;
  finput.open("./fit_short_centbins4_pTdiff");
  if (!finput.good()) {
    cout << "Cannot open input file: " << Form("./fit_short_centbins4_pTdiff") << endl;
    return -1;
  }

  for (int pt=0; pt<nPt; pt++) {
    for (int cent=0; cent<nCent; cent++) {
      for (int sig=0; sig<nSig; sig++) {
        finput >> forward[sig][pt][cent] >> forwardErr[sig][pt][cent];
      }
      for (int sig=0; sig<nSig; sig++) {
        finput >> backward[sig][pt][cent] >> backwardErr[sig][pt][cent];
      }
    } // cent ends
  } // pt ends

  // Get Rcp and ratio of (forward/backward)
  for (int sig=0; sig<nSig; sig++) {
    for (int pt=0; pt<nPt; pt++) {
      for (int cent=0; cent<nCent; cent++) {

        forwardRcp[sig][pt][cent] = Ncoll[cent] * forward[sig][pt][cent] / forward[sig][pt][0];
        backwardRcp[sig][pt][cent] = Ncoll[cent] * backward[sig][pt][cent] / backward[sig][pt][0];

        double f_ncoll = NcollErr[cent]/Ncoll[cent];

        double f_central = forwardErr[sig][pt][cent] / forward[sig][pt][cent];
        double f_peri = forwardErr[sig][pt][0] / forward[sig][pt][0];
        forwardRcpErr[sig][pt][cent] = forwardRcp[sig][pt][cent] * Sqrt(Power(f_ncoll,2)+Power(f_central,2)+Power(f_peri,2));
        double b_central = backwardErr[sig][pt][cent] / backward[sig][pt][cent];
        double b_peri = backwardErr[sig][pt][0] / backward[sig][pt][0];
        backwardRcpErr[sig][pt][cent] = backwardRcp[sig][pt][cent] * Sqrt(Power(f_ncoll,2)+Power(b_central,2)+Power(b_peri,2));

        cout << "[" << sig << "][" << pt << "][" << cent << "]: " << endl;
        cout << forward[sig][pt][cent] << " " << forwardErr[sig][pt][cent] << " " << backward[sig][pt][cent] << " " << backwardErr[sig][pt][cent] << endl;
        cout << f_central << " " << f_peri << " " << b_central << " " << b_peri << endl;
        cout << Power(NcollErr[cent],2) << " " << Power(NcollErr[cent],2) << endl;
        cout << Power(f_central,2) << " " << Power(b_central,2) << " " << Power(f_peri,2) << " " << Power(b_peri,2) << endl;
        cout << Sqrt(Power(NcollErr[cent],2)+Power(f_central,2)+Power(f_peri,2)) << " " << Sqrt(Power(NcollErr[cent],2)+Power(b_central,2)+Power(b_peri,2)) << endl;
        cout << "fRcp [" << sig << "][" << pt << "][" << cent << "]: " << forwardRcp[sig][pt][cent] << " " << forwardRcpErr[sig][pt][cent];
        cout << " bRcp [" << sig << "][" << pt << "][" << cent << "]: " << backwardRcp[sig][pt][cent] << " " << backwardRcpErr[sig][pt][cent] << endl;

        // Get Rcp ratio
        finalRatio[sig][pt][cent] = forwardRcp[sig][pt][cent]/backwardRcp[sig][pt][cent];
        
        double r_forw = forwardRcpErr[sig][pt][cent]/forwardRcp[sig][pt][cent];
        double r_back = backwardRcpErr[sig][pt][cent]/backwardRcp[sig][pt][cent];
        finalRatioErr[sig][pt][cent] = finalRatio[sig][pt][cent]*Sqrt(Power(r_forw,2)+Power(r_back,2));
      } // cent end
    } // pt end
  } //sig end

  double xaxis[nPt][nCent] = {{0}};
  double xaxis2[nCent+1] = {0};
  double yaxis[nCent] = {0};
  double yaxis2[nCent+1] = {0};
  for (int i=0; i<nCent; i++) {
    for (int j=0; j<nPt; j++) {
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
  
  for (int i=0; i<nCent; i++) gr->GetXaxis()->SetBinLabel(i+1,Form("%s%%",str_cent[i].c_str()));
  gr->GetYaxis()->SetTitle("Forward R_{cp}");

  TLegend *leg;
  TLine *gline = new TLine();
  gline->SetLineWidth(1.2);
  gline->SetLineColor(kGray+1);

  TGraphErrors *gr_forwardRcp[nSig][nPt];
  for (int sig=0; sig<nSig; sig++) {
    leg = new TLegend(0.15,0.71,0.93,0.93);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);

    //Because [0,3], [3,5] GeV/c aren't included in the acceptance region
    for (int pt=1; pt<nPt; pt++) {
      gr_forwardRcp[sig][pt] = new TGraphErrors(nCent,xaxis[pt],forwardRcp[sig][pt],0,forwardRcpErr[sig][pt]);
      
      SetGraphStyle(gr_forwardRcp[sig][pt],sig,pt);
      
      char name[512];
      sprintf(name,"p_{T} %s GeV/c",str_pt[pt].c_str());
      leg->AddEntry(gr_forwardRcp[sig][pt],name,"lp");

      gr_forwardRcp[sig][pt]->Draw("[lp]");
    }

    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_cent_fw_%s.pdf",str_sig_filename[sig].c_str()));
    delete leg;
    canv.Clear();
  }

  gr->GetYaxis()->SetTitle("Backward R_{cp}");
  TGraphErrors *gr_backwardRcp[nSig][nPt];
  for (int sig=0; sig<nSig; sig++) {
    leg = new TLegend(0.15,0.65,0.93,0.93);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);

    for (int pt=0; pt<nPt; pt++) {
      gr_backwardRcp[sig][pt] = new TGraphErrors(nCent,xaxis[pt],backwardRcp[sig][pt],0,backwardRcpErr[sig][pt]);
      
      SetGraphStyle(gr_backwardRcp[sig][pt],sig,pt);
      
      char name[512];
      sprintf(name,"p_{T} %s GeV/c",str_pt[pt].c_str());
      leg->AddEntry(gr_backwardRcp[sig][pt],name,"lp");

      gr_backwardRcp[sig][pt]->Draw("[lp]");
    }
    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_cent_bw_%s.pdf",str_sig_filename[sig].c_str()));
    canv.Clear();
    delete leg;
  }


  gr->GetYaxis()->SetTitle("Ratio of forward R_{cp}/backward R_{cp}");
  TGraphErrors *gr_finalRatio[nSig][nPt];
  for (int sig=0; sig<nSig; sig++) {
    leg = new TLegend(0.15,0.71,0.93,0.93);
    SetLegendStyle(leg);
    leg->SetHeader(str_sig[sig].c_str());
    gr->Draw("l");
    gline->DrawLine(0,1,nCent,1);
    
    //Because [0,3], [3,5] GeV/c aren't included in the acceptance region
    for (int pt=1; pt<nPt; pt++) {
      gr_finalRatio[sig][pt]= new TGraphErrors(nCent,xaxis[pt],finalRatio[sig][pt],0,finalRatioErr[sig][pt]);
      
      SetGraphStyle(gr_finalRatio[sig][pt],sig,pt);
      
      char name[512];
      sprintf(name,"p_{T} %s GeV/c",str_pt[pt].c_str());
      leg->AddEntry(gr_finalRatio[sig][pt],name,"lp");

      gr_finalRatio[sig][pt]->Draw("[lp]");
    }
    leg->Draw();
    canv.SaveAs(Form("Ratio_Rcp_cent_fwbw_%s.pdf",str_sig_filename[sig].c_str()));
    canv.Clear();
    delete leg;
  }

  return 0;
}



