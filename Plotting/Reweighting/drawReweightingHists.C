// drawReweightingHists.c
// Hannah Bossi, <hannah.bossi@cern.ch>


void drawReweightingHists(){

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    string tag = "ReweigthingHists_MC_FirstDraft"; 

    TLatex* cms = new TLatex(0.10,0.92, "#bf{PYTHIA8 + HIJING OO} (5.36 TeV) ");
    cms->SetNDC();
    cms->SetTextSize(0.05);
    cms->SetTextFont(42);



   // List your ROOT files here
    std::vector<std::string> files = { "ReweightingHistsMC_OOEmbedded_June26th.root"};

    std::vector<int> colors = {kViolet-2, kAzure-2, kSpring+2 };

   

    TFile* f = TFile::Open(files[0].c_str());
    TH1D* hCent     = (TH1D*)f->Get("hCent"); 
    TH1D* hRho      = (TH1D*)f->Get("hRho");
    TH1D* hZVertex  = (TH1D*)f->Get("hZVertex"); 
    TH2D* h2RhoCent = (TH2D*)f->Get("h2RhoCent");
    TH2D* h2VzRho   = (TH2D*)f->Get("h2VzRho"); 


    TCanvas* cRho2D = new TCanvas("cRho2D", "", 600, 600);
    cRho2D->SetTickx(); 
    cRho2D->SetTicky(); 
    cRho2D->SetLogz(); 
    cRho2D->SetRightMargin(0.15);
    h2RhoCent->GetYaxis()->SetTitle("< #rho > (GeV)");
    h2RhoCent->SetMinimum(1e-1);
    h2RhoCent->GetXaxis()->SetTitle("Centrality (%)");
    h2RhoCent->Draw("COLZ");
    cms->Draw(); 
    cRho2D->SaveAs(Form("Rho2D_%s.pdf", tag.c_str())); 

     // -----------------------------------------------------------
    // Now loop over the bins and plot the JES and the JER

    // first create the empty histograms for the JES and JER
    // inclusive case 
    TH1D* hRhoVCent = (TH1D*)h2RhoCent->ProjectionX("hRhoVCent"); 
    hRhoVCent->Reset(); 


    Int_t nBins = hRhoVCent->GetXaxis()->GetNbins();
    for(Int_t i=1; i <= nBins ; i++){
      TString proj = Form("px_%d", i);
      TH1D* px = h2RhoCent->ProjectionY(proj, i, i); 
      TF1 *f1 = new TF1("f1", "gaus", 0, 50);
      px->Fit(f1,"","", 0, 50);
      TCanvas* cTemp = new TCanvas( Form("canvas_%d", i), "", 600, 600);
      cTemp->SetTickx(); 
      cTemp->SetTicky(); 
      cTemp->SetRightMargin(0.15); 
      cTemp->SetLeftMargin(0.12);
      TLatex* pt = new TLatex(0.3,0.15, Form("%0.2f < Cent < %0.2f GeV : #mu = %0.4f",h2RhoCent->GetXaxis()->GetBinLowEdge(i), h2RhoCent->GetXaxis()->GetBinLowEdge(i) + h2RhoCent->GetXaxis()->GetBinWidth(i), f1->GetParameter(1)));
      pt->SetNDC();
      pt->SetTextSize(0.02);
      pt->SetTextFont(42);
      std::cout << "bins " << px->GetNbinsX() << std::endl;
      px->Draw();
      cms->Draw();  
      pt->Draw(); 
      cTemp->SaveAs(Form("projRhoBinNumber_%d.pdf", i));   
      hRhoVCent->SetBinContent(i, f1->GetParameter(1)); 
      hRhoVCent->SetBinError(i, f1->GetParError(2)); 
    }

 


    
    TCanvas* cRhoVCent = new TCanvas("cRhoVCent", "", 600, 600);
    cRhoVCent->SetTickx(); 
    cRhoVCent->SetTicky(); 
    cRhoVCent->SetRightMargin(0.05); 
    cRhoVCent->SetLeftMargin(0.12); 

    hRhoVCent->GetYaxis()->SetTitle("#mu(< #rho >) (GeV)");    
    hRhoVCent->GetXaxis()->SetTitle("Centrality (%)"); 
    hRhoVCent->SetMarkerColor(kBlack); 
    hRhoVCent->SetMarkerStyle(20); 
    hRhoVCent->SetLineColor(kBlack); 
    hRhoVCent->Draw(); 
    cms->Draw(); 
    cRhoVCent->SaveAs(Form("RhoVCent_%s.pdf", tag.c_str())); 


    TCanvas* cRhoVz = new TCanvas("cRhoVz ", "", 600, 600);
    cRhoVz->SetTickx(); 
    cRhoVz->SetTicky(); 
    cRhoVz->SetLogz(); 
    cRhoVz->SetRightMargin(0.05); 
    cRhoVz->SetLeftMargin(0.12); 

    h2VzRho->GetYaxis()->SetTitle("< #rho > (GeV)");    
    h2VzRho->GetXaxis()->SetTitle("v_{z} (cm)"); 
    h2VzRho->Draw(); 
    cms->Draw(); 
    cRhoVz->SaveAs(Form("VzRho_%s.pdf", tag.c_str())); 


    //-------------------------------------------------------------
    // now draw the centrality distribution
    TCanvas* cCENT = new TCanvas("cCENT", "", 600, 600);
    cCENT->SetTickx(); 
    cCENT->SetTicky(); 
    cCENT->SetLogy(); 
    cCENT->SetRightMargin(0.15); 
    cCENT->SetLeftMargin(0.12); 
    hCent->SetMarkerColor(kBlack); 
    hCent->SetMarkerStyle(20); 
    hCent->SetLineColor(kBlack);
    hCent->GetXaxis()->SetTitle("Centrality (%)"); 
    hCent->Draw("same");

    cms->Draw(); 
    cCENT->SaveAs(Form("Cent_%s.pdf", tag.c_str())); 


    TCanvas* cRho = new TCanvas("cRho", "", 600, 600);
    cRho->SetTickx(); 
    cRho->SetTicky(); 
    cRho->SetLogy(); 
    hRho->SetMarkerColor(kBlack); 
    hRho->SetMarkerStyle(20); 
    hRho->SetLineColor(kBlack);
    hRho->GetXaxis()->SetTitle("< #rho > (GeV)"); 
    hRho->Draw("same");

    cms->Draw(); 
    cRho->SaveAs(Form("Rho1D_%s.pdf", tag.c_str())); 


    TCanvas* cZvertex = new TCanvas("cZvertex", "", 600, 600);
    cZvertex->SetTickx(); 
    cZvertex->SetTicky(); 
    cZvertex->SetLogy(); 
    // cZvertex->SetRightMargin(0.15); 
    // cZvertex->SetLeftMargin(0.12); 
    hZVertex->SetMarkerColor(kBlack); 
    hZVertex->SetMarkerStyle(20); 
    hZVertex->SetLineColor(kBlack);
    hZVertex->GetXaxis()->SetTitle("v_{z} (cm)"); 
    hZVertex->Draw("same");

    cms->Draw(); 
    cZvertex->SaveAs(Form("Zvetex_%s.pdf", tag.c_str())); 


    //-------------------------------------------------------------
    f->Close();

}