### HiForest CMSSW Configuration
# Collisions: Oxygen-Oxygen
# Input: miniAOD
# Type: data

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_2025_OXY_cff import Run3_2025_OXY
process = cms.Process('HiForest', Run3_2025_OXY)

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 150X, data")

###############################################################################

# input files
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
         '/store/hidata/OORun2025/IonPhysics0/MINIAOD/PromptReco-v1/000/394/154/00000/115d70a4-cdc7-488f-b2b9-2bccad75b5ee.root'
    ),
)

#include the OO golden json
# crab will do this automatically
# import FWCore.PythonUtilities.LumiList as LumiList
# process.source.lumisToProcess = LumiList.LumiList(filename = '/eos/user/c/cmsdqm/www/CAF/certification/Collisions25OO/Cert_Collisions2025OO_394153_394217_golden.json').getVLuminosityBlockRange()


# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

###############################################################################

# load Global Tag, geometry, etc.
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '150X_dataRun3_Prompt_v3', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

###############################################################################

# Define centrality binning
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")

###############################################################################

# root output
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD.root"))

# # edm output for debugging purposes
# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('HiForestEDM.root'),
#     outputCommands = cms.untracked.vstring(
#         'keep *',
#     )
# )

# process.output_path = cms.EndPath(process.output)

###############################################################################

# event analysis
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('L1Trigger.L1TNtuples.l1MetFilterRecoTree_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
process.particleFlowAnalyser.ptMin = cms.double(0.)
process.particleFlowAnalyser.absEtaMax = cms.double(6.)
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.hiEvtAnalyzer.doHFfilters = cms.bool(False)
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')
# process.hiEvtAnalyzer.doCentrality = cms.bool(False) # used for UPC

# add L1 MET filter

################################
# electrons, photons, muons
#process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
#process.ggHiNtuplizer.doMuons = cms.bool(False)
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
################################

################################
# jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.akCs4PFJetSequence_pponPbPb_data_cff')


# add the unsubtracted jet collection
process.load('HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_data_cff')

# add rho analyzer for CS studies
process.load('HeavyIonsAnalysis.JetAnalysis.hiFJRhoAnalyzer_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.hiFlowRhoAnalyzer_cff')
process.load('HeavyIonsAnalysis.JetAnalysis.RandomConeAnalysis_cff')

################################

# tracks
process.load("HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff")
# muons
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")
###############################################################################

#########################
# ZDC RecHit Producer && Analyzer
#########################
## to prevent crash related to HcalSeverityLevelComputerRcd record
#process.load("RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi")
#process.load('HeavyIonsAnalysis.ZDCAnalysis.ZDCAnalyzersPbPb_cff')
#
## =============================================================================
## ==================== modification needed for the fsc data ===================
#from CondCore.CondDB.CondDB_cfi import *
#process.es_pool = cms.ESSource("PoolDBESSource",
#    toGet = cms.VPSet(
#        cms.PSet(
#            record = cms.string("HcalElectronicsMapRcd"),
#            tag = cms.string("HcalElectronicsMap_v10.0_offline")
#        )
#    ),
#    connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
#)
#process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')
#process.es_ascii = cms.ESSource(
#    'HcalTextCalibrations',
#    input = cms.VPSet(
#        cms.PSet(
#            object = cms.string('ElectronicsMap'),
#            file = cms.FileInPath("emap_2025_full.txt")
#        )
#    )
#)
## =============================================================================

###############################################################################
# main forest sequence
process.forest = cms.Path(
    process.HiForestInfo +
    process.centralityBin +
    process.hltanalysis +
#    process.hltobject +
    process.l1object +
    process.l1MetFilterRecoTree +
    process.trackSequencePP +
    process.particleFlowAnalyser +
    process.rhoSequence +
    process.rhoFlowDataSequence +
    process.randomConeSequence + 
    process.hiEvtAnalyzer #+
#    process.zdcSequencePbPb
#    process.ggHiNtuplizer +
#    process.unpackedMuons +
#    process.muonAnalyzer
)

#########################
# Event Selection -> add the needed filters here
#########################
process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.load('HeavyIonsAnalysis.EventAnalysis.hffilterPF_cfi')
process.pAna = cms.EndPath(process.skimanalysis)


# add jets to the final process
process.forest += getattr(process,"ak4PFJetAnalyzer")

# process.HFAdcana = cms.EDAnalyzer("HFAdcToGeV",
#     digiLabel = cms.untracked.InputTag("hcalDigis"),
#     #digiLabel = cms.untracked.InputTag("simHcalUnsuppressedDigis","HFQIE10DigiCollection"),
#     minimized = cms.untracked.bool(True),
#     fillhf = cms.bool(False) # only turn this on when you have or know how to produce "towerMaker"
# )
# process.hfadc = cms.Path(process.HFAdcana)


# Select the types of jets filled
matchJets = False             # Enables q/g and heavy flavor jet identification in MC 
jetPtMin = 30 # changed by Hannah on 6/29 in an effort to save space, was 15
jetAbsEtaMax = 2.0 # changed by Hannah on 6/29 to save space, was 2.5

# Choose which additional information is added to jet trees
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = False        # Add jet phi and eta for WTA axis
doBtagging  =  False         # Note that setting to True increases computing time a lot

# Configuration for jet flow subtraction
iterativeFlow = True         # Iterative jetty region exclusion. Default = True
pfCandidateEtaCut = 2        # Eta range for PF candidates used in flow fit. Default = 2
minPfCandidatesPerEvent = 60 # Minimum number of PF candidates to make the flow fit. Default = 60
minPfCandidatePt = 0.3       # Minimum pT for PF candidates in flow fit. Default = 0.3
maxPfCandidatePt = 3         # Maximum pT for PF candidates in flow fit. Default = 3
minFitQuality = cms.double(0)           # Minimum flow fit quality score. Default = 0
maxFitQuality = cms.double(1)            # Maximum flow fit quality score. Default = 1
firstFittedVn = 2            # First fitted vn component. Default = 2
lastFittedVn = 3             # Last fitted vn component. Default = 3

# 0 means use original mini-AOD jets, otherwise use R value, e.g., 3,4,8
# Add all the values you want to process to the list
# These will create collections of CS subtracted jets (only eta dependent background)
jetLabelsCS = ["4"]

# For this list, give the R-values for flow subtracted CS jets (eta and phi dependent background)
jetLabelsFlowCS = ["4"]

# Combine the two lists such that all selected jets can be easily looped over
# Also add "Flow" tag for the flow jets to distinguish them from non-flow jets
allJetLabels = jetLabelsCS + [flowR + "Flow" for flowR in jetLabelsFlowCS]

# add candidate tagging
from HeavyIonsAnalysis.JetAnalysis.setupJets_PbPb_cff import candidateBtaggingMiniAOD

for jetLabel in allJetLabels:
    candidateBtaggingMiniAOD(process, isMC = False, jetPtMin = jetPtMin, jetCorrLevels = ['L2Relative', 'L2L3Residual'], doBtagging = doBtagging, labelR = jetLabel)

    # setup jet analyzer
    setattr(process,"akCs"+jetLabel+"PFJetAnalyzer",process.akCs4PFJetAnalyzer.clone())
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").jetTag = "selectedUpdatedPatJetsAK"+jetLabel+"PFBtag"
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").jetName = 'akCs'+jetLabel+'PF'
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").matchJets = matchJets
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").matchTag = 'patJetsAK'+jetLabel+'PFUnsubJets'
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").doBtagging = doBtagging
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").doHiJetID = doHIJetID
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").doWTARecluster = doWTARecluster
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").jetPtMin = jetPtMin
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").jetAbsEtaMax = cms.untracked.double(jetAbsEtaMax)
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").doIterativeDeclustering = cms.untracked.bool(True)
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").zcut = cms.double(0.2)
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").beta = cms.double(0.0)
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").useRawPt = cms.untracked.bool(False)
    getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").rParam = 0.4 if jetLabel=="0" else float(jetLabel.replace("Flow",""))*0.1
    if doBtagging:
        getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").pfJetProbabilityBJetTag = cms.untracked.string("pfJetProbabilityBJetTagsAK"+jetLabel+"PFBtag")
        getattr(process,"akCs"+jetLabel+"PFJetAnalyzer").pfUnifiedParticleTransformerAK4JetTags = cms.untracked.string("pfUnifiedParticleTransformerAK4JetTagsAK"+jetLabel+"PFBtag")
    process.forest += getattr(process,"akCs"+jetLabel+"PFJetAnalyzer")

# Configuration for the flow fit
for jetLabel in [flowR + "Flow" for flowR in jetLabelsFlowCS]:

    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").pfCandidateEtaCut = pfCandidateEtaCut
    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").minPfCandidatesPerEvent = minPfCandidatesPerEvent
    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").firstFittedVn = firstFittedVn
    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").lastFittedVn = lastFittedVn
    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").pfCandidateMinPtCut = minPfCandidatePt
    getattr(process, "rhoModulationAkCs"+jetLabel+"PFJets").pfCandidateMaxPtCut = maxPfCandidatePt
    getattr(process, "akCs"+jetLabel+"PFJets").minFlowChi2Prob = minFitQuality
    getattr(process, "akCs"+jetLabel+"PFJets").maxFlowChi2Prob = maxFitQuality

    if iterativeFlow:
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").pfCandidateEtaCut = pfCandidateEtaCut
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").minPfCandidatesPerEvent = minPfCandidatesPerEvent
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").firstFittedVn = firstFittedVn
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").lastFittedVn = lastFittedVn
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").pfCandidateMinPtCut = minPfCandidatePt
        getattr(process, "rhoModulationIterAkCs"+jetLabel+"PFJets").pfCandidateMaxPtCut = maxPfCandidatePt