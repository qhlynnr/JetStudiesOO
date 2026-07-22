
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
#include "/home/xirong/JetStudiesOO/Include/JetSelection_PbPb.h"
#include "/home/xirong/JetStudiesOO/Include/JetCorrector.h"


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

std::string FormatNEntriesLabel(int inputNum)
{

    string nentriesLabel;
    if (inputNum >= 1000000) {
        if (inputNum % 1000000 == 0) {
            nentriesLabel = Form("%iM", inputNum / 1000000);
        } else {
            nentriesLabel = Form("%.1fM", inputNum / 1000000.0);
        }
    } else if (inputNum >= 1000) {
        if (inputNum % 1000 == 0) {
            nentriesLabel = Form("%iK", inputNum / 1000);
        } else {
            nentriesLabel = Form("%.1fK", inputNum / 1000.0);
        }
    } else {
        nentriesLabel = Form("%d", inputNum);
    }
    return nentriesLabel;
}

void SaveHistChain(bool MC = false,
                   float jtptminCut = 30.0,
                   float jtptmaxCut = 1000.0,
                   int nevents = 100000,
                   const string& outfoldername = "/PPRef/070926PPRefMCData/Test/",
                   vector<string> rootFiles = {"/eos/cms/store/group/phys_heavyions/xirong/Run3_ppref_Data_withrg/PPRefHardProbes0/crab_ppref_5362GeV_Data_withrg_rawpt/260709_053729/0000/HiForestMiniAOD_2.root"},
                   const string& outfiletag2 = "1000evt",
                   const string& jetTreeName = "ak4PFJetAnalyzer/t",
                   const string& JetVetoMap = "/home/xirong/JetStudiesOO/Include/Winter24Prompt24_2024BCDEFGHI.root",
                   vector<string> JECtxtvector= {"/home/xirong/JetStudiesOO/Include/Prompt24HIpp_V1_DATA_L2Residual_AK4PF.txt",
                                            "/home/xirong/JetStudiesOO/Include/Prompt24HIpp_V1_DATA_L2L3Residual_AK4PF.txt"});

int main(int argc, char *argv[]) {
    if (argc < 11) {
    std::cerr << "Usage: " << argv[0]
              << " MC jtptmin jtptmax neventcut outfolder infilepath outfiletag jettreename\n";
    return 1;
    }

    std::cout << "Have " << argc << " arguments:\n";
    bool MC = atoi(argv[1]);
    float jtptminCut = atof(argv[2]);
    float jtptmaxCut = atof(argv[3]);
    int nevents = atoi(argv[4]);
    string outfoldername = argv[5];
    string ForestFolder = Form("%s*",argv[6]);
    string outfiletag2 = argv[7];
    string jetTreeName = argv[8];
    string JetVetoMap = Form("/home/xirong/JetStudiesOO/Include/%s",argv[9]);
    std::vector<std::string> correctionFiles;
    for (int i = 10; i < argc; ++i) {
        cout << "Added file: " << argv[i] << endl;
        correctionFiles.push_back(
            Form("/home/xirong/JetStudiesOO/Include/%s", argv[i])
        );
    }
    cout << "MC: " << MC << endl;
    cout << "Jet pT min cut: " << jtptminCut << " GeV/c" << endl;
    cout << "Jet pT max cut: " << jtptmaxCut << " GeV/c" << endl;
    cout << "Number of events to process: " << nevents << endl;
    cout << "outfolder:" << outfoldername << endl;
    cout << "inputfolder:" << ForestFolder << endl;
    cout << "outfiletag2: " << outfiletag2 << endl;

    // string MCpthat15String = "/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/*";
   // string MCpthat0String = "/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/MinBias_OO_5p36TeV_hijing/MiniumBiasOO_MC_5p36TeV_HIJING/260316_213344/0000/*";
   // string DataString = "/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics0/OO_Data_PromptReco_IonPhsyics0/260306_195006/0000/*";
    vector<string> rootFiles = GetRootFiles(ForestFolder);
    cout << "Number of ROOT files to process: " << rootFiles.size() << endl;
    if (rootFiles.empty()) {
        cout << "ERROR: No ROOT files found in " << ForestFolder << endl;
        return 1;
    }

    cout << "Starting SaveHistChain (main):" << endl;
    SaveHistChain(MC, jtptminCut, jtptmaxCut, nevents, outfoldername, rootFiles, outfiletag2, jetTreeName, JetVetoMap,
                  correctionFiles);

    return 0;
}

void SaveHistChain(bool MC,
                   float jtptminCut,
                   float jtptmaxCut,
                   int nevents,
                   const string& outfoldername,
                   vector<string> rootFiles,
                   const string& outfiletag2,
                   const string& jetTreeName,
                   const string& JetVetoMap,
                    vector<string> JECtxtvector)
                                        
{
    /*************************************************************************
     *                                                                       *
     *                      TUNABLE PARAMETERS SECTION                       *
     *                                                                       *
     *                                                                       *
     *************************************************************************/
    string outfiletag = Form("%s", outfiletag2.c_str());

    cout << ">>Starting SaveHistChain<<" << endl;
    bool L1MinBiasBool = false;
    bool L1ZeroBiasBool = true;

    bool PVFilterBool = true;
    bool zvtxCutBool = true;
    bool JetPtCutBool = true;
    bool HFEFilterBool = true;
    float zvtxCutValue = 15.0;
    float etaCut = 1.6;
    float HFEMaxCut = 13;
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
    if (L1ZeroBiasBool == 1){
        cutString += "L1 ZeroBias Trigger;";
    }
    if (PVFilterBool == 1 && !MC){
        cutString += "Primary Vertex Filter;";
    }
    if (JetPtCutBool == 1){
        cutString += Form("%.1f > Jet pT > %.1f GeV/c;", jtptminCut, jtptmaxCut);
        cutString += Form("Jet |eta| < %.1f;", etaCut);
    }
    if (HFEFilterBool == 1){
        cutString += Form("HF E_{Max}+ AND HF E_{Max}- > %i;", (int)HFEMaxCut);
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
    
    
    string JetAnalyserTreeString = jetTreeName;
    string HltTreeString = "hltanalysis/HltTree";
    string SkimTreeString = "skimanalysis/HltTree";
    string PPTracksTreeString = "ppTracks/trackTree";
    string HiEvtAnalyzersTreeString = "hiEvtAnalyzer/HiTree";

    TChain* JetAnalyserTree = new TChain(JetAnalyserTreeString.c_str());
    TChain* HltTree = new TChain(HltTreeString.c_str());
    TChain* PPTracksTree = new TChain(PPTracksTreeString.c_str());
    TChain* HiEvtAnalyzersTree = new TChain(HiEvtAnalyzersTreeString.c_str());
    TChain* SkimTree = new TChain(SkimTreeString.c_str());

    cout << "Number of entries: " << JetAnalyserTree->GetEntries() << endl;
    cout << endl;
   
    int run, evt, lumi, nref;
    const int N_MAXJETS = 500;
    Float_t rawpt[N_MAXJETS];
    Float_t jtpt[N_MAXJETS],   jteta[N_MAXJETS],  jtphi[N_MAXJETS];
    Float_t jtrg[N_MAXJETS],  jtzg[N_MAXJETS],  jtkt[N_MAXJETS];
    Float_t jtPfCHF[N_MAXJETS], jtPfNHF[N_MAXJETS], jtPfCEF[N_MAXJETS], jtPfNEF[N_MAXJETS], jtPfMUF[N_MAXJETS];
    int jtPfCHM[N_MAXJETS];
    Float_t refpt[N_MAXJETS],  refeta[N_MAXJETS], refphi[N_MAXJETS];
    Float_t refrg[N_MAXJETS],  refzg[N_MAXJETS], refkt[N_MAXJETS];
    Float_t genpt[N_MAXJETS],  geneta[N_MAXJETS], genphi[N_MAXJETS];
    Float_t genrg[N_MAXJETS],  genzg[N_MAXJETS], genkt[N_MAXJETS];

    //Trigger variables
    int Trigger;
    int TriggerPrescale;
 //   int L1_SingleJet28_BptxAND;
    float weight;

    //SkimFilters
    int pprimaryVertexFilter;

    //HiEvtAnalyzer variables
    float hiHFMinus_pf;
    float hiHFEPlus_pf;
    float ptHat;

   // double ptBins[] = {80, 140, 200, 300, 400, 500};
    double rgBins[] = {-0.05,0,0.05,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0}; 
    double ptBins[] = {30, 32, 35, 38, 41, 44, 48, 52, 56, 61, 66, 71, 77, 83, 90, 97, 105, 113, 123, 133, 143, 155, 168, 181, 196, 212, 229, 248, 268, 290, 314, 339, 367, 397, 429, 464, 502, 542, 587, 634, 686, 742, 802, 867, 938, 1014, 1097, 1186, 1283, 1387, 1500};

    int nrgBins = sizeof(rgBins)/sizeof(double) - 1;
    int nptBins = sizeof(ptBins)/sizeof(double) - 1;

    TH1F* hjtpt = new TH1F("hjtpt", "Jet pT Distribution; Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hcorrectedpt = new TH1F("hcorrectedpt", "Jet pT Distribution (manual corrections); Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hjteta = new TH1F("hjteta", "Jet eta Distribution; Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hjtphi = new TH1F("hjtphi", "Jet phi Distribution; Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hjtrg = new TH1F("hjtrg", "Jet RG Distribution; Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hjtzg = new TH1F("hjtzg", "Jet Zg Distribution; Jet Zg; Number of Jets", 50, 0, 1);
    TH1F* hjtkt = new TH1F("hjtkt", "Jet Kt Distribution; Jet Kt (GeV/c); Number of Jets", 100, 0, 5);
    TH1F* hrawpt = new TH1F("hrawpt", "Raw Jet pT Distribution; Raw Jet pT (GeV/c); Number of Jets", nptBins, ptBins);

    TH1F* hrefpt = new TH1F("hrefpt", "Reference Jet pT Distribution; Reference Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hrefeta = new TH1F("hrefeta", "Reference Jet eta Distribution; Reference Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hrefphi = new TH1F("hrefphi", "Reference Jet phi Distribution; Reference Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hrefrg = new TH1F("hrefrg", "Reference Jet RG Distribution; Reference Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hrefzg = new TH1F("hrefzg", "Reference Jet Zg Distribution; Reference Jet Zg; Number of Jets", 50, 0, 1);
    TH1F* hrefkt = new TH1F("hrefkt", "Reference Jet Kt Distribution; Reference Jet Kt (GeV/c); Number of Jets", 100, 0, 5);

    TH1F* hgenpt = new TH1F("hgenpt", "Generated Jet pT Distribution; Generated Jet pT (GeV/c); Number of Jets", nptBins, ptBins);
    TH1F* hgeneta = new TH1F("hgeneta", "Generated Jet eta Distribution; Generated Jet eta; Number of Jets", 100, -5, 5);
    TH1F* hgenphi = new TH1F("hgenphi", "Generated Jet phi Distribution; Generated Jet phi; Number of Jets", 100, -TMath::Pi(), TMath::Pi());
    TH1F* hgenrg = new TH1F("hgenrg", "Generated Jet RG Distribution; Generated Jet RG; Number of Jets", nrgBins, rgBins);
    TH1F* hgenzg = new TH1F("hgenzg", "Generated Jet Zg Distribution; Generated Jet Zg; Number of Jets", 50, 0, 1);
    TH1F* hgenkt = new TH1F("hgenkt", "Generated Jet Kt Distribution; Generated Jet Kt (GeV/c); Number of Jets", 100, 0, 5);

    TH2F* hperformance = new TH2F("hperformance", "Jet Performance; Reference Jet pT (GeV/c); Response (Raw pT / Reference pT)", nptBins, ptBins, 100, 0, 2);
    gStyle->SetOptStat(0); // Disable statistics box

    HltTree->SetBranchStatus("*", 0); // Disable all branches
    HiEvtAnalyzersTree->SetBranchStatus("*", 0); // Disable all branches
    PPTracksTree->SetBranchStatus("*", 0); // Disable all branches
    if(!MC){
        SkimTree->SetBranchStatus("*", 0); // Disable all branches
        SkimTree->SetBranchStatus("pprimaryVertexFilter", 1);
    }
    //HltTree->SetBranchStatus("L1_MinimumBiasHF1_OR_BptxAND", 1);
    //HltTree->SetBranchStatus("L1_SingleJet28_BptxAND",1);

    if (L1MinBiasBool){
        HltTree->SetBranchStatus("L1_MinimumBiasHF0_AND_BptxAND", 1);
    }
    if (L1ZeroBiasBool){
        HltTree->SetBranchStatus("L1_ZeroBias", 1);
        HltTree->SetBranchStatus("L1_ZeroBias_Prescl", 1);
    }
    
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

    if(MC){
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

    for (size_t filenum = 0; filenum < rootFiles.size(); ++filenum) {
        const string& file = rootFiles[filenum];
       // std::string eosFileName = "root://eoscms.cern.ch/" + file;
        cout << "Processing file " << (filenum + 1) << " of " << rootFiles.size() << ": " << file << endl;

        JetAnalyserTree->Add(file.c_str());
        HltTree->Add(file.c_str());
        PPTracksTree->Add(file.c_str());
        HiEvtAnalyzersTree->Add(file.c_str());
        if(!MC){
            SkimTree->Add(file.c_str());
        }
        if (JetAnalyserTree->GetEntries() > nevents && nevents > 0){
            cout << "Reached event limit of " << nevents << ". Stopping file addition." << endl;
            cout << endl;
            break;
        }
    }

    if (!MC){
        JetAnalyserTree->AddFriend(SkimTree, "skimanalysis");
    }

    JetAnalyserTree->AddFriend(HltTree, "hltanalysis");
    JetAnalyserTree->AddFriend(PPTracksTree, "ppTracks");
    JetAnalyserTree->AddFriend(HiEvtAnalyzersTree, "hiEvtAnalyzer");
    
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
 
    JetAnalyserTree->SetBranchAddress("jtPfCHF",jtPfCHF);
    JetAnalyserTree->SetBranchAddress("jtPfNHF",jtPfNHF);
    JetAnalyserTree->SetBranchAddress("jtPfCEF",jtPfCEF);
    JetAnalyserTree->SetBranchAddress("jtPfNEF",jtPfNEF);
    JetAnalyserTree->SetBranchAddress("jtPfMUF",jtPfMUF);
    JetAnalyserTree->SetBranchAddress("jtPfCHM",jtPfCHM);

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

    //JetAnalyserTree->SetBranchAddress("hltanalysis.L1_MinimumBiasHF1_OR_BptxAND",&L1_MinimumBiasHF1_OR_BptxAND);
   // JetAnalyserTree->SetBranchAddress("hltanalysis.L1_SingleJet28_BptxAND",&L1_SingleJet28_BptxAND);

    JetAnalyserTree->SetBranchAddress("hiEvtAnalyzer.hiHFMinus_pf",&hiHFMinus_pf);
    JetAnalyserTree->SetBranchAddress("hiEvtAnalyzer.hiHFEPlus_pf",&hiHFEPlus_pf);

    if(!MC){
        JetAnalyserTree->SetBranchAddress("skimanalysis.pprimaryVertexFilter",&pprimaryVertexFilter);
    }
    if(MC){
        JetAnalyserTree->SetBranchAddress("hiEvtAnalyzer.pthat",&ptHat);
        JetAnalyserTree->SetBranchAddress("hiEvtAnalyzer.weight",&weight);
    }
    else{
        weight = 1.0;
    }
    
    //PPTracks variables
    int nvtx;
    vector<float>* zVtx = nullptr;

    JetAnalyserTree->SetBranchAddress("ppTracks.nVtx",&nvtx);
    JetAnalyserTree->SetBranchAddress("ppTracks.zVtx",&zVtx);
    if (L1MinBiasBool){
        JetAnalyserTree->SetBranchAddress("hltanalysis.L1_MinimumBiasHF0_AND_BptxAND", &Trigger);
    }
    else if (L1ZeroBiasBool){
        JetAnalyserTree->SetBranchAddress("hltanalysis.L1_ZeroBias", &Trigger);
        JetAnalyserTree->SetBranchAddress("hltanalysis.L1_ZeroBias_Prescl", &TriggerPrescale);
    }

    // Event counting variables
    Long64_t eventsBeforeCuts = 0;
    Long64_t eventsAfterCuts = 0;
    Long64_t jetsBeforeSelection = 0;
    Long64_t jetsAfterSelection = 0;
    Long64_t jetsbeforeTrigger = 0;
    Long64_t jetsafterTrigger=0;
    Long64_t totaljets = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    cout << "Starting processing..." << endl;
    vector<string> Files;
    for (string file : JECtxtvector){
        Files.push_back(file);
    }
    
    JetCorrector JEC(Files);
    JetSelect js(JetVetoMap);
    
    int nEntries = JetAnalyserTree->GetEntries();
    for (Long64_t entrynum = 0; entrynum < nEntries; entrynum++){
        JetAnalyserTree->GetEntry(entrynum);
        totaljets += nref;
        eventsBeforeCuts++;

        if (entrynum % 1000000 == 0){
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
            cout << "Total jets processed so far: " << totaljets << endl;
            cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start_time).count() << " seconds" << endl;
            cout << "Current memory usage: " << current_memory_mb << " MB (+" << memory_increase << " MB from start)" << endl;
            cout << endl;
        }
        jetsbeforeTrigger++;
        if (Trigger != 1) continue;
        jetsafterTrigger++;

        if (PVFilterBool == 1 && !MC){
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
            if(!js.JetSelection_pp(jteta[j], jtphi[j], jtPfCEF[j], jtPfNEF[j],jtPfMUF[j],
                        jtPfNHF[j], jtPfCHF[j], jtPfCHM[j])){continue;}
            
            jetsAfterSelection++;

            JEC.SetJetPT(rawpt[j]);
            JEC.SetJetEta(jteta[j]);
            JEC.SetJetPhi(jtphi[j]);  
            Float_t jet_pt_corr = JEC.GetCorrectedPT();
            
            float genjetpt;
            if (MC){
                genjetpt = refpt[j];
            } else {
                genjetpt = jet_pt_corr;
            }

            float totalWeight = weight * TriggerPrescale;
            float response = rawpt[j] / genjetpt;

            if (JetPtCutBool == 1){
                if (genjetpt < jtptminCut) continue;
                if (genjetpt > jtptmaxCut) continue;
                if (TMath::Abs(jteta[j]) > etaCut) continue;
            }

            hjtpt->Fill(jtpt[j], totalWeight);
            hrawpt->Fill(rawpt[j], totalWeight);
            hcorrectedpt->Fill(jet_pt_corr, totalWeight);

            hjteta->Fill(jteta[j], totalWeight);
            hjtphi->Fill(jtphi[j], totalWeight);
            hjtrg->Fill(jtrg[j], totalWeight);
            hjtzg->Fill(jtzg[j], totalWeight);
            hjtkt->Fill(jtkt[j], totalWeight);

            if(MC){
                hrefpt->Fill(refpt[j], totalWeight);
                hrefeta->Fill(refeta[j], totalWeight);
                hrefphi->Fill(refphi[j], totalWeight);
                hrefrg->Fill(refrg[j], totalWeight);
                hrefzg->Fill(refzg[j], totalWeight);
                hrefkt->Fill(refkt[j], totalWeight);

                hgenpt->Fill(genpt[j], totalWeight);
                hgeneta->Fill(geneta[j], totalWeight);
                hgenphi->Fill(genphi[j], totalWeight);
                hgenrg->Fill(genrg[j], totalWeight);
                hgenzg->Fill(genzg[j], totalWeight);
                hgenkt->Fill(genkt[j], totalWeight);
            }
            hperformance->Fill(genjetpt, response, totalWeight);
        }
        
        if (entrynum > nevents && nevents != 0){
            cout << "Debug mode: stopping after" << nevents << "entries" << endl;
            break;
        }
    }
    string nentriesLabel = FormatNEntriesLabel(static_cast<long long>(eventsBeforeCuts));
    string outFileName = Form("/home/xirong/JetStudiesOO/LynnsCode/RootFiles/%s/%s_%sEvts_%s_%s.root",
                                outfoldername.c_str(), sampleType.c_str(), nentriesLabel.c_str(), outfiletag.c_str(), date.c_str());

    TFile* outFile = new TFile(outFileName.c_str(), "RECREATE");
    
    if (!outFile || outFile->IsZombie()) {
        cout << "ERROR: Could not create output file: " << outFileName << endl;
        return;
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
    generalInfoString += Form("TotalJetsProcessed: %lld;", totaljets);
    if(MC) {
        generalInfoString += Form("MCWeighted: Yes;");
    } else {
        generalInfoString += Form("MCWeighted: No;");
    }
    
    // Add individual cut efficiencies if cuts were applied
    
    TNamed* generalInfo = new TNamed("GeneralInfo", generalInfoString.c_str());
    cout << "Cuts applied: " << cutString << endl;
    cout << "General info: " << generalInfoString << endl;
    
    cout << "jets before trigger" << jetsbeforeTrigger << endl;
    cout << "jets after trigger" << jetsafterTrigger << endl;
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
    hrawpt->Write();
    hcorrectedpt->Write();
    hjteta->Write();
    hjtphi->Write();
    hjtrg->Write();
    hjtzg->Write();
    hjtkt->Write();
    hperformance->Write();
    if (MC){
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
    }
    outFile->cd("EventLevelHistRaw");

    // Close output file first
    outFile->Close();
    delete outFile;
    
    // Delete TNamed objects
    delete cutInfo;
    delete generalInfo;

    // Delete all histograms
    delete hjtpt; delete hjteta; delete hjtphi;
    delete hjtrg; delete hjtzg; delete hjtkt; 

    // Delete TChains
    delete JetAnalyserTree;
    if (!MC){
        delete SkimTree;
    }
    delete HltTree;
    delete PPTracksTree;
    delete HiEvtAnalyzersTree;
}

