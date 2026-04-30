//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Apr  9 19:45:08 2026 by ROOT version 6.32.13
// from TTree t/ Jet Analysis Tree
// found on file: root://eoscms.cern.ch//eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/HiForestMiniAOD_1.root
//////////////////////////////////////////////////////////

#ifndef MyTree_h
#define MyTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class MyTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           run;
   Int_t           evt;
   Int_t           lumi;
   Int_t           nref;
   Int_t           ncalo;
   Float_t         rawpt[29];   //[nref]
   Float_t         jtpt[29];   //[nref]
   Float_t         jteta[29];   //[nref]
   Float_t         jty[29];   //[nref]
   Float_t         jtphi[29];   //[nref]
   Float_t         jtpu[29];   //[nref]
   Float_t         jtm[29];   //[nref]
   Float_t         jtarea[29];   //[nref]
   Int_t           ncalo;
   Float_t         calopt[48];   //[ncalo]
   Float_t         caloeta[48];   //[ncalo]
   Float_t         calophi[48];   //[ncalo]
   Float_t         jtPfCHF[29];   //[nref]
   Float_t         jtPfNHF[29];   //[nref]
   Float_t         jtPfCEF[29];   //[nref]
   Float_t         jtPfNEF[29];   //[nref]
   Float_t         jtPfMUF[29];   //[nref]
   Int_t           jtPfCHM[29];   //[nref]
   Int_t           jtPfNHM[29];   //[nref]
   Int_t           jtPfCEM[29];   //[nref]
   Int_t           jtPfNEM[29];   //[nref]
   Int_t           jtPfMUM[29];   //[nref]
   Float_t         jttau1[29];   //[nref]
   Float_t         jttau2[29];   //[nref]
   Float_t         jttau3[29];   //[nref]
   Float_t         jtzg[29];   //[nref]
   Float_t         jtrg[29];   //[nref]
   Float_t         jtkt[29];   //[nref]
   Float_t         jtangu[29];   //[nref]
   Float_t         pthat;
   Float_t         refpt[29];   //[nref]
   Float_t         refeta[29];   //[nref]
   Float_t         refy[29];   //[nref]
   Float_t         refphi[29];   //[nref]
   Float_t         refm[29];   //[nref]
   Float_t         refarea[29];   //[nref]
   Float_t         refdphijt[29];   //[nref]
   Float_t         refdrjt[29];   //[nref]
   Float_t         refparton_pt[29];   //[nref]
   Int_t           refparton_flavor[29];   //[nref]
   Int_t           refparton_flavorForB[29];   //[nref]
   Float_t         refzg[29];   //[nref]
   Float_t         refrg[29];   //[nref]
   Float_t         refkt[29];   //[nref]
   Float_t         refangu[29];   //[nref]
   Float_t         genChargedSum[29];   //[nref]
   Float_t         genHardSum[29];   //[nref]
   Float_t         signalChargedSum[29];   //[nref]
   Float_t         signalHardSum[29];   //[nref]
   Int_t           ngen;
   Int_t           genmatchindex[25];   //[ngen]
   Float_t         genpt[25];   //[ngen]
   Float_t         geneta[25];   //[ngen]
   Float_t         geny[25];   //[ngen]
   Float_t         genphi[25];   //[ngen]
   Float_t         genm[25];   //[ngen]
   Float_t         gendphijt[25];   //[ngen]
   Float_t         gendrjt[25];   //[ngen]
   Float_t         genzg[25];   //[ngen]
   Float_t         genrg[25];   //[ngen]
   Float_t         genkt[25];   //[ngen]
   Float_t         genangu[25];   //[ngen]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_nref;   //!
   TBranch        *b_ncalo;   //!
   TBranch        *b_rawpt;   //!
   TBranch        *b_jtpt;   //!
   TBranch        *b_jteta;   //!
   TBranch        *b_jty;   //!
   TBranch        *b_jtphi;   //!
   TBranch        *b_jtpu;   //!
   TBranch        *b_jtm;   //!
   TBranch        *b_jtarea;   //!
   TBranch        *b_ncalo;   //!
   TBranch        *b_calopt;   //!
   TBranch        *b_caloeta;   //!
   TBranch        *b_calophi;   //!
   TBranch        *b_jtPfCHF;   //!
   TBranch        *b_jtPfNHF;   //!
   TBranch        *b_jtPfCEF;   //!
   TBranch        *b_jtPfNEF;   //!
   TBranch        *b_jtPfMUF;   //!
   TBranch        *b_jtPfCHM;   //!
   TBranch        *b_jtPfNHM;   //!
   TBranch        *b_jtPfCEM;   //!
   TBranch        *b_jtPfNEM;   //!
   TBranch        *b_jtPfMUM;   //!
   TBranch        *b_jttau1;   //!
   TBranch        *b_jttau2;   //!
   TBranch        *b_jttau3;   //!
   TBranch        *b_jtzg;   //!
   TBranch        *b_jtrg;   //!
   TBranch        *b_jtkt;   //!
   TBranch        *b_jtangu;   //!
   TBranch        *b_pthat;   //!
   TBranch        *b_refpt;   //!
   TBranch        *b_refeta;   //!
   TBranch        *b_refy;   //!
   TBranch        *b_refphi;   //!
   TBranch        *b_refm;   //!
   TBranch        *b_refarea;   //!
   TBranch        *b_refdphijt;   //!
   TBranch        *b_refdrjt;   //!
   TBranch        *b_refparton_pt;   //!
   TBranch        *b_refparton_flavor;   //!
   TBranch        *b_refparton_flavorForB;   //!
   TBranch        *b_refzg;   //!
   TBranch        *b_refrg;   //!
   TBranch        *b_refkt;   //!
   TBranch        *b_refangu;   //!
   TBranch        *b_genChargedSum;   //!
   TBranch        *b_genHardSum;   //!
   TBranch        *b_signalChargedSum;   //!
   TBranch        *b_signalHardSum;   //!
   TBranch        *b_ngen;   //!
   TBranch        *b_genmatchindex;   //!
   TBranch        *b_genpt;   //!
   TBranch        *b_geneta;   //!
   TBranch        *b_geny;   //!
   TBranch        *b_genphi;   //!
   TBranch        *b_genm;   //!
   TBranch        *b_gendphijt;   //!
   TBranch        *b_gendrjt;   //!
   TBranch        *b_genzg;   //!
   TBranch        *b_genrg;   //!
   TBranch        *b_genkt;   //!
   TBranch        *b_genangu;   //!

   MyTree(TTree *tree=0);
   virtual ~MyTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MyTree_cxx
MyTree::MyTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root://eoscms.cern.ch//eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/HiForestMiniAOD_1.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root://eoscms.cern.ch//eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/HiForestMiniAOD_1.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("root://eoscms.cern.ch//eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/HiForestMiniAOD_1.root:/akCs4PFJetAnalyzer");
      dir->GetObject("t",tree);

   }
   Init(tree);
}

MyTree::~MyTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MyTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MyTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void MyTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("nref", &nref, &b_nref);
   fChain->SetBranchAddress("ncalo", &ncalo, &b_ncalo);
   fChain->SetBranchAddress("rawpt", rawpt, &b_rawpt);
   fChain->SetBranchAddress("jtpt", jtpt, &b_jtpt);
   fChain->SetBranchAddress("jteta", jteta, &b_jteta);
   fChain->SetBranchAddress("jty", jty, &b_jty);
   fChain->SetBranchAddress("jtphi", jtphi, &b_jtphi);
   fChain->SetBranchAddress("jtpu", jtpu, &b_jtpu);
   fChain->SetBranchAddress("jtm", jtm, &b_jtm);
   fChain->SetBranchAddress("jtarea", jtarea, &b_jtarea);
//    fChain->SetBranchAddress("ncalo", &ncalo, &b_ncalo);
   fChain->SetBranchAddress("calopt", calopt, &b_calopt);
   fChain->SetBranchAddress("caloeta", caloeta, &b_caloeta);
   fChain->SetBranchAddress("calophi", calophi, &b_calophi);
   fChain->SetBranchAddress("jtPfCHF", jtPfCHF, &b_jtPfCHF);
   fChain->SetBranchAddress("jtPfNHF", jtPfNHF, &b_jtPfNHF);
   fChain->SetBranchAddress("jtPfCEF", jtPfCEF, &b_jtPfCEF);
   fChain->SetBranchAddress("jtPfNEF", jtPfNEF, &b_jtPfNEF);
   fChain->SetBranchAddress("jtPfMUF", jtPfMUF, &b_jtPfMUF);
   fChain->SetBranchAddress("jtPfCHM", jtPfCHM, &b_jtPfCHM);
   fChain->SetBranchAddress("jtPfNHM", jtPfNHM, &b_jtPfNHM);
   fChain->SetBranchAddress("jtPfCEM", jtPfCEM, &b_jtPfCEM);
   fChain->SetBranchAddress("jtPfNEM", jtPfNEM, &b_jtPfNEM);
   fChain->SetBranchAddress("jtPfMUM", jtPfMUM, &b_jtPfMUM);
   fChain->SetBranchAddress("jttau1", jttau1, &b_jttau1);
   fChain->SetBranchAddress("jttau2", jttau2, &b_jttau2);
   fChain->SetBranchAddress("jttau3", jttau3, &b_jttau3);
   fChain->SetBranchAddress("jtzg", jtzg, &b_jtzg);
   fChain->SetBranchAddress("jtrg", jtrg, &b_jtrg);
   fChain->SetBranchAddress("jtkt", jtkt, &b_jtkt);
   fChain->SetBranchAddress("jtangu", jtangu, &b_jtangu);
   fChain->SetBranchAddress("pthat", &pthat, &b_pthat);
   fChain->SetBranchAddress("refpt", refpt, &b_refpt);
   fChain->SetBranchAddress("refeta", refeta, &b_refeta);
   fChain->SetBranchAddress("refy", refy, &b_refy);
   fChain->SetBranchAddress("refphi", refphi, &b_refphi);
   fChain->SetBranchAddress("refm", refm, &b_refm);
   fChain->SetBranchAddress("refarea", refarea, &b_refarea);
   fChain->SetBranchAddress("refdphijt", refdphijt, &b_refdphijt);
   fChain->SetBranchAddress("refdrjt", refdrjt, &b_refdrjt);
   fChain->SetBranchAddress("refparton_pt", refparton_pt, &b_refparton_pt);
   fChain->SetBranchAddress("refparton_flavor", refparton_flavor, &b_refparton_flavor);
   fChain->SetBranchAddress("refparton_flavorForB", refparton_flavorForB, &b_refparton_flavorForB);
   fChain->SetBranchAddress("refzg", refzg, &b_refzg);
   fChain->SetBranchAddress("refrg", refrg, &b_refrg);
   fChain->SetBranchAddress("refkt", refkt, &b_refkt);
   fChain->SetBranchAddress("refangu", refangu, &b_refangu);
   fChain->SetBranchAddress("genChargedSum", genChargedSum, &b_genChargedSum);
   fChain->SetBranchAddress("genHardSum", genHardSum, &b_genHardSum);
   fChain->SetBranchAddress("signalChargedSum", signalChargedSum, &b_signalChargedSum);
   fChain->SetBranchAddress("signalHardSum", signalHardSum, &b_signalHardSum);
   fChain->SetBranchAddress("ngen", &ngen, &b_ngen);
   fChain->SetBranchAddress("genmatchindex", genmatchindex, &b_genmatchindex);
   fChain->SetBranchAddress("genpt", genpt, &b_genpt);
   fChain->SetBranchAddress("geneta", geneta, &b_geneta);
   fChain->SetBranchAddress("geny", geny, &b_geny);
   fChain->SetBranchAddress("genphi", genphi, &b_genphi);
   fChain->SetBranchAddress("genm", genm, &b_genm);
   fChain->SetBranchAddress("gendphijt", gendphijt, &b_gendphijt);
   fChain->SetBranchAddress("gendrjt", gendrjt, &b_gendrjt);
   fChain->SetBranchAddress("genzg", genzg, &b_genzg);
   fChain->SetBranchAddress("genrg", genrg, &b_genrg);
   fChain->SetBranchAddress("genkt", genkt, &b_genkt);
   fChain->SetBranchAddress("genangu", genangu, &b_genangu);
   Notify();
}

bool MyTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void MyTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MyTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MyTree_cxx
