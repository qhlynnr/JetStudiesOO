// drawJERJES.C
//
// Extracts Jet Energy Scale (JES) and Jet Energy Resolution (JER)
// from the hperformance TH2F (x = genjetpt, y = rawpt/genjetpt)
// stored in JetLevelHistRaw.
//
// Usage (in ROOT):
//   root [0] .x drawJERJES.C("Data_10.0KEvts_1000evt_30to1000GEV_20260710.root")
//
// Or compile-run:
//   root [0] .L drawJERJES.C+
//   root [1] drawJERJES("yourfile.root")

#include "TFile.h"
#include "TDirectory.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TROOT.h"
#include <vector>
#include <iostream>
#include <cmath>
static void FormatCanvas(TCanvas* c, bool logx, bool logy) {
  c->SetTicks(1,1);
  c->SetLeftMargin(0.14);
  c->SetBottomMargin(0.12);
  c->SetTopMargin(0.08);
  c->SetRightMargin(0.05);

  if (logx) c->SetLogx();
  if (logy) c->SetLogy();
}

// Draws the standard "PYTHIA8 OO (5.36 TeV)" header and z_cut label on the
// current pad. Call after drawing the main plot content.
void drawLabels(bool drawZcut = false)
{
    TLatex* cms = new TLatex(0.10, 0.92, "#bf{PPRef} (5.36 TeV) ");
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);
    cms->Draw();

    if (drawZcut) {
        TLatex* zcut = new TLatex(0.3, 0.815, "#it{z}_{cut} = 0.2");
        zcut->SetNDC();
        zcut->SetTextSize(0.04);
        zcut->SetTextFont(42);
        zcut->Draw();
    }
}

void drawJERJES(bool MC = true,
                const char* filename = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/PPRef/070926PPRefMCData/Test/MC_100.0KEvts_MC_test_Pt30to1000_30to1000GEV_20260710.root",
                 const char* dirname  = "/home/xirong/JetStudiesOO/LynnsCode/Plots/PPRefQAPlots/")
{
    string MCstring;
    if (MC) {
        MCstring = "MC";
    } else {
        MCstring = "Data";
    }
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetOptFit(1);

    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return;
    }

    TDirectory* dir = (TDirectory*) f->Get("JetLevelHistRaw");
    if (!dir) {
        std::cerr << "ERROR: cannot find directory " << dirname << std::endl;
        return;
    }

    TH2F* hperf = (TH2F*) dir->Get("hperformance");
    if (!hperf) {
        std::cerr << "ERROR: cannot find histogram hperformance" << std::endl;
        return;
    }

    // --- Gen-pT bin edges (edit to taste / match your binning) ---
    std::vector<double> ptBins = {30,40,50,60,80,100,120,150,200,250,300,400,500,600,800,1000};
    int nBins = (int)ptBins.size() - 1;

    TGraphErrors* gJES = new TGraphErrors();
    TGraphErrors* gJER = new TGraphErrors();
    gJES->SetName("gJES");
    gJER->SetName("gJER");

    int nx = (int)std::ceil(std::sqrt((double)nBins));
    int ny = (int)std::ceil((double)nBins / nx);
    TCanvas* cFits = new TCanvas("cFits", "Response fits per pT bin", 1400, 900);
    cFits->Divide(nx, ny);
    FormatCanvas(cFits, false, false);
    int point = 0;
    for (int i = 0; i < nBins; i++) {
        double ptLo = ptBins[i], ptHi = ptBins[i+1];
        int binLo = hperf->GetXaxis()->FindBin(ptLo + 1e-3);
        int binHi = hperf->GetXaxis()->FindBin(ptHi - 1e-3);

        TH1D* hproj = hperf->ProjectionY(Form("proj_%d_%d", (int)ptLo, (int)ptHi), binLo, binHi);
        hproj->SetTitle(Form("%.0f < p_{T}^{gen} < %.0f GeV;p_{T}^{reco}/p_{T}^{gen};Entries", ptLo, ptHi));

        if (hproj->GetEntries() < 20) {
            std::cout << "Skipping bin [" << ptLo << "," << ptHi << "): too few entries ("
                      << hproj->GetEntries() << ")" << std::endl;
            continue;
        }

        double mean0 = hproj->GetMean();
        double rms0  = hproj->GetRMS();

        // First pass: loose Gaussian fit
        TF1* fitFunc = new TF1(Form("fit_%d", i), "gaus", mean0 - 2*rms0, mean0 + 2*rms0);
        hproj->Fit(fitFunc, "RQ0");

        // Second pass: refit in +-2 sigma window around first result (standard iterative refinement)
        double mean1 = fitFunc->GetParameter(1);
        double sig1  = fitFunc->GetParameter(2);
        fitFunc->SetRange(mean1 - 2*sig1, mean1 + 2*sig1);
        hproj->Fit(fitFunc, "RQ");

        double mean     = fitFunc->GetParameter(1);
        double meanErr  = fitFunc->GetParError(1);
        double sigma    = fitFunc->GetParameter(2);
        double sigmaErr = fitFunc->GetParError(2);

        double ptCenter = 0.5 * (ptLo + ptHi);
        double ptWidth  = 0.5 * (ptHi - ptLo);

        double jer = (mean != 0) ? sigma / mean : 0;
        double jerErr = (mean != 0 && sigma != 0)
            ? jer * std::sqrt(std::pow(sigmaErr/sigma, 2) + std::pow(meanErr/mean, 2))
            : 0;

        gJES->SetPoint(point, ptCenter, mean);
        gJES->SetPointError(point, ptWidth, meanErr);

        gJER->SetPoint(point, ptCenter, jer);
        gJER->SetPointError(point, ptWidth, jerErr);

        cFits->cd(point + 1);
        hproj->Draw();
        fitFunc->SetLineColor(kRed);
        fitFunc->Draw("same");
        drawLabels(false); // skip z_cut label in the small per-bin pads

        point++;
    }
    cFits->SaveAs(Form("response_fits_%s.png", MCstring.c_str()));

    // --- JES plot ---
    TCanvas* cJES = new TCanvas("cJES", "JES", 600, 600);
    FormatCanvas(cJES, false, false);
    gJES->SetTitle(";p_{T}^{gen} [GeV/c];<p_{T}^{reco}/p_{T}^{gen}>");
    gJES->SetMarkerStyle(20);
    gJES->SetMarkerColor(kBlue+1);
    gJES->SetLineColor(kBlue+1);
    gJES->Draw("AP");
    TLine* unity = new TLine(ptBins.front(), 1.0, ptBins.back(), 1.0);
    unity->SetLineStyle(2);
    unity->SetLineColor(kGray+2);
    unity->Draw("same");
    drawLabels();
    cJES->SaveAs(Form("%s/JES_vs_pt_%s.png", dirname,MCstring.c_str()));

    // --- JER plot ---
    TCanvas* cJER = new TCanvas("cJER", "JER", 600, 600);
    FormatCanvas(cJER, false, false);
    gJER->SetTitle(";p_{T}^{gen} [GeV/c];#sigma(response) / <response>");
    gJER->SetMarkerStyle(20);
    gJER->SetMarkerColor(kRed+1);
    gJER->SetLineColor(kRed+1);
    gJER->Draw("AP");
    drawLabels();
    cJER->SaveAs(Form("%s/JER_vs_pt_%s.png", dirname,MCstring.c_str()));

    // --- Save graphs for later reuse ---
    TFile* fout = new TFile("JES_JER_output.root", "RECREATE");
    gJES->Write();
    gJER->Write();
    fout->Close();

    std::cout << "\nDone. Wrote response_fits.png, JES_vs_pt.png, JER_vs_pt.png, "
              << "and JES_JER_output.root (containing gJES and gJER TGraphErrors)." << std::endl;
}