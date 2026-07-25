// drawVetoMap.C: Macro to draw the jet veto map
// Hannah Bossi, <hannah.bossi@cern.ch>

void drawVetoMap(){
    TFile* inFile = TFile::Open("Winter25Prompt25_RunCDEFG.root"); 
    TH2D* vetoMap = (TH2D*)inFile->Get("jetvetomap_all");  // union of all veto maps - good for analysis

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    string tag = "ForAN_June29"; 

    TLatex* cms = new TLatex(0.10,0.92, "#bf{CMS} #it{Internal} ");
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);


    TCanvas* cVeto = new TCanvas("cVeto", "", 600, 600);
    cVeto->SetTickx(); 
    cVeto->SetTicky(); 
    cVeto->SetBottomMargin(0.15); 
    cVeto->SetLeftMargin(0.15);
    vetoMap->Draw("same");

    cms->Draw(); 
    cVeto->SaveAs(Form("VetoMap_%s.pdf", tag.c_str())); 


}