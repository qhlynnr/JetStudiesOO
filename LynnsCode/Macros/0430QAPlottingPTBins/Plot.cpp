#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>

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

  if (yMax == -999) {
    double maxy = 0;
    for (auto h : hs) if (h) maxy = std::max(maxy, h->GetMaximum());
    if (maxy > 0) h0->SetMaximum(1.25 * maxy);
  }

  // Allow setting x/y min and max independently
// Allow setting x/y min and max independently
  double curXmin = h0->GetXaxis()->GetXmin();
  double curXmax = h0->GetXaxis()->GetXmax();
  double newXmin = (xMin != -999) ? xMin : curXmin;
  double newXmax = (xMax != -999) ? xMax : curXmax;
  h0->GetXaxis()->SetRangeUser(newXmin, newXmax);

  double newYmin = (yMin != -999) ? yMin : 0;
  double newYmax = (yMax != -999) ? yMax : (1.25 * h0->GetMaximum());
  h0->GetYaxis()->SetRangeUser(newYmin, newYmax);
  cout << "Xtitle: " << xTitle << endl;
  cout << "Set x range: [" << newXmin << ", " << newXmax << "]" << endl;
  cout << "Set y range: [" << newYmin << ", " << newYmax << "]" << endl;
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
  double ly1 = ly2 - 0.05 * std::max<size_t>(n, 1);
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

static void DrawStack(const std::vector<TH1*>& hs,
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

  THStack *hsStack = new THStack("hsStack", title.c_str());
  for (auto h : hs) {
    if (!h) continue;
    hsStack->Add(h);
  }

  hsStack->SetTitle(title.c_str());
  hsStack->Draw("HIST");

  hsStack->GetXaxis()->SetTitle(xTitle.c_str());
  hsStack->GetYaxis()->SetTitle(yTitle.c_str());
  if (xMin != -999 && xMax != -999) hsStack->GetXaxis()->SetRangeUser(xMin, xMax);
  if (yMin != -999 && yMax != -999) hsStack->GetYaxis()->SetRangeUser(yMin, yMax);

  
  DrawTLatexLines(captionInfo);
  // legend placement: adapt height to number of entries
  const size_t n = hs.size();
  double ly2 = 0.9;
  double ly1 = ly2 - 0.05 * std::max<size_t>(n, 1);
  if (ly1 < 0.1) ly1 = 0.1;
  TLegend* leg = new TLegend(0.5, ly1, 0.9, ly2);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(132);
  leg->SetTextSize(0.035);
  for (size_t i = 0; i < hs.size(); ++i) {
    if (!hs[i]) continue;
    leg->AddEntry(hs[i], lab[i].c_str(), "f");
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


int Plot(){
    cout << "Starting QA plot comparison..." << endl;
    string outfolder = "/home/xirong/JetStudiesOO/LynnsCode/Plots/QAPlots/0430PtBinDebug/NewBin/";

    string inputFileFolder = "/home/xirong/JetStudiesOO/LynnsCode/RootFiles/0430QAPlotDiffptBins/Debug/";
    string inputFileMCbin1 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_80to140GEV_20260502.root";
    string inputFileMCbin2 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_140to200GEV_20260502.root";
    string inputFileMCbin3 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_200to300GEV_20260502.root";
    string inputFileMCbin4 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_300to400GEV_20260502.root";
    string inputFileMCbin5 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_400to500GEV_20260502.root";
   // string inputFileMCbin6 = inputFileFolder + "MC_99.7MEvts_1PD_xrd_test_400to500GEV_20260502.root";

    string inputFileDatabin1 = inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_80to140GEV_20260502.root";
    string inputFileDatabin2 = inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_140to200GEV_20260502.root";
    string inputFileDatabin3 = inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_200to300GEV_20260502.root";
    string inputFileDatabin4= inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_300to400GEV_20260502.root";
    string inputFileDatabin5 = inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_400to500GEV_20260502.root";
   // string inputFileDatabin6 = inputFileFolder + "Data_93.4MEvts_1PD_xrd_test_400to500GEV_20260502.root";

    cout << "reading files: " << endl;

      vector<string> filesToRead = {inputFileMCbin1, inputFileMCbin2, inputFileMCbin3, inputFileMCbin4, inputFileMCbin5,
                                    inputFileDatabin1, inputFileDatabin2, inputFileDatabin3, inputFileDatabin4, inputFileDatabin5};

    for (const auto& f : filesToRead){
      if (!TFile::Open(f.c_str())) {
        cout << "Error opening file: " << f << endl;
        return 1;
      } else {
        cout << "Successfully opened file: " << f << endl;
      }
    }

    TFile* fMCbin1 = TFile::Open(inputFileMCbin1.c_str());
    TFile* fMCbin2 = TFile::Open(inputFileMCbin2.c_str());
    TFile* fMCbin3 = TFile::Open(inputFileMCbin3.c_str());
    TFile* fMCbin4 = TFile::Open(inputFileMCbin4.c_str());
    TFile* fMCbin5 = TFile::Open(inputFileMCbin5.c_str());
    //TFile* fMCbin6 = TFile::Open(inputFileMCbin6.c_str());
    TFile* fDatabin1 = TFile::Open(inputFileDatabin1.c_str());
    TFile* fDatabin2 = TFile::Open(inputFileDatabin2.c_str());
    TFile* fDatabin3 = TFile::Open(inputFileDatabin3.c_str());
    TFile* fDatabin4 = TFile::Open(inputFileDatabin4.c_str());
    TFile* fDatabin5 = TFile::Open(inputFileDatabin5.c_str());
    //TFile* fDatabin6 = TFile::Open(inputFileDatabin6.c_str());

    TNamed* cutInfoDataBin1 = (TNamed*)fDatabin1->Get("Cuts");
    TNamed* cutInfoDataBin2 = (TNamed*)fDatabin2->Get("Cuts");
    TNamed* cutInfoDataBin3 = (TNamed*)fDatabin3->Get("Cuts");
    TNamed* cutInfoDataBin4 = (TNamed*)fDatabin4->Get("Cuts");
    TNamed* cutInfoDataBin5 = (TNamed*)fDatabin5->Get("Cuts");
    //TNamed* cutInfoDataBin6 = (TNamed*)fDatabin6->Get("Cuts");

    if (!cutInfoDataBin1 || !cutInfoDataBin2 || !cutInfoDataBin3 || !cutInfoDataBin4 || !cutInfoDataBin5) {
        cout << "Error: Could not retrieve 'Cuts' TNamed from one of the data files." << endl;
        return 1;
    }
    TH1F* hjtpt_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin1 = (TH1F*)fDatabin1->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_databin1 || !hjteta_databin1 || !hjtphi_databin1 || !hjty_databin1 || !hjtrg_databin1 ||
        !hjtzg_databin1 || !hjtkt_databin1 || !hjtangu_databin1) {
        cout << "Bin1 Error: Could not retrieve one of the expected histograms from databin1." << endl;
        return 1;
    }
    TH1F* hjtpt_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin2 = (TH1F*)fDatabin2->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_databin2 || !hjteta_databin2 || !hjtphi_databin2 || !hjty_databin2 || !hjtrg_databin2 ||
        !hjtzg_databin2 || !hjtkt_databin2 || !hjtangu_databin2) {
        cout << "Bin2 Error: Could not retrieve one of the expected histograms from databin2." << endl;
        return 1;
    }
    TH1F* hjtpt_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin3 = (TH1F*)fDatabin3->Get("JetLevelHistRaw/hjtangu");  

    if (!hjtpt_databin3 || !hjteta_databin3 || !hjtphi_databin3 || !hjty_databin3 || !hjtrg_databin3 ||
        !hjtzg_databin3 || !hjtkt_databin3 || !hjtangu_databin3) {
        cout << "Bin3 Error: Could not retrieve one of the expected histograms from databin3." << endl;
        return 1;
    }
    TH1F* hjtpt_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin4 = (TH1F*)fDatabin4->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_databin4 || !hjteta_databin4 || !hjtphi_databin4 || !hjty_databin4 || !hjtrg_databin4 ||
        !hjtzg_databin4 || !hjtkt_databin4 || !hjtangu_databin4) {
        cout << "Bin4 Error: Could not retrieve one of the expected histograms from databin4." << endl;
        return 1;
    }
    TH1F* hjtpt_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin5 = (TH1F*)fDatabin5->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_databin5 || !hjteta_databin5 || !hjtphi_databin5 || !hjty_databin5 || !hjtrg_databin5 ||
        !hjtzg_databin5 || !hjtkt_databin5 || !hjtangu_databin5) {
        cout << "Bin5 Error: Could not retrieve one of the expected histograms from databin5." << endl;
        return 1;
    }

    /*TH1F* hjtpt_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjteta");        
    TH1F* hjtphi_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtzg");  
    TH1F* hjtkt_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_databin6 = (TH1F*)fDatabin6->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_databin6 || !hjteta_databin6 || !hjtphi_databin6 || !hjty_databin6 || !hjtrg_databin6 ||
        !hjtzg_databin6 || !hjtkt_databin6 || !hjtangu_databin6) {
        cout << "Bin6 Error: Could not retrieve one of the expected histograms from databin6." << endl;
        return 1;
    }*/

    TH1F* hjtpt_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_MCbin1 = (TH1F*)fMCbin1->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_MCbin1 || !hjteta_MCbin1 || !hjtphi_MCbin1 || !hjty_MCbin1 || !hjtrg_MCbin1 ||
        !hjtzg_MCbin1 || !hjtkt_MCbin1 || !hjtangu_MCbin1) {
        cout << "MC Bin1 Error: Could not retrieve one of the expected histograms from MCbin1." << endl;
        return 1;
    }
    TH1F* hjtpt_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_MCbin2 = (TH1F*)fMCbin2->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_MCbin2 || !hjteta_MCbin2 || !hjtphi_MCbin2 || !hjty_MCbin2 || !hjtrg_MCbin2 ||
        !hjtzg_MCbin2 || !hjtkt_MCbin2 || !hjtangu_MCbin2) {
        cout << "MC Bin2 Error: Could not retrieve one of the expected histograms from MCbin2." << endl;
        return 1;
    }
    TH1F* hjtpt_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_MCbin3 = (TH1F*)fMCbin3->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_MCbin3 || !hjteta_MCbin3 || !hjtphi_MCbin3 || !hjty_MCbin3 || !hjtrg_MCbin3 ||
        !hjtzg_MCbin3 || !hjtkt_MCbin3 || !hjtangu_MCbin3) {
        cout << "MC Bin3 Error: Could not retrieve one of the expected histograms from MCbin3." << endl;
        return 1;
    }
    TH1F* hjtpt_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_MCbin4 = (TH1F*)fMCbin4->Get("JetLevelHistRaw/hjtangu");

    if (!hjtpt_MCbin4 || !hjteta_MCbin4 || !hjtphi_MCbin4 || !hjty_MCbin4 || !hjtrg_MCbin4 ||
        !hjtzg_MCbin4 || !hjtkt_MCbin4 || !hjtangu_MCbin4) {
        cout << "MC Bin4 Error: Could not retrieve one of the expected histograms from MCbin4." << endl;
        return 1;
    }
    TH1F* hjtpt_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtpt");
    TH1F* hjteta_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjteta");
    TH1F* hjtphi_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtphi");
    TH1F* hjty_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjty");
    TH1F* hjtrg_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtrg");
    TH1F* hjtzg_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtzg");
    TH1F* hjtkt_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtkt");
    TH1F* hjtangu_MCbin5 = (TH1F*)fMCbin5->Get("JetLevelHistRaw/hjtangu");
    if (!hjtpt_MCbin5 || !hjteta_MCbin5 || !hjtphi_MCbin5 || !hjty_MCbin5 || !hjtrg_MCbin5 ||
        !hjtzg_MCbin5 || !hjtkt_MCbin5 || !hjtangu_MCbin5) {
        cout << "MC Bin5 Error: Could not retrieve one of the expected histograms from MCbin5." << endl;
        return 1;
    }

    TNamed* generalInfoDatabin1 = (TNamed*)fDatabin1->Get("GeneralInfo");
    TNamed* generalInfoDatabin2 = (TNamed*)fDatabin2->Get("GeneralInfo");
    TNamed* generalInfoDatabin3 = (TNamed*)fDatabin3->Get("GeneralInfo");
    TNamed* generalInfoDatabin4 = (TNamed*)fDatabin4->Get("GeneralInfo");
    TNamed* generalInfoDatabin5 = (TNamed*)fDatabin5->Get("GeneralInfo");

    std::vector<std::vector<TH1*>> data = {
      {hjtpt_databin1, hjtpt_databin2, hjtpt_databin3, hjtpt_databin4, hjtpt_databin5},
      {hjteta_databin1, hjteta_databin2, hjteta_databin3, hjteta_databin4, hjteta_databin5},
      {hjtphi_databin1, hjtphi_databin2, hjtphi_databin3, hjtphi_databin4, hjtphi_databin5},
      {hjty_databin1, hjty_databin2, hjty_databin3, hjty_databin4, hjty_databin5},
      {hjtrg_databin1, hjtrg_databin2, hjtrg_databin3, hjtrg_databin4, hjtrg_databin5},
      {hjtzg_databin1, hjtzg_databin2, hjtzg_databin3, hjtzg_databin4, hjtzg_databin5},
      {hjtkt_databin1, hjtkt_databin2, hjtkt_databin3, hjtkt_databin4, hjtkt_databin5},
      {hjtangu_databin1, hjtangu_databin2, hjtangu_databin3, hjtangu_databin4, hjtangu_databin5}
    };
      std::vector<std::vector<TH1*>> mc = {
      {hjtpt_MCbin1, hjtpt_MCbin2, hjtpt_MCbin3, hjtpt_MCbin4, hjtpt_MCbin5},
      {hjteta_MCbin1, hjteta_MCbin2, hjteta_MCbin3, hjteta_MCbin4, hjteta_MCbin5},
      {hjtphi_MCbin1, hjtphi_MCbin2, hjtphi_MCbin3, hjtphi_MCbin4, hjtphi_MCbin5},
      {hjty_MCbin1, hjty_MCbin2, hjty_MCbin3, hjty_MCbin4, hjty_MCbin5},
      {hjtrg_MCbin1, hjtrg_MCbin2, hjtrg_MCbin3, hjtrg_MCbin4, hjtrg_MCbin5},
      {hjtzg_MCbin1, hjtzg_MCbin2, hjtzg_MCbin3, hjtzg_MCbin4, hjtzg_MCbin5},
      {hjtkt_MCbin1, hjtkt_MCbin2, hjtkt_MCbin3, hjtkt_MCbin4, hjtkt_MCbin5},
      {hjtangu_MCbin1, hjtangu_MCbin2, hjtangu_MCbin3, hjtangu_MCbin4, hjtangu_MCbin5}
    };
        cout << "Number of events in each data bin: " << endl;
    for (size_t i = 0; i < data[0].size(); ++i) {
      if (data[0][i]) cout << "Data Bin " << i+1 << ": " << data[0][i]->Integral() << " entries" << endl;
    }
    cout << "Number of events in each MC bin: " << endl;
    for (size_t i = 0; i < mc[0].size(); ++i) {
      if (mc[0][i]) cout << "MC Bin " << i + 1 << ": " << mc[0][i]->Integral() << " entries" << endl;
    }


    vector<TNamed*> cutInfoTNamedVector = {cutInfoDataBin1, cutInfoDataBin2, cutInfoDataBin3, cutInfoDataBin4, cutInfoDataBin5};
    vector<TNamed*> generalInfoTNamedVector = {generalInfoDatabin1, generalInfoDatabin2, generalInfoDatabin3, generalInfoDatabin4, generalInfoDatabin5};

      for (size_t i = 0; i < data.size(); ++i) {
      for (size_t j = 0; j < data[i].size(); ++j) {
        TH1* h = data[i][j];
        StyleHist(h, kBlack, 20);
      }
    }
    for (size_t i = 0; i < mc.size(); ++i) {
      for (size_t j = 0; j < mc[i].size(); ++j) {
        TH1* h = mc[i][j];
        StyleHist(h, kRed, 21);
      }
    }

    gStyle->SetOptStat(0); // Disable statistics box

    vector<vector<std::string>> captionInfoByBin;

    for (size_t i = 0; i < cutInfoTNamedVector.size(); ++i) {
      BlockCaptionInfo captionInfo;
      vector<std::string> cutInfoDataVector;
      vector<std::string> generalInfoDataVector;

      string cutInfoTitle = cutInfoTNamedVector[i]->GetTitle();
      string generalInfoTitle = generalInfoTNamedVector[i]->GetTitle();

      std::stringstream ssCut(cutInfoTitle);
      std::string item;
      while (std::getline(ssCut, item, ';')) cutInfoDataVector.push_back(item);

      std::stringstream ssGen(generalInfoTitle);
      while (std::getline(ssGen, item, ';')) generalInfoDataVector.push_back(item);

      cutInfoDataVector.push_back(generalInfoDataVector[8]);
      captionInfoByBin.push_back(cutInfoDataVector);
    }

    // Debug Caption Info
  //  for (const auto& s : generalInfoDataVector) cout << "General Info: " << s << endl;


   /* captionLines.push_back(generalInfoDataVector[4]);
    captionLines.push_back(generalInfoDataVector[5]);
    captionLines.push_back(generalInfoDataVector[7]);
    captionLines.push_back(generalInfoDataVector[8]);*/

    /*************************************************************************
     *                                                                       *
     *                      MC, Data Comparison                              *
     *                                                                       *
     *************************************************************************/

    std::vector<std::string> obsNames = {"hjtpt", 
      "hjteta", 
      "hjtphi", 
      "hjty", 
      "hjtrg", 
      "hjtzg", 
      "hjtkt", 
      "hjtangu"};

    std::vector<std::string> xTitles = {
      "Jet p_{T} (GeV/c)",
      "Jet #eta",
      "Jet #phi",
      "Jet y",
      "Jet rg",
      "Jet zg",
      "Jet kt",
      "Jet angularity"
    };
    
    std::vector<std::pair<float,float>> xranges = {
      {0, 400},    // pt
      {-2.5f, 2.5f},// eta
      {-3.2f, 3.2f},// phi
      {-2.5f, 2.5f},// y
      {0, 1.0f},   // rg
      {0, 1.0f},   // zg
      {0, 100},    // kt
      {0, 1} // angularity: auto
    };

    std::vector<std::pair<float,float>> yranges = {
      {10e-2, -999},    // pt
      {-999, -999},    // eta
      {-999, -999},    // phi
      {-999, -999},    // y
      {-999, -999},   // rg
      {-999, -999},   // zg
      {-999, -999},   // kt
      {-999, -999}    // angularity: auto
    };

    vector<bool> logy = {true, false, false, false, false, false, false, false};
    
    for (size_t obs = 0; obs < data.size(); ++obs) {
      std::vector<TH1*> h_data_bins = data[obs];
      std::vector<TH1*> h_mc_bins   = mc[obs];
      for (size_t j = 0; j < h_data_bins.size() && j < h_mc_bins.size(); ++j) {
        BlockCaptionInfo captionInfo;
        captionInfo.text = captionInfoByBin[j];
        captionInfo.x = 0.70;
        captionInfo.y = 0.85;
        captionInfo.size = 0.020;
        captionInfo.spacing = 0.025;
        captionInfo.useNDC = true;

        string binname = "bin" + to_string(j);
        TH1* hdata = h_data_bins[j];
        TH1* hMC = h_mc_bins[j];
        if (!hdata || !hMC) continue;

        DrawAndSave({hdata, hMC},
              {"Data", "MC Reco"},
              outfolder + obsNames[obs] + "_" + binname + "_datamc_comp.png",
              xTitles[obs],
              Form("dN/d%s", xTitles[obs].c_str()),
              std::string("MC Data comparison ") + xTitles[obs] + " in " + binname,
              xranges[obs].first, xranges[obs].second,
              yranges[obs].first, yranges[obs].second,
              false, logy[obs],
              captionInfo);
        TH1F* ratio = (TH1F*)hMC->Clone(Form("%s_ratio", hMC->GetName()));
        ratio->Divide(hdata);
        DrawAndSave({ratio},
              {"MC/Data"},
              outfolder + obsNames[obs] + "_" + binname + "_datamc_ratio.png",
              xTitles[obs],
              Form("MC/Data"),
              std::string("MC/Data ratio ") + xTitles[obs] + " in " + binname,
              xranges[obs].first, xranges[obs].second,
              0.5, 1.5,
              false, false,
              captionInfo);
      }

    }  

    fDatabin1->Close();
    fDatabin2->Close();
    fDatabin3->Close();
    fDatabin4->Close();
    fDatabin5->Close();
    fMCbin1->Close();
    fMCbin2->Close();
    fMCbin3->Close();
    fMCbin4->Close();
    fMCbin5->Close();

    return 0;
}
