// fill2025jet.C: quick macro to play with high pT jet sample from 2025 

#include <string>
#include <vector>
#include <iostream>
#include "JetSelection_PbPb.h"



// list of functions
void GetFiles(char const *input, vector<string> &files); 
void FillChain(TChain &chain, vector<string> &files); 
int GetCentBin(float hiHFval); 
void fill2025jets(); 

// get the files 
void GetFiles(char const *input, vector<string> &files) {
  TSystemDirectory dir(input, input);
  TList *list = dir.GetListOfFiles();

  if (list) {
    TSystemFile *file;
    string fname;
    TIter next(list);
    while ((file = (TSystemFile *)next())) {
      fname = file->GetName();

      if (file->IsDirectory() && (fname.find(".") == string::npos)) {
        string newDir = string(input) + fname + "/";
        GetFiles(newDir.c_str(), files);
      } else if ((fname.find(".root") != string::npos)) {
        files.push_back(string(input) + fname);
        cout << files.back() << endl;
      }
    }
  }

  return;
}

// fill the tchain
void FillChain(TChain &chain, vector<string> &files) {
  for (auto file : files) {
    chain.Add(file.c_str());
  }
}


// main function
void fillRandomCone(){


    char const *input = "/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/PYTHIA_HIJING_OO_MCEmbedded/260624_212945/";
    std::cout << "Running over " << input << std::endl; 

    /* read in all files in the input folder */
    vector<string> files;
    GetFiles(input, files);
    std::cout << "Done getting the files " << std::endl; 


    const double Rjet = 0.4; 

    /* read in event information */
    TChain hiEventChain("hiEvtAnalyzer/HiTree"); 
    FillChain(hiEventChain, files);
    TTreeReader hiEventReader(&hiEventChain);
    TTreeReaderValue<float>   HFpf(hiEventReader, "hiHF_pf");
    TTreeReaderValue<float>   weight(hiEventReader, "weight"); 
    TTreeReaderValue<float>   zVertex(hiEventReader, "vz");
    TTreeReaderValue<int>   nTrk(hiEventReader, "hiNtracks");  

    /* read in filter information */
    TChain eventChain("skimanalysis/HltTree"); 
    FillChain(eventChain, files);
    TTreeReader eventReader(&eventChain);
    TTreeReaderValue<int>   vertexFilter(eventReader, "pprimaryVertexFilter");
    TTreeReaderValue<int>   clusterFilter(eventReader, "pclusterCompatibilityFilter"); 

    /* read in rho information */
    TChain randomConeChain("randomConeAnalysisR4/t");
    FillChain(randomConeChain, files);
    TTreeReader randomConeReader(&randomConeChain);
    TTreeReaderValue<double> rconeEta(randomConeReader, "rcone_eta");
    TTreeReaderValue<double> rconePtraw(randomConeReader, "rcone_pt_raw");
    TTreeReaderValue<double> rconeDeltaPtBands(randomConeReader, "rcone_pt_bandsCorr");
    TTreeReaderValue<double> rconeDeltaPtAxis(randomConeReader, "rcone_pt_axisCorr");



    /* read in trigger information */
    TChain trigChain("hltanalysis/HltTree");
    FillChain(trigChain, files);
    TTreeReader trigReader(&trigChain);
    TTreeReaderValue<int> mb(trigReader, "HLT_MinimumBiasHF_OR_BptxAND_v1");



    /* declare the histograms */
    TH1D* hDeltaPtBands   = new TH1D("hDeltaPtBands", "", 200, -100, 100);
    TH1D* hDeltaPtAxis    = new TH1D("hDeltaPtAxis", "", 200, -100, 100);


    Long64_t totalEvents = randomConeReader.GetEntries(true);

    std::cout << " ---> Total number of events to process is " << totalEvents << std::endl;

    /* read in information from TTrees */
    for (Long64_t i = 0; i < totalEvents; i++) {
        randomConeReader.Next(); eventReader.Next(); trigReader.Next(); hiEventReader.Next(); 

        if (i % 20000 == 0) { 
            cout << "Entry: " << i << " / " <<  totalEvents << endl; 
        }
        
        // trigger selection
        if(*mb != 1) continue; 

        // z vertex filter 
        if(*zVertex < -15 || *zVertex > 15.0) continue; 

        // event filters
        if(*vertexFilter == 0 || *clusterFilter == 0) continue; 

        float w = *weight; 

        int cent = GetCentBin(*HFpf); 
        
        // skip events with centrality outside of the range
        if(cent < 0)continue; 

        // now fill the centrality and/or event histograms 
       hDeltaPtBands->Fill(*rconeDeltaPtBands, w); 
       hDeltaPtAxis->Fill(*rconeDeltaPtAxis, w); 
      
    } // end loop over the number of events

    // now take the rhoCent and calculate the rho per cent bin


    TFile* outFile = new TFile("RandomConeHistsMC_OOEmbedded_June26th.root", "RECREATE"); 
    outFile->cd(); 
    hDeltaPtBands->Write(); 
    hDeltaPtAxis->Write(); 


}

/* function to return the centrality bin */
int GetCentBin(float hiHFval){
  double vcent[201] ={0,0.91197,1.82394,2.73591,3.64788,4.55985,5.47182,6.38379,7.29576,8.20773,9.1197,10.0317,10.6667,10.7669,11.1084,11.4511,11.7967,12.1453,12.4985,12.8586,13.2279, 13.6047,13.9872,14.378,14.7734,15.1775,15.5875,16.0048,16.4344,16.8729,17.3249,17.7878,18.2572,18.7342,19.2173,19.7139,20.221,20.7368,21.2654,21.7955,22.3333,22.8819,23.446,24.0097,24.5881,25.179,25.7856,26.3914,27.0037,27.6247,28.2619,28.9063,29.5473,30.2005,30.8754,31.5574,32.2518,32.9558,33.6616,34.3925,35.1279,35.8711,36.618,37.3905,38.1709,38.9568,39.76,40.5757,41.3886,42.2135,43.0613,43.921,44.8002,45.6699,46.5569,47.4536,48.3673,49.2971,50.2437,51.1962,52.1656,53.1554,54.1508,55.1587,56.1668,57.1942,58.2508,59.3168,60.406,61.5058,62.6151,63.7513,64.8938,66.0488,67.2111,68.4189,69.6168,70.8422,72.0903,73.3433,74.6371,75.9344,77.2465,78.5776,79.9278,81.3019,82.7002,84.105,85.5389,86.9959,88.4836,89.9969,91.5244,93.0648,94.6373,96.2243,97.8198,99.454,101.105,102.798,104.466,106.186,107.925,109.692,111.487,113.265,115.092,116.936,118.856,120.819,122.768,124.744,126.732,128.739,130.809,132.907,135.033,137.173,139.34,141.543,143.777,146.028,148.298,150.674,153.07,155.464,157.912,160.369,162.846,165.358,167.907,170.503,173.094,175.76,178.495,181.231,183.991,186.784,189.674,192.575,195.507,198.523,201.592,204.653,207.796,210.983,214.245,217.554,220.881,224.256,227.741,231.249,234.786,238.444,242.096,245.842,249.627,253.523,257.577,261.697,265.88,270.16,274.601,279.162,283.888,288.745,293.767,298.968,304.526,310.308,316.408,322.893,329.956,337.457,345.746,354.995,365.685,378.512,395.16,420.836,526.238}; 
  int centrality = -999; 
  for(int index = 0; index < 201; index ++){
    if(hiHFval < vcent[index]){
      centrality = abs(200 - (index -1))/2; 
      break; 
    }
  }

  if(centrality < 0) std::cout << "Error in centrality determination, result = " << centrality  << " with HiHFval " << hiHFval << std::endl;

  return centrality; 
}