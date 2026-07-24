from CRABClient.UserUtilities import config
from CRABClient.UserUtilities import getUsername
username = getUsername()

###############################################################################
# INPUT/OUTPUT SETTINGS

jobTag = 'ppref_5362GeV_MC_0702_withrg_rawpt'
#input = '/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/RunIIIpp5p36Winter24DR-NoPU_FEVTDEBUGHLT_forTracking_141X_mcRun3_2024_realistic_ppRef5TeV_v7-v4/GEN-SIM-RECODEBUG'
input = '/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/RunIIIpp5p36Winter24MiniAOD-141X_mcRun3_2024_realistic_ppRef5TeV_v7-v2/MINIAODSIM'
inputDatabase = 'global'
output = '/store/group/phys_heavyions/' + username + '/Run3_ppref_MC_withrg/'
outputServer = 'T2_CH_CERN'

###############################################################################

config = config()

config.General.requestName = jobTag
config.General.workArea = 'CrabWorkArea'
config.General.transferOutputs = True

config.JobType.psetName = 'forest_miniAOD_run3_ppref_MC.py'
config.JobType.pluginName = 'Analysis'
config.JobType.maxMemoryMB = 3000
config.JobType.pyCfgParams = ['noprint']
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = input
config.Data.inputDBS = inputDatabase
config.Data.outLFNDirBase = output
config.Data.splitting = 'FileBased'
#Das based config
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1
config.Data.publication = False
config.Data.allowNonValidInputDataset = True

config.Site.storageSite = outputServer
