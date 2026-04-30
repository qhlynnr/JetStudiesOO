#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
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

static void StyleHist(TH1* h, int color, int marker){
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerStyle(marker);
  h->SetLineWidth(2);
}

static void Normalize(TH1* h){
  double integral = h->Integral(0, h->GetNbinsX()+1);
  if (integral > 0) h->Scale(1.0 / integral);
}

static void NormalizePerEvt(TH1* h, double nEvts){
  if (nEvts > 0) h->Scale(1.0 / nEvts);
}

TH1F* Unfold(TH1* hInput,
            int nIterations,
            TH1* hMatchedRecoPt,
            TH1* hMatchedGenPt,
            TH1* hRecoPt,
            TH1* hGenPt,
            RooUnfoldResponse* response_pt) {

    TH1F* hGenResponse = (TH1F*)response_pt->Htruth();
    TH1F* hRecoResponse = (TH1F*)response_pt->Hmeasured();
    TH1F* hPurityPt = (TH1F*)hMatchedRecoPt->Clone("hPurityPt");
    hPurityPt->Divide(hRecoPt); // hMatched/hRecoPt

    TH1F* hEfficiencyPt = (TH1F*)hMatchedGenPt->Clone("hEfficiencyPt");
    hEfficiencyPt->Divide(hGenPt); // hMatched/hGenPt

    TH1F* hPurityCorrectedPt = (TH1F*)hInput->Clone("hPurityCorrectedPt");
    hPurityCorrectedPt->Multiply(hPurityPt); // hRecoPt * (hMatchedPt/hRecoPt) = hMatchedPt

    RooUnfoldBayes unfold(response_pt, hPurityCorrectedPt, nIterations);

    TH1F* hUnfolded = (TH1F*)unfold.Hunfold()->Clone("hUnfolded");
    TH1F* hUnfoldFinal = (TH1F*)hUnfolded->Clone("hUnfoldFinal");
    hUnfoldFinal->Divide(hEfficiencyPt);
    cout << "Unfolding complete!" << endl;

    return hUnfoldFinal;
}
TH1F* Unfold_nocorr(TH1* hInput,
            int nIterations,
            TH1* hMatchedRecoPt,
            TH1* hMatchedGenPt,
            TH1* hRecoPt,
            TH1* hGenPt,
            RooUnfoldResponse* response_pt) {

    TH1F* hGenResponse = (TH1F*)response_pt->Htruth();
    TH1F* hRecoResponse = (TH1F*)response_pt->Hmeasured();
    TH1F* hPurityPt = (TH1F*)hMatchedRecoPt->Clone("hPurityPt");
    hPurityPt->Divide(hRecoPt); // hMatched/hRecoPt

    TH1F* hEfficiencyPt = (TH1F*)hMatchedGenPt->Clone("hEfficiencyPt");
    hEfficiencyPt->Divide(hGenPt); // hMatched/hGenPt

    TH1F* hPurityCorrectedPt = (TH1F*)hInput->Clone("hPurityCorrectedPt");
    hPurityCorrectedPt->Multiply(hPurityPt); // hRecoPt * (hMatchedPt/hRecoPt) = hMatchedPt

    RooUnfoldBayes unfold(response_pt, hPurityCorrectedPt, nIterations);

    TH1F* hUnfolded = (TH1F*)unfold.Hunfold()->Clone("hUnfolded");
   
    return hUnfolded;
}
TH1F* Refold(TH1* hUnfolded,
            TH1* hMatchedRecoPt,
            TH1* hMatchedGenPt,
            TH1* hRecoPt,
            TH1* hGenPt,
            RooUnfoldResponse* response_pt) {

    TH1F* hGenResponse = (TH1F*)response_pt->Htruth();
    TH1F* hRecoResponse = (TH1F*)response_pt->Hmeasured();
    TH1F* hPurityPt = (TH1F*)hMatchedRecoPt->Clone("hPurityPt");
    hPurityPt->Divide(hRecoPt); // hMatched/hRecoPt
    TH1F* hEfficiencyPt = (TH1F*)hMatchedGenPt->Clone("hEfficiencyPt");
    hEfficiencyPt->Divide(hGenPt); // hMatched/hGenPt

    TH1F* hUnfoldedRaw = (TH1F*)hUnfolded->Clone("hUnfoldedRaw");
    hUnfoldedRaw->Multiply(hEfficiencyPt);

    TH1F* hRefolded = (TH1F*)response_pt->ApplyToTruth(hUnfoldedRaw)->Clone("hRefolded");
    TH1F* hRefoldedFinal = (TH1F*)hRefolded->Clone("hRefoldedFinal");
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

    string dataFile = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/0408Unfolding/Response_Data_93.4MEvts_Data_xrd_20260409.root";
    string TrainSet = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/0414HalfHalfTest/Response_MC_1.2MEvts_MC_TrainSet_newbin_test_20260416.root";
    string TestSet = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/0414HalfHalfTest/Response_MC_1.3MEvts_MC_TestSet_newbin_test_20260416.root";
    string outfolder = "/home/xirong/JetStudiesOO/LynnsCode/Plots/Response/0416Unfolding_HalfhalftestDebug/";
    string outFileTag = "5050NewBinTest";
    TFile *dataf = TFile::Open(dataFile.c_str(), "READ");
    CheckFileExist(dataf, dataFile);
    TFile *Trainf = TFile::Open(TrainSet.c_str(), "READ");
    CheckFileExist(Trainf, TrainSet);
    TFile *Testf = TFile::Open(TestSet.c_str(), "READ");
    CheckFileExist(Testf, TestSet);

    TH1F* hDataPt = (TH1F*)dataf->Get("hDataPt");
    CheckFileExist(hDataPt, "hDataPt");
    TH1F* hGenPt = (TH1F*)Trainf->Get("hGenPt");
    CheckFileExist(hGenPt, "hGenPt");
    TH1F* hRecoPt = (TH1F*)Trainf->Get("hRecoPt");
    CheckFileExist(hRecoPt, "hRecoPt");

    TH1F* hFakePt = (TH1F*)Trainf->Get("hFakePt");
    CheckFileExist(hFakePt, "hFakePt");
    TH1F* hMatchedRecoPt = (TH1F*)Trainf->Get("hMatchedRecoPt");
    CheckFileExist(hMatchedRecoPt, "hMatchedRecoPt");
    TH1F* hMatchedGenPt = (TH1F*)Trainf->Get("hMatchedGenPt");
    CheckFileExist(hMatchedGenPt, "hMatchedGenPt");

    TH1F* hTestDataPt = (TH1F*)Testf->Get("hRecoPt");
    CheckFileExist(hTestDataPt, "hTestDataPt");
    TH1F* hTestRecoPt = (TH1F*)Testf->Get("hRecoPt");
    CheckFileExist(hTestRecoPt, "hTestRecoPt");
    TH1F* hTestGenPt = (TH1F*)Testf->Get("hGenPt");
    CheckFileExist(hTestGenPt, "hTestGenPt");

    vector<TH1*> HistsTrain = {hDataPt, hGenPt, hRecoPt};
    vector<TH1*> HistsTrainResponse = {hFakePt, hMatchedRecoPt, hMatchedGenPt};
    vector<TH1*> HistsTest = {hTestDataPt, hTestGenPt, hTestRecoPt};

    for (int i = 1; i<= hGenPt->GetNbinsX(); i++) {
      cout << "Bin" << i << "Gen pt: " << hGenPt->GetBinContent(i) << " | Reco pt: " << hRecoPt->GetBinContent(i) << endl;
    }
    for (auto h : HistsTrain) h->Scale(1.0 / h->GetSumOfWeights());
    for (auto h : HistsTrainResponse) h->Scale(1.0 / h->GetSumOfWeights());
    for (auto h: HistsTest) h->Scale(1.0 / h->GetSumOfWeights());

    RooUnfoldResponse* response_pt = (RooUnfoldResponse*)Trainf->Get("response_pt");
    TH1F* hGenResponse = (TH1F*)response_pt->Htruth();
    TH1F* hRecoResponse = (TH1F*)response_pt->Hmeasured();

    vector<TH1*> hUnfoldedIters;
    vector<string> iterLabels;

// ############ Cut info and general info ############
    TNamed* cutInfo= (TNamed*)Trainf->Get("Cuts");
    TNamed* generalInfo = (TNamed*)Trainf->Get("GeneralInfo");

    cout << "Debug: Cuts applied: " << cutInfo->GetTitle() << endl;
    cout << "Debug: General info: " << generalInfo->GetTitle() << endl;

    vector<std::string> cutInfoVector;
    std::stringstream ssCut(cutInfo->GetTitle());
    std::string item;
    while (std::getline(ssCut, item, ';')) cutInfoVector.push_back(item);

    vector<std::string> generalInfoVector;
    std::stringstream ssGen(generalInfo->GetTitle());
    while (std::getline(ssGen, item, ';')) generalInfoVector.push_back(item);

    vector<std::string> captionLines = {};
    captionLines.push_back(generalInfoVector[0]);

    BlockCaptionInfo captionInfo;
    captionInfo = {captionLines, 0.20, 0.80, 0.03, 0.03, true};

    gStyle->SetOptStat(0); // Disable statistics box
    gStyle->SetPaintTextFormat(".2f");


  // ################### Start Gen Reco Test ###################
    StyleHist(hGenPt, kBlue, 20);
    StyleHist(hTestGenPt, kRed, 20);

    StyleHist(hRecoPt, kBlue, 21);
    StyleHist(hTestRecoPt, kRed, 21);

    DrawAndSave({hGenPt,hTestGenPt},
                {"Gen from Train", "Gen from Test"},
                outfolder + "PtComparison_Gen_" + outFileTag + ".png",
                "p_{T} (GeV/c)",
                "dN/dp_{T}",
                "Gen p_{T} distribution: Gen from Response vs Gen from Test Set",
                30, -999, 1e-6, 5e5, false, true, captionInfo);

    DrawAndSave({hRecoPt,hTestRecoPt},
                {"Reco from Train", "Reco from Test"},
                outfolder + "PtComparison_Reco_" + outFileTag + ".png",
                "p_{T} (GeV/c)",
                "dN/dp_{T}",
                "Reco p_{T} distribution: Reco from Response vs Reco from Test Set",
                30, -999, 1e-6, 5e5, false, true, captionInfo);

    TH1F* ratioReco = (TH1F*)hTestRecoPt->Clone("ratioReco");
    ratioReco->Divide(hRecoPt);
                   
    TH1F* ratioGen = (TH1F*)hTestGenPt->Clone("ratioGen");
    ratioGen->Divide(hGenPt);

    StyleHist(ratioGen, kBlue, 20);
    StyleHist(ratioReco, kRed, 21);

    DrawAndSave({ratioGen, ratioReco},
                {"Gen Test / Gen Train", "Reco Test / Reco Train"},
                outfolder + "PtComparison_Ratio_GenReco" + outFileTag + ".png",
                "p_{T} (GeV/c)",
                "Ratio of Test to Train",
                "p_{T} distribution ratio: Test Set vs Train Set",
                30, -999, 0.5, 1.1, false, false, captionInfo);

    // ################### End Gen Reco Test ###################
    StyleHist(hGenPt, kBlue, 20);
    StyleHist(hTestGenPt, kBlue, 20);
    StyleHist(hRecoPt, kRed, 21);
    StyleHist(hTestRecoPt, kRed, 21);

    int n = 4; // number of iterations to test
    for (int niterations = 1; niterations <= n; ++niterations) {
        cout << "Performing unfolding with " << niterations << " iterations..." << endl;
        TH1F* hUnfoldedIter = Unfold(hTestRecoPt, niterations, hMatchedRecoPt, hMatchedGenPt, hRecoPt, hGenPt, response_pt);
        int col = RainbowColor(niterations, n);
        StyleHist(hUnfoldedIter, col, 20 + (niterations % 5));
        hUnfoldedIters.push_back(hUnfoldedIter);
        iterLabels.push_back("Unfolded (Iter " + to_string(niterations) + ")");
    }

    TH1F* hUnfolded = Unfold(hTestRecoPt, 1, hMatchedRecoPt, hMatchedGenPt, hRecoPt, hGenPt, response_pt);
    TH1F* hUnfoldedTrivial = Unfold(hRecoPt, 1, hMatchedRecoPt, hMatchedGenPt, hRecoPt, hGenPt, response_pt);
    
    TH1F* hRefolded = Refold(hUnfolded, hMatchedRecoPt, hMatchedGenPt, hRecoPt, hGenPt, response_pt);
    TH1F* hRefoldedTrivial = Refold(hUnfoldedTrivial, hMatchedRecoPt, hMatchedGenPt, hRecoPt, hGenPt, response_pt);
  
    StyleHist(hUnfoldedTrivial, kGreen+2, 24);
    StyleHist(hUnfolded, kGreen+2, 24);

    hUnfoldedIters.push_back(hTestGenPt);
    iterLabels.push_back("Gen");
    hUnfoldedIters.push_back(hTestRecoPt);
    iterLabels.push_back("Reco");

    cout << "Size of hUnfoldedIters: " << hUnfoldedIters.size() << endl;
    cout << "Size of iterLabels: " << iterLabels.size() << endl;


    DrawAndSave(hUnfoldedIters,
                iterLabels,
                outfolder + "PtComparison" + outFileTag + "_withiter.png",
                "p_{T} (GeV/c)",
                "dN/dp_{T}",
                "50/50 Test p_{T} distribution: Gen vs Unfolded",
                30, -999, 1e-6, 5e5, true, true, captionInfo);
    DrawAndSave({hGenPt, hRecoPt, hUnfoldedTrivial,hRefoldedTrivial},
                {"Gen", "Reco", "Unfolded (With Fake Corrections)", "Refolded"},
                outfolder + "PtComparison_" + outFileTag + "_TrivialTest.png",
                "p_{T} (GeV/c)",
                "dN/dp_{T}",
                "Trivial Test p_{T} distribution: Gen/Reco from Response vs Unfolded",
                30, -999, 1e-6, 5e5, true, true, captionInfo);

    TH1F* hRatioUnfolded = (TH1F*)hUnfolded->Clone("hRatioUnfolded");
    hRatioUnfolded->Divide(hTestGenPt);
    TH1F* hRatioReco = (TH1F*)hTestRecoPt->Clone("hRatioReco");
    hRatioReco->Divide(hTestGenPt);

    vector<TH1*> hRatioIters;
    vector<string> ratioIterLabels;
    for (int i = 0; i <= n-1; ++i) {
        TH1F* hist = (TH1F*)hUnfoldedIters[i]; // i-1 because iterLabels includes Gen and Reco at the end
        TH1F* hRatio = (TH1F*)hist->Clone("hRatio");
        hRatio->Divide(hTestGenPt);
        StyleHist(hRatio, RainbowColor(i, n), 20 + (i % 5));
        hRatioIters.push_back(hRatio);
        ratioIterLabels.push_back(iterLabels[i] + "/Gen");
    }


    DrawAndSave(hRatioIters,
                ratioIterLabels,
                outfolder + "PtRatio_" + outFileTag + "_withiter.png",
                "p_{T} (GeV/c)",
                "Ratio to Gen",
                "50/50 Test p_{T} ratio: Gen vs Unfolded with iterations",
                30, -999, 0.5, 1.5, true, false, captionInfo);

    TH1F* hRatioUnfoldedTrivial = (TH1F*)hUnfoldedTrivial->Clone("hRatioUnfoldedTrivial");
    hRatioUnfoldedTrivial->Divide(hGenPt);
    TH1F* hRatioRecoTrivial = (TH1F*)hRecoResponse->Clone("hRatioRecoTrivial");
    hRatioRecoTrivial->Divide(hGenPt);

    StyleHist(hRatioRecoTrivial, kRed, 21);
    StyleHist(hRatioUnfoldedTrivial, kGreen+2, 24);

    DrawAndSave({hRatioRecoTrivial, hRatioUnfoldedTrivial},
                {"Reco/Gen", "Unfolded/Gen (With Fake Corrections)"},
                outfolder + "PtRatio_" + outFileTag + "_TrivialTest.png",
                "p_{T} (GeV/c)",
                "Ratio to Gen from Response",
                "Trivial Test p_{T} ratio: Reco and Unfolded to Gen from Response",
                30, -999, 0.5,1.5 , true, false, captionInfo);
    
  
    //Plot Response matrix
    TCanvas* c = new TCanvas("c", "Response Matrix", 800, 800);
    FormatCanvas(c, false, false);
    c->SetLogz();
    response_pt->Hresponse()->SetMinimum(1e-6);
    response_pt->Hresponse()->Draw("COLZ");
    DrawTLatexLines(captionInfo);

    c->SaveAs((outfolder + "ResponseMatrix_pt_logz_"+ outFileTag +".png").c_str());
/*
    TCanvas* c2 = new TCanvas("c2", "Gen Compare", 800, 800);
    FormatCanvas(c2, true, true);
    StyleHist(hGenResponse, kRed, 24);
    StyleHist(hGenPt, kBlue, 20);
    hGenResponse->SetTitle("DEBUG: Gen from Response vs Gen from GenPt hist");
    hGenResponse->Draw("E1 HIST");
    hGenPt->Draw("E1 HIST SAME");
    hGenResponse->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    hGenResponse->GetYaxis()->SetTitle("dN/dp_{T}");
    TLegend* leg = new TLegend(0.5, 0.8, 0.9, 0.9);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(132);
    leg->SetTextSize(0.03);
    leg->AddEntry(hGenResponse, "Gen from Response", "l");
    leg->AddEntry(hGenPt, "Gen from GenPt hist", "l");
    leg->Draw();
    c2->SaveAs((outfolder + "gencomp.png").c_str());
    */
   /* TCanvas* c3 = new TCanvas("c3", "Gen Compare", 800, 800);
    FormatCanvas(c3, true, true);
    StyleHist(hRecoResponse, kRed, 24);
    StyleHist(hRecoPt, kBlue, 20);
    hRecoResponse->SetTitle("DEBUG: Reco from Response vs Reco from GenPt hist");
    hRecoResponse->Draw("E1 HIST");
    hRecoPt->Draw("E1 HIST SAME");
    hRecoResponse->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    hRecoResponse->GetYaxis()->SetTitle("dN/dp_{T}");
    leg = new TLegend(0.5, 0.8, 0.9, 0.9);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(132);
    leg->SetTextSize(0.03);
    leg->AddEntry(hRecoResponse, "Reco from Response", "l");
    leg->AddEntry(hRecoPt, "Reco from GenPt hist", "l");
    leg->Draw();
    c3->SaveAs((outfolder + "recocomp.png").c_str());
    */


      return 0;
}
