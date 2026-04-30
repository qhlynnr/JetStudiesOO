#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TLegend.h>
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include <TStyle.h>
#include <TColor.h>

struct BlockCaptionInfo{
  std::vector<std::string> text;
  double x;
  double y;
  double size;
  double spacing;
  bool useNDC;
};
static void FormatCanvas(TCanvas* c, bool logx, bool logy) {
  c->SetTicks(1,1);
  c->SetLeftMargin(0.17);
  c->SetBottomMargin(0.15);
  c->SetTopMargin(0.08);
  c->SetRightMargin(0.15);

  if (logx) c->SetLogx();
  if (logy) c->SetLogy();
}
static void DrawTLatexLines(const BlockCaptionInfo& info)
{
  TLatex lat;
  lat.SetNDC(info.useNDC);
  lat.SetTextSize(info.size);
  double yy = info.y;
  for (const auto& s : info.text) {
    lat.SetTextFont(132);  // Helvetica (normal, not bold)
    lat.DrawLatex(info.x, yy, s.c_str());
    yy -= info.spacing;
  }
}

static void DrawAndSave(const std::vector<TH1*>& hs,
                        const std::vector<std::string>& labels,
                        const std::string& outPng,
                        const std::string& xTitle,
                        const std::string& yTitle = "Normalized entries",
                        const std::string& title = "",
                        float xMin = -999, float xMax = -999,
                        float yMin = -999, float yMax = -999,
                        bool logx = false,
                        bool logy = false,
                        BlockCaptionInfo captionInfo = {}) {
  if (hs.empty()) return;

  // basic sanity on labels
  std::vector<std::string> lab = labels;
  if (lab.size() != hs.size()) {
    lab.resize(hs.size());
    for (size_t i = 0; i < hs.size(); ++i) {
      if (i < labels.size()) lab[i] = labels[i];
      else lab[i] = std::string("hist") + std::to_string(i+1);
    }
  }

  TCanvas* c = new TCanvas("c", "canvas", 800, 800);
  c->SetTicks(1,1);
  c->SetLeftMargin(0.14);
  c->SetBottomMargin(0.12);
  c->SetTopMargin(0.08);
  c->SetRightMargin(0.05);

  if (logx) c->SetLogx();
  if (logy) c->SetLogy();

  // set titles on first histogram
  TH1* h0 = hs[0];
  if (!title.empty()) h0->SetTitle(title.c_str());
  h0->GetXaxis()->SetTitle(xTitle.c_str());
  h0->GetYaxis()->SetTitle(yTitle.c_str());

  // determine y range using all histograms
  double maxy = 0;
  for (auto h : hs) if (h) maxy = std::max(maxy, h->GetMaximum());
  if (maxy > 0) h0->SetMaximum(1.25 * maxy);

  // allow setting x/y min and max independently (use current axis limits when one side is left as -999)
  double curXmin = h0->GetXaxis()->GetXmin();
  double curXmax = h0->GetXaxis()->GetXmax();
  double newXmin = (xMin != -999) ? xMin : curXmin;
  double newXmax = (xMax != -999) ? xMax : curXmax;
  h0->GetXaxis()->SetRangeUser(newXmin, newXmax);

  double curYmin = h0->GetYaxis()->GetXmin();
  double curYmax = h0->GetYaxis()->GetXmax();
  double newYmin = (yMin != -999) ? yMin : curYmin;
  double newYmax = (yMax != -999) ? yMax : curYmax;
  h0->GetYaxis()->SetRangeUser(newYmin, newYmax);
  // draw histograms (assume they are already styled)
  bool first = true;
  for (auto h : hs) {
    if (!h) continue;
    if (first) { h->Draw("E1 HIST"); first = false; }
    else       { h->Draw("E1 HIST SAME"); }
  }

  DrawTLatexLines(captionInfo);
  // legend placement: adapt height to number of entries
  const size_t n = hs.size();
  double ly2 = 0.9;
  double ly1 = ly2 - 0.03 * std::max<size_t>(n, 1);
  if (ly1 < 0.1) ly1 = 0.1;
  TLegend* leg = new TLegend(0.5, ly1, 0.9, ly2);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(132);
  leg->SetTextSize(0.020);
  for (size_t i = 0; i < hs.size(); ++i) {
    if (!hs[i]) continue;
    leg->AddEntry(hs[i], lab[i].c_str(), "l");
  }
  leg->Draw();
  c->SaveAs(outPng.c_str());
}

TH1D* CreatePlotHist(TH1* original, const string& name, bool normalizeByWidth = true) {
    TH1D* plotHist = (TH1D*)original->Clone(name.c_str());
    if (normalizeByWidth) {
        plotHist->Scale(1.0 / plotHist->GetSumOfWeights(), "width");
    } else {
        plotHist->Scale(1.0 / plotHist->GetSumOfWeights());
    }
    return plotHist;
}



void DrawAndSaveNormalized(const std::vector<TH1*>& originals,
                          const std::vector<std::string>& labels,
                          const std::string& outPng,
                          const std::string& xTitle,
                          const std::string& yTitle = "Normalized entries",
                          const std::string& title = "",
                          float xMin = -999, float xMax = -999,
                          float yMin = -999, float yMax = -999,
                          bool logx = false,
                          bool logy = false,
                          bool normalizeByWidth = true,
                          BlockCaptionInfo captionInfo = {}) {
    
    std::vector<TH1*> plotHists;
    for (size_t i = 0; i < originals.size(); ++i) {
        if (originals[i]) {
            string plotName = originals[i]->GetName() + string("_plot_") + to_string(i);
            TH1D* plotHist = CreatePlotHist(originals[i], plotName, normalizeByWidth);
            plotHists.push_back(plotHist);
        } else {
            plotHists.push_back(nullptr);
        }
    }
    
    DrawAndSave(plotHists, labels, outPng, xTitle, yTitle, title, 
                xMin, xMax, yMin, yMax, logx, logy, captionInfo);
    
    // Cleanup
    for (auto* h : plotHists) {
        if (h) delete h;
    }
}

static void StyleHist(TH1* h, int color, int marker = 20, int lineStyle = 1){
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerStyle(marker);
  h->SetLineWidth(2);
  h->SetLineStyle(lineStyle);
}

TH1D* Unfold(TH1* hInput,
            int nIterations,
            TH1* hMatchedRecoPt,
            TH1* hMatchedGenPt,
            TH1* hRecoPt,
            TH1* hGenPt,
            RooUnfoldResponse* response_pt) {

    TH1D* hGenResponse = (TH1D*)response_pt->Htruth();
    TH1D* hRecoResponse = (TH1D*)response_pt->Hmeasured();
    TH1D* hPurityPt = (TH1D*)hMatchedRecoPt->Clone("hPurityPt");
    hPurityPt->Divide(hRecoPt); // hMatched/hRecoPt

    TH1D* hEfficiencyPt = (TH1D*)hMatchedGenPt->Clone("hEfficiencyPt");
    hEfficiencyPt->Divide(hGenPt); // hMatched/hGenPt ()
 //   hEfficiencyPt->Print("all");

    TH1D* hPurityCorrectedPt = (TH1D*)hInput->Clone("hPurityCorrectedPt");
    hPurityCorrectedPt->Multiply(hPurityPt); // hRecoPt * (hMatchedPt/hRecoPt) = hMatchedPt

    RooUnfoldBayes unfold(response_pt, hPurityCorrectedPt, nIterations);

    TH1D* hUnfolded = (TH1D*)unfold.Hreco()->Clone("hUnfolded");
    TH1D* hUnfoldFinal = (TH1D*)hUnfolded->Clone("hUnfoldFinal");
    hUnfoldFinal->Divide(hEfficiencyPt);
    cout << "Unfolding complete!" << endl;

    return hUnfoldFinal;
}


TH2D* Unfold_nocorr_2D(TH2D* hInput,
            int nIterations,
            RooUnfoldResponse* response_pt) {

    RooUnfoldBayes unfold(response_pt, hInput, nIterations);
    TH2D* hUnfolded = (TH2D*)unfold.Hreco()->Clone("hUnfolded");
   
    return hUnfolded;
}


TH1D* Refold(TH1* hUnfolded,
            TH1* hMatchedRecoPt,
            TH1* hMatchedGenPt,
            TH1* hRecoPt,
            TH1* hGenPt,
            RooUnfoldResponse* response_pt) {

    TH1D* hGenResponse = (TH1D*)response_pt->Htruth();
    TH1D* hRecoResponse = (TH1D*)response_pt->Hmeasured();
    TH1D* hPurityPt = (TH1D*)hMatchedRecoPt->Clone("hPurityPt");
    hPurityPt->Divide(hRecoPt); // hMatched/hRecoPt
    TH1D* hEfficiencyPt = (TH1D*)hMatchedGenPt->Clone("hEfficiencyPt");
    hEfficiencyPt->Divide(hGenPt); // hMatched/hGenPt

    TH1D* hUnfoldedRaw = (TH1D*)hUnfolded->Clone("hUnfoldedRaw");
    hUnfoldedRaw->Multiply(hEfficiencyPt);

    TH1D* hRefolded = (TH1D*)response_pt->ApplyToTruth(hUnfoldedRaw)->Clone("hRefolded");
    TH1D* hRefoldedFinal = (TH1D*)hRefolded->Clone("hRefoldedFinal");
    hRefoldedFinal->Divide(hPurityPt);

    return hRefoldedFinal;
}
template <typename T>
void CheckFileExist(T* obj, const std::string& filename = "") {
    if (!obj) {
        cout << "Error: Could not open file" << filename << endl;
        exit(1);
    }
}

static int RainbowColor(int i, int n) {
  double hue = (n == 1) ? 0.0 : (double)i / (double)(n);

  Float_t r, g, b;

  TColor::HLStoRGB(hue * 360.0, 0.5, 0.5, r, g, b);
  return TColor::GetColor(r, g, b);
}

int PlotUnfolding(){
    gStyle->SetOptStat(0);

    string MCFile = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/0429RooUnfold2D/Response_MC_145KEvts_MC2D_1file_debug_20260430.root";
    string outfolder = "/home/xirong/JetStudiesOO/LynnsCode/Plots/Response/0428UnfoldingRooUnfold2";
    string outFileTag = "5050test_FullPD";
    TFile *MCFilef = TFile::Open(MCFile.c_str(), "READ");
    CheckFileExist(MCFilef, MCFile);
    TH2D* hGenPt_rg = (TH2D*)MCFilef->Get("hGenPt_rg");
    CheckFileExist(hGenPt_rg, "hGenPt_rg");
    TH2D* hRecoPt_rg = (TH2D*)MCFilef->Get("hRecoPt_rg");
    CheckFileExist(hRecoPt_rg, "hRecoPt_rg");
    TH2D* hMatchedRecoPt_rg = (TH2D*)MCFilef->Get("hMatchedRecoPt_rg");
    CheckFileExist(hMatchedRecoPt_rg, "hMatchedRecoPt_rg");
    TH2D* hMatchedGenPt_rg = (TH2D*)MCFilef->Get("hMatchedGenPt_rg");
    CheckFileExist(hMatchedGenPt_rg, "hMatchedGenPt_rg");
    TH2D* hGenPt_rg2 = (TH2D*)MCFilef->Get("hGenPt_rg2");
    CheckFileExist(hGenPt_rg2, "hGenPt_rg2");
    TH2D* hRecoPt_rg2 = (TH2D*)MCFilef->Get("hRecoPt_rg2");
    CheckFileExist(hRecoPt_rg2, "hRecoPt_rg2");
    TH2D* hMatchedRecoPt_rg2 = (TH2D*)MCFilef->Get("hMatchedRecoPt_rg2");
    CheckFileExist(hMatchedRecoPt_rg2, "hMatchedRecoPt_rg2");
    TH2D* hMatchedGenPt_rg2 = (TH2D*)MCFilef->Get("hMatchedGenPt_rg2");
    CheckFileExist(hMatchedGenPt_rg2, "hMatchedGenPt_rg2");

    RooUnfoldResponse* response_pt_rg = (RooUnfoldResponse*)MCFilef->Get("response_pt_rg");
    TH2D* hUnfolded_trivial_2D = Unfold_nocorr_2D(hRecoPt_rg, 1, response_pt_rg);
   
    TH2D* hRecoResponse2D = (TH2D*)response_pt_rg->Hmeasured();
    TH2D* hGenResponse2D  = (TH2D*)response_pt_rg->Htruth();

    RooUnfoldBayes unfold2D(response_pt_rg, hRecoResponse2D, 1);
    TH2D* hUnfolded_trivial2D = (TH2D*)unfold2D.Hreco();


    // Plot 1: Trivial 2D 

    cout << "Trivial 2D Unfolding ratio (Unfolded / Gen) per bin:" << endl;
    for (int ix = 1; ix <= hUnfolded_trivial2D->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= hUnfolded_trivial2D->GetNbinsY(); ++iy) {
            float unfolded = hUnfolded_trivial2D->GetBinContent(ix, iy);
            float gen_val  = hGenResponse2D->GetBinContent(ix, iy);
            float gen_val2 = hMatchedGenPt_rg->GetBinContent(ix, iy);
            if (gen_val > 0)
                cout << Form("Bin (%d,%d): Ratio = %.6f", ix, iy, unfolded/gen_val) << endl;
                cout << Form("Bin (%d,%d): Ratio (MatchedGen2) = %.6f", ix, iy, (gen_val2 > 0) ? unfolded/gen_val2 : 0) << endl;
        }
    }


    
    return 0;
}
