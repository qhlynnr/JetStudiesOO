
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TStyle.h>
#include <TNamed.h>
#include <algorithm>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystemFile.h> 
#include <chrono>
#include <sys/resource.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <TSystem.h>
#include <TRegexp.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TVector2.h>
#include <cctype>
#include "/home/xirong/JetStudiesOO/PPRef/Include/JetCorrector.h"
int RainbowColor(int i, int n)
{
    if (n <= 1)
        return kRed;

    // Hue from 0.70 (violet) to 0.00 (red)
    float h = 270.0f * (1.0f - static_cast<float>(i) / (n - 1));

    float r, g, b;
    TColor::HSV2RGB(h, 1.0f, 1.0f, r, g, b);

    cout << "r: " << r << " g: " << g << "b: " << b << endl;
    return TColor::GetColor(r, g, b);
}
void PlotJEC(string inFileName = "/home/xirong/JetStudiesOO/PPRef/Include/Prompt24HIpp_V1_DATA_L2Residual_AK4PF.txt"){
    JetCorrector JEC(inFileName);
    std::ifstream in(inFileName);
    std::string line;

    string correctionName = "L2Residual";
    string outFolderName = "/home/xirong/JetStudiesOO/PPRef/Plots/JECPlots/Correctiontxt/";
    std::vector<std::pair<double,double>> EtaBins;

    while(getline(in,line))
    {
        if(line.empty() || line[0]=='{')
            continue;

        std::stringstream ss(line);

        double etamin, etamax;
        ss >> etamin >> etamax;

        EtaBins.push_back({etamin, etamax});
        cout << "Eta Min: " << etamin << " Eta Max: " << etamax << endl;
    }

    TMultiGraph *mg = new TMultiGraph();
    TLegend *leg = new TLegend(0.8, 0.15, 0.95, 0.90);
    TH2D* h2 = new TH2D("h2", "pT vs eta;eta;pT",
                        50, -2.5, 2.5,
                        100, 0, 1000);
    TGraph2D* g2 = new TGraph2D();

    for (int i=0;i<EtaBins.size();i++){
        double eta = 0.5*(EtaBins[i].first + EtaBins[i].second);
        TCanvas *c_sub = new TCanvas(Form("JEC_%f",eta), Form("JEC_%f",eta), 900, 700);
        TGraph *graph = new TGraph();
        
        for(double pt = 5; pt <= 1500; pt += 1)
        {
            JEC.SetJetEta(eta);
            JEC.SetJetPT(pt);

            double corr = JEC.GetCorrection();
            graph->SetPoint(graph->GetN(), pt, corr);
            h2->Fill(eta, pt, corr);
            g2->SetPoint(g2->GetN(), pt, eta, corr);
        }
        graph->SetLineColor(RainbowColor(i, EtaBins.size()));
        leg->AddEntry(graph,
        Form("%.2f < #eta < %.2f",
            EtaBins[i].first,
            EtaBins[i].second),
        "l");
        mg->Add(graph); 
        graph->Draw("AL");
        graph->SetMarkerSize(5);
        graph->SetLineWidth(2);
        graph->GetXaxis()->SetTitle("Pt");
        graph->GetYaxis()->SetTitle("Corrections");
        graph->SetTitle(Form("JEC Correction Bins %.2f < #eta < %.2f",EtaBins[i].first, EtaBins[i].second));
        c_sub->SaveAs(Form("%s/EtaBins/JEC%s_etabin%.2f.png",outFolderName.c_str(),correctionName.c_str(),eta));
    }
    TCanvas* c_2D = new TCanvas("JEC_2D", "JEC_2D", 900, 700);
    g2->SetMinimum(0.8);
    g2->SetMaximum(1.5);
    g2->Draw("COLZ");
    c_2D->SaveAs(Form("%s/JEC%s_2D_Prompt24.png",outFolderName.c_str(), correctionName.c_str()));

    TCanvas* c = new TCanvas("JEC", "JEC", 900, 700);
    mg->SetTitle(Form("JEC Correction %s Bins ", correctionName.c_str()));
    mg->GetXaxis()->SetTitle("Pt");
    mg->GetYaxis()->SetTitle("Corrections");
    mg->SetMinimum(0.8);
    mg->SetMaximum(1.2);
    c->SetRightMargin(0.25);
    mg->Draw("AL");
    leg->SetBorderSize(0);
    string outFileName = Form("%s/JEC%s_Prompt24.png",outFolderName.c_str(), correctionName.c_str());
    leg->Draw();
    c->SaveAs(outFileName.c_str());
}