// drawVetoMaps.C
// Quick plot of every top-level TH2D in the JME veto-map file
// (jetvetomap_hot, jetvetomap_cold, ..., jetpullsummap_ref, jetvetomap).
//
// Usage:
//   root [0] .x drawVetoMaps.C("Winter24Prompt24_2024BCDEFGHI.root")

#include <TFile.h>
#include <TKey.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>

void drawVetoMaps(const char* filename = "/home/xirong/JetStudiesOO/Include/Winter24Prompt24_2024BCDEFGHI.root",
                   const char* outdir  = "/home/xirong/JetStudiesOO/LynnsCode/Plots/PPRefJetVetoMap")
{
    gStyle->SetOptStat(0);

    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return;
    }

    TIter next(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*) next())) {
        // skip the "trigs" TDirectoryFile, only draw top-level TH2s
        if (!TString(key->GetClassName()).BeginsWith("TH2")) continue;

        TH2* h = (TH2*) key->ReadObj();
        if (!h) continue;

        TCanvas* c = new TCanvas(Form("c_%s", h->GetName()), h->GetName(), 800, 600);
        c->SetRightMargin(0.15);
        h->SetTitle(h->GetTitle()); // keep original title (e.g. "Hot zones...")
        h->Draw("colz");
        c->SaveAs(Form("%s/%s.png", outdir, h->GetName()));

        std::cout << "Drew " << h->GetName() << std::endl;
    }

    std::cout << "\nDone." << std::endl;
}