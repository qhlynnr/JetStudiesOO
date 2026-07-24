from CRABClient.UserUtilities import config
from CRABClient.UserUtilities import getUsername
username = getUsername()

###############################################################################
# INPUT/OUTPUT SETTINGS

jobTag = 'ppref_5362GeV_Data_withrg_rawpt_PD4'
input = '/PPRefHardProbes4/Run2024J-PromptReco-v1/MINIAOD'
inputDatabase = 'global'
output = '/store/group/phys_heavyions/' + username + '/Run3_ppref_Data_withrg/'
outputServer = 'T2_CH_CERN'

###############################################################################

config = config()

config.General.requestName = jobTag
config.General.workArea = 'CrabWorkArea'
config.General.transferOutputs = True

config.JobType.psetName = 'forest_miniAOD_run3_ppref_DATA.py'
config.JobType.pluginName = 'Analysis'
config.JobType.maxMemoryMB = 2500
config.JobType.pyCfgParams = ['noprint']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['Prompt25HIOO_V1_MC_L2Relative_AK4PF.txt']

config.Data.inputDataset = input
config.Data.inputDBS = inputDatabase
config.Data.outLFNDirBase = output
config.Data.lumiMask = '/afs/cern.ch/user/x/xirong/CMSSW_14_1_9_patch2/src/jsonfiles/Cert_Collisions2024_ppref_387474_387721_golden.json'
#config.Data.lumiMask = '/afs/cern.ch/user/x/xirong/CMSSW_14_1_9_patch2/src/CrabWorkArea/crab_ppref_5362GeV_Data_withrg_rawpt/results/notFinishedLumis.json'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1
config.Data.publication = False
config.Data.allowNonValidInputDataset = True
config.Data.ignoreLocality = True
config.Site.whitelist = []  # leave empty, let the matchmaker find any capable site
config.Site.blacklist = []  # or exclude IIHE/Vanderbilt specifically if you want to force diversification, though it's optional since Automatic splitting + bigger memory ask should let the matchmaker sort it out

config.Site.storageSite = outputServer
