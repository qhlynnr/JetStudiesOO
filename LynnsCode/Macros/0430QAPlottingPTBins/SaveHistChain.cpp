
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <string>
#include <vector>
#include <TMath.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TChain.h>
#include <TStyle.h>
#include <TNamed.h>
#include <algorithm>
#include <ctime>
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
#include <TVector2.h>

using namespace std;

// Function to get list of ROOT files from a directory
vector<string> GetRootFiles(const string& pattern) {
    vector<string> rootFiles;

    TString dirname = gSystem->DirName(pattern.c_str());
    TString basename = gSystem->BaseName(pattern.c_str());

    void* dp = gSystem->OpenDirectory(dirname);
    if (!dp) {
        cout << "Error: cannot open directory " << dirname << endl;
        return rootFiles;
    }

    // wildcard matcher (compatible with older ROOT)
    TRegexp re(basename, true);

    const char* file;
    while ((file = gSystem->GetDirEntry(dp))) {
        TString fname = file;

        // skip . and ..
        if (fname == "." || fname == "..") continue;

        if (!fname.EndsWith(".root")) continue;
        if (fname.Index(re) == kNPOS) continue;
        rootFiles.push_back((dirname + "/" + fname).Data());
    }

    gSystem->FreeDirectory(dp);

    sort(rootFiles.begin(), rootFiles.end());

    cout << "Found " << rootFiles.size() << " ROOT files matching pattern: "
         << pattern << endl;

    return rootFiles;
}
int main(int argc, char *argv[]) {
    std::cout << "Have " << argc << " arguments:\n";
    bool MC = atoi(argv[1]);
    float jtptminCut = atof(argv[2]);
    float jtptmaxCut = atof(argv[3]);
    int nevents = atoi(argv[4]);
    string outfoldername = argv[5];

    cout << "MC: " << MC << endl;
    cout << "Jet pT min cut: " << jtptminCut << " GeV/c" << endl;
    cout << "Jet pT max cut: " << jtptmaxCut << " GeV/c" << endl;
    cout << "Number of events to process: " << nevents << endl;
    string MCpthat15String = "/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/*";
    string MCpthat0String = "/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/MinBias_OO_5p36TeV_hijing/MiniumBiasOO_MC_5p36TeV_HIJING/260316_213344/0000/*";
    string DataString = "/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics0/OO_Data_PromptReco_IonPhsyics0/260306_195006/0000/*";
    /*************************************************************************
     *                                                                       *
     *                      TUNABLE PARAMETERS SECTION                       *
     *                                                                       *
     *                                                                       *
     *************************************************************************/
    string outfiletag = Form("1PD_xrd_test_%ito%iGEV", (int)jtptminCut, (int)jtptmaxCut);

    string ForestFolder = MC ? MCpthat15String : DataString;
    bool L1MinBiasBool = true;
    bool HLTMinBiasBool = false;
    bool JetTriggerBool = false;
    bool CCFilterBool = true;
    bool PVFilterBool = true;
    bool zvtxCutBool = true;
    bool JetPtCutBool = true;
    bool JetSelectionsBool = true;
    bool HFEFilterBool = true;

    float zvtxCutValue = 15.0;

    float jtPfCEFcut = 0.8;
    float jtPfNEFcut = 0.8;
    float jtPfMUFcut = 0.9;
    int jtPfCHMcut = 0;

    float etaCut = 1.6;
    float HFEMaxCut = 14;
    /*************************************************************************
     *                                                                       *
     *                          Code here                                    *
     *                                                                       *
     *                                                                       *
     *************************************************************************/


    string cutString = "";
    string generalInfoString = "";

    if (L1MinBiasBool == 1){
        cutString += "L1 MinBias Trigger;";
    }
    if (HLTMinBiasBool == 1){
        cutString += "HLT MinBias Trigger;";
    }
    if (JetTriggerBool == 1){
        cutString += "Jet60 Trigger (L1 and HLT);";
    }
    if (CCFilterBool == 1){
        cutString += "Cluster Compatibility Filter;";
    }
    if (PVFilterBool == 1){
        cutString += "Primary Vertex Filter;";
    }
    if (JetPtCutBool == 1){
        cutString += Form("%.1f > Jet pT > %.1f GeV/c;", jtptminCut, jtptmaxCut);
        cutString += Form("Jet |eta| < %.1f;", etaCut);
    }
    if (HFEFilterBool == 1){
        cutString += Form("HF E_{Max}+ AND HF E_{Max}- > %i;", (int)HFEMaxCut);
    }

    if (JetSelectionsBool == 1){
        cutString += Form("Jet PF CEF < %.1f;", jtPfCEFcut);
        cutString += Form("Jet PF NEF < %.1f;", jtPfNEFcut);
        cutString += Form("Jet PF MUF < %.1f;", jtPfMUFcut);
        cutString += Form("Jet PF CHM > %d;", jtPfCHMcut);
    }


    // inside SaveHistChain(), replace placeholder with:
    time_t t = time(nullptr);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
    char datebuf[64];
    strftime(datebuf, sizeof(datebuf), "%Y%m%d", &tm);
    std::string date(datebuf);
    cout << "Current time: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << endl;
    string sampleType = MC ? "MC" : "Data";
    
    
    string JetAnalyserTreeString = "akCs4PFJetAnalyzer/t";
    string HltTreeString = "hltanalysis/HltTree";
    string SkimTreeString = "skimanalysis/HltTree";
    string PPTracksTreeString = "ppTracks/trackTree";
    string HiEvtAnalyzersTreeString = "hiEvtAnalyzer/HiTree";

    TChain* JetAnalyserTree = new TChain(JetAnalyserTreeString.c_str());
    TChain* SkimTree = new TChain(SkimTreeString.c_str());
    TChain* HltTree = new TChain(HltTreeString.c_str());
    TChain* PPTracksTree = new TChain(PPTracksTreeString.c_str());
    TChain* HiEvtAnalyzersTree = new TChain(HiEvtAnalyzersTreeString.c_str());

    // Get list of ROOT files
    vector<string> rootFiles = GetRootFiles(ForestFolder);

    cout << "Number of ROOT files to process: " << rootFiles.size() << endl;
    
    if (rootFiles.empty()) {
        cout << "ERROR: No ROOT files found in " << ForestFolder << endl;
        // Clean up TChains before returning
        delete JetAnalyserTree;
        delete SkimTree; 
        delete HltTree;
        delete PPTracksTree;
        delete HiEvtAnalyzersTree;
        return -1;
    }
    
    for (size_t filenum = 0; filenum < rootFiles.size(); ++filenum) {
        const string& file = rootFiles[filenum];
       // std::string eosFileName = "root://eoscms.cern.ch/" + file;
        cout << "Processing file " << (filenum + 1) << " of " << rootFiles.size() << ": " << file << endl;

        JetAnalyserTree->Add(file.c_str());
        HltTree->Add(file.c_str());
        SkimTree->Add(file.c_str());
        PPTracksTree->Add(file.c_str());
        HiEvtAnalyzersTree->Add(file.c_str());
        if (JetAnalyserTree->GetEntries() > nevents && nevents > 0){
            cout << "Reached event limit of " << nevents << ". Stopping file addition." << endl;
            cout << endl;
            break;
        }
    }

    cout << "Number of entries: " << JetAnalyserTree->GetEntries() << endl;
    cout << endl;
   
    int run, evt, lumi, nref;
    const int N_MAXJETS = 500;
    Float_t rawpt[N_MAXJETS];
    Float_t jtpt[N_MAXJETS],   jteta[N_MAXJETS],  jtphi[N_MAXJETS],   jtrg[N_MAXJETS],  jtzg[N_MAXJETS],  jtkt[N_MAXJETS];
    Float_t refpt[N_MAXJETS],  refeta[N_MAXJETS], refphi[N_MAXJETS],  refrg[N_MAXJETS], refzg[N_MAXJETS], refkt[N_MAXJETS];
    Float_t genpt[N_MAXJETS],  geneta[N_MAXJETS], genphi[N_MAXJETS], genrg[N_MAXJETS], genzg[N_MAXJETS], genkt[N_MAXJETS];
    Float_t jtPfCHF[N_MAXJETS], jtPfNHF[N_MAXJETS], jtPfCEF[N_MAXJETS], jtPfNEF[N_MAXJETS], jtPfMUF[N_MAXJETS];
    int jtPfCHM[N_MAXJETS];

    //Trigger variables
    int L1_MinimumBiasHF1_OR_BptxAND;
    int HLT_MinimumBiasHF_AND_BptxAND_v1;
    int L1_SingleJet60;
    int HLT_OxyL1SingleJet60_v1;
    float weight;

    //SkimFilters
    int pclusterCompatibilityFilter;
    int pprimaryVertexFilter;

    //HiEvtAnalyzer variables
    float hiHFMinus_pf;
    float hiHFEPlus_pf;
    float ptHat;

    double ptBins[] = {80, 140, 200, 300,400,500};
    double rgBins[] = {-0.05,0,0.05,0.1,0.2,0.3}; 

    int nrgBins = sizeof(rgBins)/sizeof(double) - 1;
    int nptBins = sizeof(ptBins)/sizeof(double) - 1;


    TH1F* hjtpt = new TH1F("hjtpt", "Jet pT Distribution; Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hjteta = new TH1F("hjteta", "Jet eta Distribution; Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hjtphi = new TH1F("hjtphi", "Jet phi Distribution; Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hjtrg = new TH1F("hjtrg", "Jet RG Distribution; Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hjtzg = new TH1F("hjtzg", "Jet Zg Distribution; Jet Zg; Number of Jets", 100, 0, 2);
    TH1F* hjtkt = new TH1F("hjtkt", "Jet Kt Distribution; Jet Kt (GeV/c); Number of Jets", 100, 0, 5);

    TH1F* hrefpt = new TH1F("hrefpt", "Reference Jet pT Distribution; Reference Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hrefeta = new TH1F("hrefeta", "Reference Jet eta Distribution; Reference Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hrefphi = new TH1F("hrefphi", "Reference Jet phi Distribution; Reference Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hrefrg = new TH1F("hrefrg", "Reference Jet RG Distribution; Reference Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hrefzg = new TH1F("hrefzg", "Reference Jet Zg Distribution; Reference Jet Zg; Number of Jets", 100, 0, 2);
    TH1F* hrefkt = new TH1F("hrefkt", "Reference Jet Kt Distribution; Reference Jet Kt (GeV/c); Number of Jets", 100, 0, 5);

    TH1F* hgenpt = new TH1F("hgenpt", "Gen Jet pT Distribution; Gen Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hgeneta = new TH1F("hgeneta", "Gen Jet eta Distribution; Gen Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hgenphi = new TH1F("hgenphi", "Gen Jet phi Distribution; Gen Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hgenrg = new TH1F("hgenrg", "Gen Jet RG Distribution; Gen Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hgenzg = new TH1F("hgenzg", "Gen Jet Zg Distribution; Gen Jet Zg; Number of Jets", 100, 0, 2);
    TH1F* hgenkt = new TH1F("hgenkt", "Gen Jet Kt Distribution; Gen Jet Kt (GeV/c); Number of Jets", 100, 0, 5);
    
    TH1F* hjtPfCHF = new TH1F("hjtPfCHF", "Jet PF Charged Hadron Energy Fraction Distribution; Jet PF Charged Hadron Energy Fraction; Number of Jets", 100, 0, 1);
    TH1F* hjtPfNHF = new TH1F("hjtPfNHF", "Jet PF Neutral Hadron Energy Fraction Distribution; Jet PF Neutral Hadron Energy Fraction; Number of Jets", 100, 0, 1);
    TH1F* hjtPfCEF = new TH1F("hjtPfCEF", "Jet PF Charged Em Energy Fraction Distribution; Jet PF Charged EM Energy Fraction; Number of Jets", 100, 0, 1);
    TH1F* hjtPfNEF = new TH1F("hjtPfNEF", "Jet PF Neutral EM Energy Fraction Distribution; Jet PF Neutral EM Energy Fraction; Number of Jets", 100, 0, 1);
    TH1F* hjtPfMUF = new TH1F("hjtPfMUF", "Jet PF Muon Energy Fraction Distribution; Jet PF Muon Energy Fraction; Number of Jets", 100, 0, 1);
    
    TH2F* hpthatvjtpt = new TH2F("hpthatvjtpt", "pthat vs jetpt;#hat{p}_{T};jet p_{T}",
                   100, 0, 500,
                   100, 0, 500);

    gStyle->SetOptStat(0); // Disable statistics box

    HltTree->SetBranchStatus("*", 0); // Disable all branches
    SkimTree->SetBranchStatus("*", 0); // Disable all branches
    HiEvtAnalyzersTree->SetBranchStatus("*", 0); // Disable all branches
    PPTracksTree->SetBranchStatus("*", 0); // Disable all branches

    HltTree->SetBranchStatus("L1_MinimumBiasHF1_OR_BptxAND", 1);
    HltTree->SetBranchStatus("HLT_MinimumBiasHF_AND_BptxAND_v1", 1);
    HltTree->SetBranchStatus("L1_SingleJet60", 1);
    HltTree->SetBranchStatus("HLT_OxyL1SingleJet60_v1", 1);
    SkimTree->SetBranchStatus("pclusterCompatibilityFilter", 1);
    SkimTree->SetBranchStatus("pprimaryVertexFilter", 1);
    HiEvtAnalyzersTree->SetBranchStatus("hiHFMinus_pf", 1);
    HiEvtAnalyzersTree->SetBranchStatus("hiHFEPlus_pf", 1);
    if (MC){
        HiEvtAnalyzersTree->SetBranchStatus("pthat", 1);
        HiEvtAnalyzersTree->SetBranchStatus("weight", 1);
    }
    PPTracksTree->SetBranchStatus("nVtx", 1);
    PPTracksTree->SetBranchStatus("xVtx", 1);
    PPTracksTree->SetBranchStatus("yVtx", 1);
    PPTracksTree->SetBranchStatus("zVtx", 1);
    PPTracksTree->SetBranchStatus("xErrVtx", 1);
    PPTracksTree->SetBranchStatus("yErrVtx", 1);
    PPTracksTree->SetBranchStatus("zErrVtx", 1);
    JetAnalyserTree->SetBranchStatus("*", 0);

    JetAnalyserTree->SetBranchStatus("run", 1);
    JetAnalyserTree->SetBranchStatus("evt", 1);
    JetAnalyserTree->SetBranchStatus("lumi", 1);
    JetAnalyserTree->SetBranchStatus("nref", 1);

    JetAnalyserTree->SetBranchStatus("rawpt", 1);
    JetAnalyserTree->SetBranchStatus("jtpt", 1);
    JetAnalyserTree->SetBranchStatus("jteta", 1);
    JetAnalyserTree->SetBranchStatus("jtphi", 1);
    JetAnalyserTree->SetBranchStatus("jtrg", 1);
    JetAnalyserTree->SetBranchStatus("jtzg", 1);
    JetAnalyserTree->SetBranchStatus("jtkt", 1);

    JetAnalyserTree->SetBranchStatus("jtPfCHF", 1);
    JetAnalyserTree->SetBranchStatus("jtPfNHF", 1);
    JetAnalyserTree->SetBranchStatus("jtPfCEF", 1);
    JetAnalyserTree->SetBranchStatus("jtPfNEF", 1);
    JetAnalyserTree->SetBranchStatus("jtPfMUF", 1);
    JetAnalyserTree->SetBranchStatus("jtPfCHM", 1);

    if (MC) {
        JetAnalyserTree->SetBranchStatus("refpt", 1);
        JetAnalyserTree->SetBranchStatus("refeta", 1);
        JetAnalyserTree->SetBranchStatus("refphi", 1);
        JetAnalyserTree->SetBranchStatus("refrg", 1);
        JetAnalyserTree->SetBranchStatus("refzg", 1);
        JetAnalyserTree->SetBranchStatus("refkt", 1);

        JetAnalyserTree->SetBranchStatus("genpt", 1);
        JetAnalyserTree->SetBranchStatus("geneta", 1);
        JetAnalyserTree->SetBranchStatus("genphi", 1);
        JetAnalyserTree->SetBranchStatus("genrg", 1);
        JetAnalyserTree->SetBranchStatus("genzg", 1);
        JetAnalyserTree->SetBranchStatus("genkt", 1);
    }


    JetAnalyserTree->SetBranchAddress("run",&run);
    JetAnalyserTree->SetBranchAddress("evt",&evt);
    JetAnalyserTree->SetBranchAddress("nref",&nref);
    JetAnalyserTree->SetBranchAddress("lumi",&lumi);

    JetAnalyserTree->SetBranchAddress("rawpt",rawpt);

    JetAnalyserTree->SetBranchAddress("jtpt",jtpt);
    JetAnalyserTree->SetBranchAddress("jteta",jteta);
    JetAnalyserTree->SetBranchAddress("jtphi",jtphi);
    JetAnalyserTree->SetBranchAddress("jtrg",jtrg);
    JetAnalyserTree->SetBranchAddress("jtzg",jtzg);
    JetAnalyserTree->SetBranchAddress("jtkt",jtkt);
    
    if(MC){
        JetAnalyserTree->SetBranchAddress("refpt",refpt);
        JetAnalyserTree->SetBranchAddress("refeta",refeta);
        JetAnalyserTree->SetBranchAddress("refphi",refphi);
        JetAnalyserTree->SetBranchAddress("refrg",refrg);
        JetAnalyserTree->SetBranchAddress("refzg",refzg);
        JetAnalyserTree->SetBranchAddress("refkt",refkt);

        JetAnalyserTree->SetBranchAddress("genpt",genpt);
        JetAnalyserTree->SetBranchAddress("geneta",geneta);
        JetAnalyserTree->SetBranchAddress("genphi",genphi);
        JetAnalyserTree->SetBranchAddress("genrg",genrg);
        JetAnalyserTree->SetBranchAddress("genzg",genzg);
        JetAnalyserTree->SetBranchAddress("genkt",genkt);
    }

    JetAnalyserTree->SetBranchAddress("jtPfCHF",jtPfCHF);
    JetAnalyserTree->SetBranchAddress("jtPfNHF",jtPfNHF);
    JetAnalyserTree->SetBranchAddress("jtPfCEF",jtPfCEF);
    JetAnalyserTree->SetBranchAddress("jtPfNEF",jtPfNEF);
    JetAnalyserTree->SetBranchAddress("jtPfMUF",jtPfMUF);
    JetAnalyserTree->SetBranchAddress("jtPfCHM",jtPfCHM);

    HltTree->SetBranchAddress("L1_MinimumBiasHF1_OR_BptxAND",&L1_MinimumBiasHF1_OR_BptxAND);
    HltTree->SetBranchAddress("HLT_MinimumBiasHF_AND_BptxAND_v1",&HLT_MinimumBiasHF_AND_BptxAND_v1);
    HltTree->SetBranchAddress("L1_SingleJet60",&L1_SingleJet60);
    HltTree->SetBranchAddress("HLT_OxyL1SingleJet60_v1",&HLT_OxyL1SingleJet60_v1);

    SkimTree->SetBranchAddress("pclusterCompatibilityFilter",&pclusterCompatibilityFilter);
    SkimTree->SetBranchAddress("pprimaryVertexFilter",&pprimaryVertexFilter);

    HiEvtAnalyzersTree->SetBranchAddress("hiHFMinus_pf",&hiHFMinus_pf);
    HiEvtAnalyzersTree->SetBranchAddress("hiHFEPlus_pf",&hiHFEPlus_pf);

    if(MC){
        HiEvtAnalyzersTree->SetBranchAddress("pthat",&ptHat);
        HiEvtAnalyzersTree->SetBranchAddress("weight",&weight);
    }
    else{
        weight = 1.0;
    }
    //PPTracks variables
    int nvtx;
    vector<float>* zVtx = nullptr;

    PPTracksTree->SetBranchAddress("nVtx",&nvtx);
    PPTracksTree->SetBranchAddress("zVtx",&zVtx);

    Long64_t nEntries = PPTracksTree->GetEntries();
    int totaljets = 0;
    
    // Event counting variables
    Long64_t eventsBeforeCuts = 0;
    Long64_t eventsAfterCuts = 0;
    Long64_t jetsBeforeSelection = 0;
    Long64_t jetsAfterSelection = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    cout << "Starting processing..." << endl;
    
    for (Long64_t entrynum = 0; entrynum < nEntries; entrynum++){
        SkimTree->GetEntry(entrynum);
        HltTree->GetEntry(entrynum);
        JetAnalyserTree->GetEntry(entrynum);
        PPTracksTree->GetEntry(entrynum);
        HiEvtAnalyzersTree->GetEntry(entrynum);
        totaljets += nref;
        eventsBeforeCuts++;

        if (entrynum % 100000 == 0){
            // Get current memory usage
            struct rusage current_usage;
            getrusage(RUSAGE_SELF, &current_usage);
            double current_memory_mb = current_usage.ru_maxrss / 1024.0;
            
            // Calculate memory increase from start
            static double initial_memory_mb = 0;
            if (entrynum == 0) {
                initial_memory_mb = current_memory_mb;
            }
            double memory_increase = current_memory_mb - initial_memory_mb;
            
            cout << endl;
            cout << "Processing entry " << entrynum << " out of " << nEntries << "; " << (float)entrynum/nEntries*100 << "%" << endl;
            cout << "Number of Jets in this event: " << nref << endl;
            cout << "Total jets processed so far: " << totaljets << endl;
            cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start_time).count() << " seconds" << endl;
            cout << "Current memory usage: " << current_memory_mb << " MB (+" << memory_increase << " MB from start)" << endl;
            if(MC){
                cout << "pthat: " << ptHat << "; weight: " << weight << endl;
            }
            cout << endl;
        }

        if (L1MinBiasBool == 1){
            if (L1_MinimumBiasHF1_OR_BptxAND != 1) continue;
        }
        if (HLTMinBiasBool == 1){
            if (HLT_MinimumBiasHF_AND_BptxAND_v1 != 1) continue;
        }
        if (JetTriggerBool == 1){
            if (L1_SingleJet60 != 1 || HLT_OxyL1SingleJet60_v1 != 1) continue;
        }
        if (CCFilterBool == 1){
            if (pclusterCompatibilityFilter != 1) continue;
        }
        if (PVFilterBool == 1){
            if (pprimaryVertexFilter != 1) continue;
        }        
        if (zvtxCutBool == 1){
            if (nvtx > 0 && zVtx && zVtx->size() > 0 && TMath::Abs(zVtx->at(0)) > zvtxCutValue) continue;
        }        
        if (HFEFilterBool == 1){
            if (hiHFMinus_pf < HFEMaxCut || hiHFEPlus_pf < HFEMaxCut) continue;
        }
        eventsAfterCuts++;

        // Safety check for nref bounds
        if (nref > N_MAXJETS) {
            cout << "WARNING: nref (" << nref << ") exceeds array bounds (50). Limiting to 50." << endl;
            nref = 50;
        }

        for (int j = 0; j < nref; j++){
            jetsBeforeSelection++;

            if(MC){
                hgenpt->Fill(genpt[j], weight);
                hgeneta->Fill(geneta[j], weight);
                hgenphi->Fill(genphi[j], weight);
                hgenrg->Fill(genrg[j], weight);
                hgenzg->Fill(genzg[j], weight);
                hgenkt->Fill(genkt[j], weight);
            }

            if (JetPtCutBool == 1){
                if (jtpt[j] < jtptminCut) continue;
                if (jtpt[j] > jtptmaxCut) continue;
                if (TMath::Abs(jteta[j]) > etaCut) continue;
            }
            if (JetSelectionsBool == 1){
                if (jtPfCEF[j] > jtPfCEFcut) continue;
                if (jtPfNEF[j] > jtPfNEFcut) continue;
                if (jtPfMUF[j] > jtPfMUFcut) continue;
                if (jtPfCHM[j] < 0) continue; 
            }
            
            jetsAfterSelection++;

            if(MC){
                if (refpt[j] < -998) continue; // Skip invalid reference jets
                hrefpt->Fill(refpt[j], weight);
                hrefeta->Fill(refeta[j], weight);
                hrefphi->Fill(refphi[j], weight);
                hrefrg->Fill(refrg[j], weight);
                hrefzg->Fill(refzg[j], weight);
                hrefkt->Fill(refkt[j], weight);
            }

            hjtpt->Fill(jtpt[j], weight);
            hjteta->Fill(jteta[j], weight);
            hjtphi->Fill(jtphi[j], weight);
            hjtrg->Fill(jtrg[j], weight);
            hjtzg->Fill(jtzg[j], weight);
            hjtkt->Fill(jtkt[j], weight);

            hjtPfCEF->Fill(jtPfCEF[j], weight);
            hjtPfNEF->Fill(jtPfNEF[j], weight);
            hjtPfMUF->Fill(jtPfMUF[j], weight);
            hjtPfNHF->Fill(jtPfNHF[j], weight);
            hjtPfCHF->Fill(jtPfCHF[j], weight);
        }
        if (entrynum > nevents && nevents != 0){
            cout << Form("Debug mode: stopping after %d entries.", nevents) << endl;
            break;
        }
    }

    cout << "\n=== PROCESSING SUMMARY ===" << endl;
    cout << "Events before cuts: " << eventsBeforeCuts << endl;
    cout << "Events after cuts: " << eventsAfterCuts << endl;
    cout << "Event cut efficiency: " << (double)eventsAfterCuts/eventsBeforeCuts*100.0 << "%" << endl;
    cout << "Jets before selection: " << jetsBeforeSelection << endl;
    cout << "Jets after selection: " << jetsAfterSelection << endl;
    cout << "Jet selection efficiency: " << (jetsBeforeSelection > 0 ? (double)jetsAfterSelection/jetsBeforeSelection*100.0 : 0.0) << "%" << endl;
    cout << "Total jets processed: " << totaljets << endl;
    cout << "=========================" << endl;
    

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    double total_seconds = duration.count() / 1000.0;

    // compute hours, minutes, seconds for a human-friendly display
    int hours = 0;
    int minutes = 0;
    double seconds = total_seconds;
    if (total_seconds > 0.0) {
        hours = static_cast<int>(total_seconds) / 3600;
        minutes = (static_cast<int>(total_seconds) % 3600) / 60;
        seconds = total_seconds - hours * 3600 - minutes * 60;
    }
    std::string total_time_hms = Form("%dh %dm %.2fs", hours, minutes, seconds);
    cout << "\n=== TIMING SUMMARY ===" << endl;
    cout << "Total processing time (H:M:S): " << total_time_hms << endl; double events_per_second = nEntries / total_seconds;
    cout << "Events processed per second: " << events_per_second << " Hz" << endl;
    cout << "Average time per event: " << (total_seconds * 1000.0) / nEntries << " ms" << endl;
    cout << "=======================" << endl;
    TNamed* cutInfo = new TNamed("Cuts",cutString.c_str());

    // produce human-friendly label: 1K, 1.5K, 1M, 2.3M, or "All"
    std::string nentriesLabel;
    long long nE = static_cast<long long>(nEntries); // ensure wide enough for formatting

    if (nE >= 1000000) {
        if (nE % 1000000 == 0) {
            nentriesLabel = Form("%lldM", nE / 1000000);
        } else {
            nentriesLabel = Form("%.1fM", nE / 1000000.0);
        }
    } else if (nE >= 1000) {
        if (nE % 1000 == 0) {
            nentriesLabel = Form("%lldK", nE / 1000);
        } else {
            nentriesLabel = Form("%.1fK", nE / 1000.0);
        }
    } else {
        nentriesLabel = Form("%lld", nE);
    }

    generalInfoString += Form("NEntries: %s;", nentriesLabel.c_str());
    generalInfoString += Form("Sample: %s;", sampleType.c_str());
    generalInfoString += Form("Tag: %s;", outfiletag.c_str());
    generalInfoString += Form("ProcessingDate: %s;", date.c_str());
    generalInfoString += Form("EventsBeforeCuts: %lld;", eventsBeforeCuts);
    generalInfoString += Form("EventsAfterCuts: %lld;", eventsAfterCuts);
    generalInfoString += Form("EventCutEfficiency: %.2f%%;", (double)eventsAfterCuts/eventsBeforeCuts*100.0);
    generalInfoString += Form("JetsBeforeSelection: %lld;", jetsBeforeSelection);
    generalInfoString += Form("JetsAfterSelection: %lld;", jetsAfterSelection);
    generalInfoString += Form("JetSelectionEfficiency: %.2f%%;", jetsBeforeSelection > 0 ? (double)jetsAfterSelection/jetsBeforeSelection*100.0 : 0.0);
    generalInfoString += Form("TotalJetsProcessed: %d;", totaljets);
    if(MC) {
        generalInfoString += Form("MCWeighted: Yes;");
    } else {
        generalInfoString += Form("MCWeighted: No;");
    }
    
    // Add individual cut efficiencies if cuts were applied
    
    TNamed* generalInfo = new TNamed("GeneralInfo", generalInfoString.c_str());
    cout << "Cuts applied: " << cutString << endl;
    cout << "General info: " << generalInfoString << endl;

    string outFileName = Form("/home/xirong/JetStudiesOO/LynnsCode/RootFiles/%s/%s_%sEvts_%s_%s.root",
                                outfoldername.c_str(), sampleType.c_str(), nentriesLabel.c_str(), outfiletag.c_str(), date.c_str());

    TFile* outFile = new TFile(outFileName.c_str(), "RECREATE");
    cout << "Saving histograms to output file: " << outFileName << endl;
    if (!outFile || outFile->IsZombie()) {
        cout << "ERROR: Could not create output file: " << outFileName << endl;
        
        // Clean up TNamed objects
        delete cutInfo;
        delete generalInfo;
        
        // Clean up all histograms
        delete hjtpt; delete hjteta; delete hjtphi;  delete hjtrg;
        delete hjtzg; delete hjtkt; 
        if (MC) {
            delete hrefpt; delete hrefeta; delete hrefphi; 
            delete hrefrg; delete hrefzg; delete hrefkt; 
            delete hgenpt; delete hgeneta; delete hgenphi; 
            delete hgenrg; delete hgenzg; delete hgenkt; 
            delete hpthatvjtpt;
        }
        delete hjtPfCHF; delete hjtPfNHF; delete hjtPfCEF; delete hjtPfNEF; delete hjtPfMUF;
        
        // Clean up TChains
        delete JetAnalyserTree;
        delete SkimTree; 
        delete HltTree;
        delete PPTracksTree;
        delete HiEvtAnalyzersTree;
        
        return -1;
    }
    
    // Print memory usage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    cout << "\n=== MEMORY SUMMARY ===" << endl;
    cout << "Peak memory usage: " << usage.ru_maxrss / 1024.0 << " MB" << endl;
    cout << "Memory cleanup completed." << endl;
    cout << "=======================" << endl;

    cutInfo->Write();
    generalInfo->Write();

    if (!outFile->mkdir("JetLevelHistRaw") || !outFile->mkdir("EventLevelHistRaw") || !outFile->mkdir("TrackLevelHistRaw")) {
        cout << "WARNING: Could not create some directories in output file" << endl;
    }

    outFile->cd("JetLevelHistRaw");
    hjtpt->Write();
    hjteta->Write();
    hjtphi->Write();
    hjtrg->Write();
    hjtzg->Write();
    hjtkt->Write();

    if(MC){
        hrefpt->Write();
        hrefeta->Write();
        hrefphi->Write();
        hrefrg->Write();
        hrefzg->Write();
        hrefkt->Write();
        hgenpt->Write();
        hgeneta->Write();
        hgenphi->Write();
        hgenrg->Write();
        hgenzg->Write();
        hgenkt->Write();
        hpthatvjtpt->Write();
    }
    hjtPfCHF->Write();
    hjtPfNHF->Write();
    hjtPfCEF->Write();
    hjtPfNEF->Write();
    hjtPfMUF->Write();

    outFile->cd("EventLevelHistRaw");


    // Close output file first
    outFile->Close();
    delete outFile;
    
    // Delete TNamed objects
    delete cutInfo;
    delete generalInfo;

    // Delete all histograms
    delete hjtpt; delete hjteta; delete hjtphi; delete hjtrg;
    delete hjtzg; delete hjtkt; 
    delete hrefpt; delete hrefeta; delete hrefphi;
    delete hrefrg; delete hrefzg; delete hrefkt; 
    delete hgenpt; delete hgeneta; delete hgenphi; 
    delete hgenrg; delete hgenzg; delete hgenkt; 
    delete hjtPfCHF; delete hjtPfNHF; delete hjtPfCEF; delete hjtPfNEF; delete hjtPfMUF;
    delete hpthatvjtpt;

    // Delete TChains
    delete JetAnalyserTree;
    delete SkimTree; 
    delete HltTree;
    delete PPTracksTree;
    delete HiEvtAnalyzersTree;


    return 0;
}

