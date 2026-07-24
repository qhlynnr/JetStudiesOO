// drawAllJetHists.C
//
// Draws each histogram found in the JetLevelHistRaw directory
// (hjtpt, hcorrectedpt, hjteta, hjtphi, hjtrg, hjtzg, hjtkt, hperformance, ...)
// on its own canvas and saves each as a PNG.
//
// Usage (in ROOT):
//   root [0] .x drawAllJetHists.C("Data_10.0KEvts_1000evt_30to1000GEV_20260710.root")
//
// Or compile-run:
//   root [0] .L drawAllJetHists.C+
//   root [1] drawAllJetHists("yourfile.root")

#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TList.h"
#include <iostream>
#include <string>

// Draws the standard "PYTHIA8 OO (5.36 TeV)" header on the current pad.
void drawHeader()
{
    TLatex* cms = new TLatex(0.10, 0.92, "#bf{PPRef} (5.36 TeV) ");
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);
    cms->Draw();
}

void drawAllJetHists(const char* filename = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/PPRef/070926PPRefMCData/Test/Data_10.0KEvts_1000evt_30to1000GEV_20260710.root",
                      const char* dirname  = "JetLevelHistRaw",
                      const char* outdir   = "/home/xirong/JetStudiesOO/LynnsCode/Plots/PPRefQAPlots/")
{
    gStyle->SetOptStat(1111);
    gStyle->SetOptTitle(0);

    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return;
    }

    TDirectory* dir = (TDirectory*) f->Get(dirname);
    if (!dir) {
        std::cerr << "ERROR: cannot find directory " << dirname << std::endl;
        return;
    }

    // Loop over every key in the directory and draw whatever is a histogram
    TList* keys = dir->GetListOfKeys();
    TIter next(keys);
    TKey* key;

    while ((key = (TKey*) next())) {
        TObject* obj = key->ReadObj();
        if (!obj) continue;

        // --- 2D histograms (e.g. hperformance) ---
        if (obj->InheritsFrom("TH2")) {
            TH2* h2 = (TH2*) obj;
            TCanvas* c = new TCanvas(Form("c_%s", h2->GetName()), h2->GetName(), 700, 600);
            c->SetRightMargin(0.15);
            c->SetLogz();
            h2->Draw("colz");
            drawHeader();
            c->SaveAs(Form("%s/%s.png", outdir, h2->GetName()));
            std::cout << "Drew 2D hist: " << h2->GetName() << std::endl;
            continue;
        }

        // --- 1D histograms (hjtpt, hcorrectedpt, hjteta, hjtphi, hjtrg, hjtzg, hjtkt, ...) ---
        if (obj->InheritsFrom("TH1")) {
            TH1* h1 = (TH1*) obj;
            TCanvas* c = new TCanvas(Form("c_%s", h1->GetName()), h1->GetName(), 700, 600);
            c->SetLeftMargin(0.12);

            h1->SetLineColor(kAzure - 2);
            h1->SetLineWidth(2);
            h1->SetMarkerStyle(20);
            h1->SetMarkerColor(kAzure - 2);
            h1->Draw("hist");

            drawHeader();
            c->SaveAs(Form("%s/%s.png", outdir, h1->GetName()));
            std::cout << "Drew 1D hist: " << h1->GetName() << std::endl;
            continue;
        }

        // Not a histogram (e.g. TNamed) - skip
    }

    std::cout << "\nDone. PNGs saved to " << outdir << "/" << std::endl;
}