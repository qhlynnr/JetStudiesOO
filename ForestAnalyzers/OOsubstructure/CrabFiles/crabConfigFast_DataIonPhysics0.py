from WMCore.Configuration import Configuration

config = Configuration()

config.section_("General")
config.General.requestName = "OO_Data_PromptReco_IonPhsyics0"
config.General.transferLogs = False

config.section_("JobType")
config.JobType.pluginName = "Analysis"
config.JobType.psetName = "forest_CMSSWConfig_Run3_OO_DATA_miniAOD.py"
config.JobType.maxMemoryMB = 3000
config.JobType.maxJobRuntimeMin = 1140
config.JobType.inputFiles = ['Prompt25HIOO_V1_MC_L2Relative_AK4PF.txt']

config.section_("Data")
config.Data.inputDataset = "/IonPhysics0/OORun2025-PromptReco-v1/MINIAOD"
config.Data.inputDBS = "global"
config.Data.splitting = 'LumiBased'
config.Data.lumiMask = 'https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions25OO/Cert_Collisions2025OO_394153_394217_golden.json'
config.Data.unitsPerJob = 4 ## Number of *input* lumi sections per job ! 
config.Data.outputDatasetTag = "OO_Data_PromptReco_IonPhsyics0"
config.Data.outLFNDirBase = "/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests"
config.Data.publication = False
# allow for production based datasets to work
config.Data.allowNonValidInputDataset = True

config.section_("Site")
config.Site.storageSite = "T2_CH_CERN"
