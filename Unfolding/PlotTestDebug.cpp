#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TColor.h>
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"


template <typename T>
void CheckFileExist(T* obj, const std::string& filename = "") {
    if (!obj) {
        cout << "Error: Could not open file" << filename << endl;
        exit(1);
    }
}

int PlotTestDebug(){
    gStyle->SetOptStat(0);

    string MCFile = "/home/xirong/JetStudiesOO/Unfolding/UnfoldingSplit_R040_Test.root";
    TFile *MCFilef = TFile::Open(MCFile.c_str(), "READ");
    CheckFileExist(MCFilef, MCFile);

    RooUnfoldResponse* response_pt = (RooUnfoldResponse*)MCFilef->Get("response");

    TH1F* hGenResponse = (TH1F*)response_pt->Htruth();
    TH1F* hRecoResponse = (TH1F*)response_pt->Hmeasured();


    for (int i = 1; i <= hGenResponse->GetNbinsX(); ++i) {
        cout << Form("Bin %d: Gen Response = %.2f, Reco Response = %.2f", i, hGenResponse->GetBinContent(i), hRecoResponse->GetBinContent(i)) << endl;
    }
    RooUnfoldBayes unfold(response_pt, hRecoResponse, 1,false);
    //TH1F* hUnfolded_trivial= (TH1F*)unfold.Hreco();
    cout << "Trivial Unfolding ratio (UnfoldedGen / ResponseGen) values:" << endl;


    TH1D* hUnfolded_trivial_double = (TH1D*)MCFilef->Get("Bayesian_UnfoldedTrivial");
    TH1F* hUnfolded_trivial_float = (TH1F*)MCFilef->Get("Bayesian_UnfoldedTrivial_Float");

    for (int i = 1; i <= hUnfolded_trivial_double->GetNbinsX(); ++i) {
      double unfolded_gen_double = hUnfolded_trivial_double->GetBinContent(i);
      float unfolded_gen_float = hUnfolded_trivial_float->GetBinContent(i);

      float gen_value_float = hGenResponse->GetBinContent(i);
      double gen_value_double = hGenResponse->GetBinContent(i);

      cout << "Bin: " << i 
           << ", Ratio (Double) = " << (gen_value_double > 0 ? unfolded_gen_double/gen_value_double : 0)
           << ", Ratio (Float) = " << (gen_value_float > 0 ? unfolded_gen_float/gen_value_float : 0) 
           << endl;
    }
    return 0;
}
