// PlotSequentialJEC.C
//
// Plots the sequential JEC-corrected jet pT spectra produced by
// SaveHistChain (histograms named "hcorrectedpt_step<N>_<Level>" in the
// "JetLevelHistRaw" directory), overlaid on top of the raw pT spectrum.
//
// Usage:
//   root -l -b -q 'PlotSequentialJEC.C("MyOutputFile.root")'
//   root -l -b -q 'PlotSequentialJEC.C("MyOutputFile.root","SeqJEC.pdf",true,true)'
//
// Arguments:
//   inFileName   - path to the ROOT file made by SaveHistChain
//   outFileName  - output image file (.pdf/.png/...)
//   normalize    - if true, each histogram is scaled to unit area before plotting
//   logY         - if true, canvas y-axis is drawn in log scale

#include <TFile.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TStyle.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

struct StepHist {
    int step;
    string level;
    TH1F* hist;
};

void PlotSequentialJEC(const string& inFileName  = "output.root",
                        const string& outFileName = "SequentialJEC.pdf",
                        bool normalize = false,
                        bool logY = true)
{
    gStyle->SetOptStat(0);

    TFile* f = TFile::Open(inFileName.c_str(), "READ");
    if (!f || f->IsZombie()) {
        cout << "ERROR: could not open " << inFileName << endl;
        return;
    }

    TDirectory* dir = (TDirectory*)f->Get("JetLevelHistRaw");
    if (!dir) {
        cout << "ERROR: could not find directory 'JetLevelHistRaw' in " << inFileName << endl;
        f->Close();
        return;
    }

    // ---- Collect all hcorrectedpt_step<N>_<Level> histograms ----------
    vector<StepHist> steps;
    TRegexp re("hcorrectedpt_step[0-9]+_.*");
    TIter next(dir->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TString name = key->GetName();
        if (name.Index(re) == kNPOS) continue;

        TH1F* h = (TH1F*)dir->Get(name);
        if (!h) continue;
        h->SetDirectory(0); // detach from file so it survives f->Close()

        // name looks like: hcorrectedpt_step<N>_<Level...>
        TObjArray* parts = name.Tokenize("_");
        int stepnum = -1;
        string level = "unknown";
        if (parts->GetEntries() >= 2) {
            TString stepTok = ((TObjString*)parts->At(1))->GetString(); // "step3"
            stepTok.ReplaceAll("step", "");
            stepnum = stepTok.Atoi();
        }
        if (parts->GetEntries() >= 3) {
            TString lvl;
            for (int i = 2; i < parts->GetEntries(); ++i) {
                if (i > 2) lvl += "_";
                lvl += ((TObjString*)parts->At(i))->GetString();
            }
            level = lvl.Data();
        }
        delete parts;

        steps.push_back({stepnum, level, h});
    }

    if (steps.empty()) {
        cout << "ERROR: no 'hcorrectedpt_step*' histograms found in " << inFileName << endl;
        f->Close();
        return;
    }

    sort(steps.begin(), steps.end(),
         [](const StepHist& a, const StepHist& b) { return a.step < b.step; });

    // Raw pT as the "step 0" reference curve
    TH1F* hraw = (TH1F*)dir->Get("hrawpt");
    if (hraw) hraw->SetDirectory(0);

    // ---- Draw ----------------------------------------------------------
    TCanvas* c = new TCanvas("cSequentialJEC", "Sequential JEC Corrections", 900, 700);
    c->SetLogy(logY);
    c->SetLogx();
    c->SetTicks(1, 1);

    TLegend* leg = new TLegend(0.55, 0.58, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.028);

    vector<int> colors = {kBlack, kRed+1, kBlue+1, kGreen+2, kMagenta+1,
                           kOrange+1, kCyan+2, kViolet+1, kSpring+3, kAzure+2};

    // find overall max for axis range, after applying normalization if requested
    double maxVal = 0;
    if (hraw) {
        if (normalize && hraw->Integral() > 0) hraw->Scale(1.0 / hraw->Integral());
        maxVal = max(maxVal, hraw->GetMaximum());
    }
    for (auto& s : steps) {
        if (normalize && s.hist->Integral() > 0) s.hist->Scale(1.0 / s.hist->Integral());
        maxVal = max(maxVal, s.hist->GetMaximum());
    }

    int colorIdx = 0;
    bool firstDrawn = false;

    TString yTitle = normalize ? "Normalized entries" : "Number of Jets";
    TString axisTitle = Form(";Jet p_{T} (GeV/c);%s", yTitle.Data());

    if (hraw) {
        hraw->SetLineColor(colors[colorIdx++ % colors.size()]);
        hraw->SetLineWidth(2);
        hraw->SetTitle(("Sequential JEC Corrections" + axisTitle).Data());
        hraw->SetMaximum(maxVal * 5);
        hraw->SetMinimum(normalize ? 1e-6 : 0.5);
        hraw->Draw("HIST");
        leg->AddEntry(hraw, "Raw p_{T} (uncorrected)", "l");
        firstDrawn = true;
    }

    for (auto& s : steps) {
        TH1F* h = s.hist;
        h->SetLineColor(colors[colorIdx++ % colors.size()]);
        h->SetLineWidth(2);

        if (!firstDrawn) {
            h->SetTitle(("Sequential JEC Corrections" + axisTitle).Data());
            h->SetMaximum(maxVal * 5);
            h->SetMinimum(normalize ? 1e-6 : 0.5);
            h->Draw("HIST");
            firstDrawn = true;
        } else {
            h->Draw("HIST SAME");
        }

        TString label = Form("Step %d: %s", s.step, s.level.c_str());
        leg->AddEntry(h, label.Data(), "l");
    }

    leg->Draw();
    c->SaveAs(outFileName.c_str());

    // ---- Summary ---------------------------------------------------------
    cout << "\nSaved plot to " << outFileName << endl;
    cout << "Found " << steps.size() << " sequential JEC step histogram(s):" << endl;
    if (hraw) cout << "  Step 0: raw (integral=" << hraw->Integral() << ")" << endl;
    for (auto& s : steps) {
        cout << "  Step " << s.step << ": " << s.level
             << " (integral=" << s.hist->Integral() << ")" << endl;
    }

    f->Close();
}