// CompareMCData.C
// Compares Data vs MC jet-level histograms (hjtpt, hjteta, hjtphi, hjtrg, hjtzg, hjtkt)
// across 4 pT bins. Both Data and MC files store these under JetLevelHistRaw with the
// same names; the MC file additionally has refpt/refeta/... and genpt/geneta/... branches
// on its TTree (not used here directly, since this macro only compares the pre-filled
// JetLevelHistRaw histograms - see the bottom of the file for how to extend this to
// gen-level histograms if you also fill e.g. hgenpt/hrefpt into JetLevelHistRaw for MC).
//
// Structure follows the same DrawAndSaveWithRatio/StyleHist/BlockCaptionInfo helpers as
// the 6-bin QA script, but uses vectors + loops instead of per-bin hardcoded variables
// so adding/removing bins or observables only means editing the config lists at the top.

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TKey.h>
#include <TStyle.h>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

struct BlockCaptionInfo{
  std::vector<std::string> text;
  double x;
  double y;
  double size;
  double spacing;
  bool useNDC;
};

static void DrawTLatexLines(const BlockCaptionInfo& info)
{
  TLatex lat;
  lat.SetNDC(info.useNDC);
  lat.SetTextSize(info.size);
  double yy = info.y;
  for (const auto& s : info.text) {
    lat.SetTextFont(132);
    lat.DrawLatex(info.x, yy, s.c_str());
    yy -= info.spacing;
  }
}

static void StyleHist(TH1* h, int color, int marker){
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerStyle(marker);
  h->SetLineWidth(2);
}

static void DrawAndSaveWithRatio(const std::vector<TH1*>& hs,
                        const std::vector<std::string>& labels,
                        const std::string& outPng,
                        const std::string& xTitle,
                        const std::string& yTitle = "Normalized entries",
                        const std::string& title = "",
                        double xMin = -999, double xMax = -999,
                        double yMin = -999, double yMax = -999,
                        bool logx = false,
                        bool logy = false,
                        BlockCaptionInfo captionInfo = {},
                        bool normalize = false) {
  if (hs.empty()) return;
  std::vector<TH1*> hdraw;

  for (auto h : hs) {
      if (!h) {
          hdraw.push_back(nullptr);
          continue;
      }

      TH1* hclone = (TH1*)h->Clone(Form("%s_clone", h->GetName()));
      hclone->SetDirectory(0);

      if (normalize && hclone->GetSumOfWeights() != 0) {
          hclone->Scale(1.0 / hclone->GetSumOfWeights(), "width");
      }

      hdraw.push_back(hclone);
  }
  std::vector<std::string> lab = labels;
  if (lab.size() != hs.size()) {
    lab.resize(hdraw.size());
    for (size_t i = 0; i < hs.size(); ++i) {
      if (i < labels.size()) lab[i] = labels[i];
      else lab[i] = std::string("hist") + std::to_string(i+1);
    }
  }
  float gap = 0.01;

  TCanvas* c = new TCanvas("c", "canvas", 800, 800);
  c->Divide(1,2,0,0);
  float topPadFrac = 0.68;
  TPad* pad1 = new TPad("pad1","pad1",0,1-topPadFrac+gap,1,1);
  TPad* pad2 = new TPad("pad2","pad2",0,0,1,1-topPadFrac-gap);
  pad1->SetBottomMargin(0.04);
  pad2->SetTopMargin(0.01);
  pad2->SetBottomMargin(0.30);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  if (logx) pad1->SetLogx();
  if (logy) pad1->SetLogy();

  TH1* h0 = hdraw[0];
  if (hdraw.empty() || !hdraw[0]) {
      for (auto h : hdraw) delete h;
      return;
  }
  if (!title.empty()) h0->SetTitle(title.c_str());
  h0->GetXaxis()->SetTitle(xTitle.c_str());
  h0->GetYaxis()->SetTitle(yTitle.c_str());

  if (yMax == -999) {
    double maxy = 0;
    for (auto h : hdraw) if (h) maxy = std::max(maxy, h->GetMaximum());
    if (maxy > 0) h0->SetMaximum(1.25 * maxy);
  }

  double curXmin = h0->GetXaxis()->GetXmin();
  double lastBinEdge = h0->GetXaxis()->GetBinUpEdge(h0->GetNbinsX());
  double newXmin = (xMin != -999) ? xMin : curXmin;
  double newXmax = (xMax != -999) ? std::min(xMax, lastBinEdge) : lastBinEdge;
  h0->GetXaxis()->SetRangeUser(newXmin, newXmax);

  double newYmin = (yMin != -999) ? yMin : 0;
  double newYmax = (yMax != -999) ? yMax : (1.25 * h0->GetMaximum());
  h0->GetYaxis()->SetRangeUser(newYmin, newYmax);

  bool first = true;
  for (auto h : hdraw) {
    if (!h) continue;
    cout << h->GetName()
      << " integral = "
      << h->Integral("width")
      << endl;
    if (first) { h->Draw("E1 HIST"); first = false; }
    else       { h->Draw("E1 HIST SAME"); }
  }

  DrawTLatexLines(captionInfo);

  const size_t n = hdraw.size();
  double ly2 = 0.9;
  double ly1 = ly2 - 0.05 * std::max<size_t>(n, 1);
  if (ly1 < 0.1) ly1 = 0.1;
  TLegend* leg = new TLegend(0.5, ly1, 0.9, ly2);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(132);
  leg->SetTextSize(0.030);
  for (size_t i = 0; i < hs.size(); ++i) {
    if (!hs[i]) continue;
    leg->AddEntry(hdraw[i], lab[i].c_str(), "l");
  }
  leg->Draw();

  pad2->cd();
  if (logx) pad2->SetLogx();

  std::vector<TH1*> ratios;
  for (size_t i = 1; i < hdraw.size(); ++i) {
    if (!hdraw[i]) { ratios.push_back(nullptr); continue; }
    TH1* ratio = (TH1*)hdraw[i]->Clone(Form("%s_ratio", hdraw[i]->GetName()));
    ratio->GetXaxis()->SetRangeUser(newXmin, newXmax);
    ratio->SetDirectory(0);
    ratio->Divide(hdraw[0]);
    std::string ratioLabel = lab[i] + " / " + lab[0];
    ratio->GetYaxis()->SetTitle(ratioLabel.c_str());
    ratio->GetXaxis()->SetTitle(xTitle.c_str());
    ratio->GetYaxis()->SetNdivisions(505);
    ratio->GetYaxis()->SetTitleSize(0.09);
    ratio->GetYaxis()->SetTitleOffset(0.5);
    ratio->GetYaxis()->SetLabelSize(0.08);
    ratio->GetXaxis()->SetTitleSize(0.10);
    ratio->GetXaxis()->SetLabelSize(0.09);
    ratio->GetXaxis()->SetTitleOffset(1.0);
    ratio->SetMinimum(0.5);
    ratio->SetMaximum(1.5);
    ratios.push_back(ratio);
  }

  bool firstRatio = true;
  for (auto r : ratios) {
    if (!r) continue;
    if (firstRatio) { r->Draw("E1 HIST"); firstRatio = false; }
    else            { r->Draw("E1 HIST SAME"); }
  }

  double xmin = newXmin, xmax = newXmax;
  TLine* line = new TLine(xmin, 1.0, xmax, 1.0);
  line->SetLineStyle(2);
  line->SetLineColor(kGray+2);
  line->Draw();

  c->SaveAs(outPng.c_str());

  for (auto r : ratios) delete r;
  for (auto h : hdraw) delete h;
  delete line;
  delete leg;
  delete pad1;
  delete pad2;
  delete c;
}

int drawComparison(){

    cout << "Starting MC vs Data comparison..." << endl;

    // ---------------------------------------------------------------
    // CONFIG: edit these paths/labels to match your 4 pT bins
    // ---------------------------------------------------------------
    string outfolder = "/home/xirong/JetStudiesOO/LynnsCode/Plots/PPRefQAPlots/Comparisons/";
    string dataFolder = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/PPRef/070926PPRefMCData/Test/";
    string mcFolder    = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/PPRef/070926PPRefMCData/Test/";

    std::vector<string> dataFiles = {
      dataFolder + "Data_100.0KEvts_1000evt_30to1000GEV_20260710.root",
    //  dataFolder + "Data_bin2_80to100GEV.root",
   //   dataFolder + "Data_bin3_100to300GEV.root",
    //  dataFolder + "Data_bin4_300to700GEV.root",
    };

    std::vector<string> mcFiles = {
      mcFolder + "MC_100.0KEvts_MC_test_Pt30to1000_30to1000GEV_20260710.root",
   //   mcFolder + "MC_bin2_80to100GEV.root",
   //   mcFolder + "MC_bin3_100to300GEV.root",
   //   mcFolder + "MC_bin4_300to700GEV.root",
    };

    std::vector<string> binLabels = {"bin1"};
    const size_t nBins = dataFiles.size();

    // Histograms to compare (must exist under JetLevelHistRaw in BOTH files)
    std::vector<std::string> obsNames = {"hjtpt", "hcorrectedpt", "hjteta", "hjtphi", "hjtrg", "hjtzg", "hjtkt","hrawpt"};
    std::vector<std::string> xTitles  = {
      "Jet p_{T} (GeV/c)", "Corrected Jet p_{T} (GeV/c)", "Jet #eta", "Jet #phi", "Jet rg", "Jet zg", "Jet kt", "Raw Jet p_{T} (GeV/c)"
    };
    std::vector<std::pair<double,double>> xranges = {
      {-999, 500}, {-999, 500}, {-999, -999}, {-999, -999}, {-999, 0.5}, {-999, -999}, {-999, -999}, {-999, 500}
    };
    std::vector<std::pair<double,double>> yranges = {
      {1e-6, -999}, {1e-6, -999}, {-999, -999}, {-999, -999}, {-999, -999}, {-999, -999}, {-999, -999}, {1e-6, -999}
    };
    std::vector<bool> logy = {true, true, false, false, false, false, false, true};

    // ---------------------------------------------------------------
    // Open all files up front and check they exist
    // ---------------------------------------------------------------
    std::vector<TFile*> fData(nBins), fMC(nBins);
    for (size_t i = 0; i < nBins; ++i) {
      fData[i] = TFile::Open(dataFiles[i].c_str());
      if (!fData[i] || fData[i]->IsZombie()) {
        cout << "ERROR: could not open data file: " << dataFiles[i] << endl;
        return 1;
      }
      fMC[i] = TFile::Open(mcFiles[i].c_str());
      if (!fMC[i] || fMC[i]->IsZombie()) {
        cout << "ERROR: could not open MC file: " << mcFiles[i] << endl;
        return 1;
      }
    }

    // ---------------------------------------------------------------
    // Grab the Cuts TNamed from each data file for the plot captions
    // (adjust the cycle number, e.g. "Cuts;1", if needed for your files)
    // ---------------------------------------------------------------
    std::vector<std::vector<std::string>> captionInfoByBin(nBins);
    for (size_t i = 0; i < nBins; ++i) {
      TNamed* cutInfo = (TNamed*) fData[i]->Get("Cuts");
      if (!cutInfo) {
        cout << "WARNING: no 'Cuts' TNamed found in " << dataFiles[i]
             << " - captions will be empty for this bin." << endl;
        continue;
      }
      std::stringstream ss(std::string(cutInfo->GetTitle()));
      std::string item;
      while (std::getline(ss, item, ';')) captionInfoByBin[i].push_back(item);
    }

    // ---------------------------------------------------------------
    // Load histograms: data[obs][bin], mc[obs][bin]
    // ---------------------------------------------------------------
    std::vector<std::vector<TH1*>> data(obsNames.size(), std::vector<TH1*>(nBins, nullptr));
    std::vector<std::vector<TH1*>> mc(obsNames.size(),   std::vector<TH1*>(nBins, nullptr));

    bool ok = true;
    for (size_t obs = 0; obs < obsNames.size(); ++obs) {
      for (size_t i = 0; i < nBins; ++i) {
        std::string path = "JetLevelHistRaw/" + obsNames[obs];
        data[obs][i] = (TH1*) fData[i]->Get(path.c_str());
        mc[obs][i]   = (TH1*) fMC[i]->Get(path.c_str());

        if (!data[obs][i]) {
          cout << "ERROR: missing " << path << " in data file " << dataFiles[i] << endl;
          ok = false;
        }
        if (!mc[obs][i]) {
          cout << "ERROR: missing " << path << " in MC file " << mcFiles[i] << endl;
          ok = false;
        } else {
          data[obs][i]->SetDirectory(0);
          mc[obs][i]->SetDirectory(0);
        }
      }
    }
    if (!ok) {
      cout << "One or more histograms could not be found - fix paths above and rerun." << endl;
      return 1;
    }

    // Print entry counts for a quick sanity check
    cout << "\nEntries per bin (hjtpt):" << endl;
    for (size_t i = 0; i < nBins; ++i) {
      cout << "  " << binLabels[i]
           << " -- Data: " << data[0][i]->GetEntries()
           << "   MC: "    << mc[0][i]->GetEntries() << endl;
    }

    // ---------------------------------------------------------------
    // hrawpt: loaded separately, since it's compared against
    // hcorrectedpt WITHIN each dataset (raw vs corrected), not
    // Data vs MC like everything else above.
    // ---------------------------------------------------------------
    std::vector<TH1*> dataRawPt(nBins, nullptr), mcRawPt(nBins, nullptr);
    for (size_t i = 0; i < nBins; ++i) {
      dataRawPt[i] = (TH1*) fData[i]->Get("JetLevelHistRaw/hrawpt");
      mcRawPt[i]   = (TH1*) fMC[i]->Get("JetLevelHistRaw/hrawpt");
      if (!dataRawPt[i]) cout << "ERROR: missing JetLevelHistRaw/hrawpt in data file " << dataFiles[i] << endl;
      if (!mcRawPt[i])   cout << "ERROR: missing JetLevelHistRaw/hrawpt in MC file "   << mcFiles[i]   << endl;
      if (dataRawPt[i]) dataRawPt[i]->SetDirectory(0);
      if (mcRawPt[i])   mcRawPt[i]->SetDirectory(0);
    }

    // ---------------------------------------------------------------
    // Style: Data = black points, MC = red points
    // ---------------------------------------------------------------
    for (auto& row : data) for (auto h : row) if (h) StyleHist(h, kBlack, 20);
    for (auto& row : mc)   for (auto h : row) if (h) StyleHist(h, kRed,   21);
    for (auto h : dataRawPt) if (h) StyleHist(h, kBlue+1, 22);
    for (auto h : mcRawPt)   if (h) StyleHist(h, kBlue+1, 22);

    gStyle->SetOptStat(0);

    // ---------------------------------------------------------------
    // Draw Data vs MC with ratio panel for every observable x bin
    // ---------------------------------------------------------------
    for (size_t obs = 0; obs < obsNames.size(); ++obs) {
      for (size_t i = 0; i < nBins; ++i) {
        TH1* hdata = data[obs][i];
        TH1* hMC   = mc[obs][i];
        if (!hdata || !hMC) continue;

        BlockCaptionInfo captionInfo;
        captionInfo.text = captionInfoByBin[i];
        captionInfo.x = 0.25;
        captionInfo.y = 0.85;
        captionInfo.size = 0.030;
        captionInfo.spacing = 0.025;
        captionInfo.useNDC = true;

        DrawAndSaveWithRatio({hdata, hMC},
              {"Data", "MC Reco"},
              outfolder + obsNames[obs] + "_" + binLabels[i] + "_datamc_comp.png",
              xTitles[obs],
              Form("dN/d%s", xTitles[obs].c_str()),
              std::string("MC/Data comparison: ") + xTitles[obs] + " in " + binLabels[i],
              xranges[obs].first, xranges[obs].second,
              yranges[obs].first, yranges[obs].second,
              false, logy[obs],
              captionInfo,
              true);
      }
    }

    // ---------------------------------------------------------------
    // Raw vs Corrected pT, within each dataset (Data and MC separately)
    // ---------------------------------------------------------------
    for (size_t i = 0; i < nBins; ++i) {
      BlockCaptionInfo captionInfo;
      captionInfo.text = captionInfoByBin[i];
      captionInfo.x = 0.25;
      captionInfo.y = 0.85;
      captionInfo.size = 0.030;
      captionInfo.spacing = 0.025;
      captionInfo.useNDC = true;

      TH1* hDataRaw  = dataRawPt[i];
      TH1* hDataCorr = data[1][i]; // index 1 = "hcorrectedpt" in obsNames above
      if (hDataRaw && hDataCorr) {
        DrawAndSaveWithRatio({hDataRaw, hDataCorr},
              {"Raw p_{T}", "Corrected p_{T}"},
              outfolder + "rawVsCorrected_Data_" + binLabels[i] + ".png",
              "Jet p_{T} (GeV/c)",
              "dN/dp_{T}",
              std::string("Data: Raw vs Corrected p_{T} in ") + binLabels[i],
              -999, 500,
              1e-6, -999,
              false, true,
              captionInfo,
              true);
      }

      TH1* hMCRaw  = mcRawPt[i];
      TH1* hMCCorr = mc[1][i]; // index 1 = "hcorrectedpt"
      if (hMCRaw && hMCCorr) {
        DrawAndSaveWithRatio({hMCRaw, hMCCorr},
              {"Raw p_{T}", "Corrected p_{T}"},
              outfolder + "rawVsCorrected_MC_" + binLabels[i] + ".png",
              "Jet p_{T} (GeV/c)",
              "dN/dp_{T}",
              std::string("MC: Raw vs Corrected p_{T} in ") + binLabels[i],
              -999, 500,
              1e-6, -999,
              false, true,
              captionInfo,
              true);
      }
    }

    for (auto f : fData) f->Close();
    for (auto f : fMC)   f->Close();

    cout << "\nDone. Plots saved to " << outfolder << endl;
    return 0;
}

// ---------------------------------------------------------------------
// NOTE on refpt/refeta/genpt/geneta:
// These typically live as TTree branches used to FILL histograms (e.g. in
// a hperformance-style TH2, or dedicated hgenpt/hrefpt TH1s) rather than
// being histograms themselves. If your MC file's JetLevelHistRaw directory
// also contains gen-level histograms (e.g. "hgenpt", "hgeneta"), you can
// compare them to the corresponding reco histograms by adding a second,
// MC-only loop here that calls a single-histogram drawing function (no
// ratio needed since Data has no gen-level equivalent) - let me know if
// you'd like that added.
// ---------------------------------------------------------------------