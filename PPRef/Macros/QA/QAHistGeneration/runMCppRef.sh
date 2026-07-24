#!/bin/bash

MC=1
neventcut=0 # 0 for full
outfolder="PPRef/0722_CorrectJetID_SU24Correction_CorrectCode/MC/"
logpath="/home/xirong/JetStudiesOO/LynnsCode/RootFiles/${outfolder}/log/"
tagMC2=""
# pt bins
ptbins=(
  "30 1000"
  #"80 100"
 # "100 300"
  #"300 700"
)
jetTreeName="ak4PFJetAnalyzer/t"
jetVetoMap="Winter24Prompt24_2024BCDEFGHI.root"
CorrectionFileL2Relative="Prompt24HIpp_V1_MC_L2Relative_AK4PF.txt"
CorrectionFileNewL1FastJet="Summer24Prompt24/Summer23BPixPrompt23_V1_MC_L1FastJet_AK4PFPuppi.txt"
CorrectionFileNewL2Relative="Summer24Prompt24/Summer24Prompt24_V1_MC_L2Relative_AK4PFPuppi.txt"
CorrectionFileNewL3Absolute="Summer24Prompt24/Summer23BPixPrompt23_V1_MC_L3Absolute_AK4PFPuppi.txt"

if [ ! -d "$logpath" ]; then
    mkdir -p "$logpath"
fi

# input paths + tags
declare -A inputsMC
inputsMC["MC"]="/eos/cms/store/group/phys_heavyions/xirong/Run3_ppref_MC_withrg/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/crab_ppref_5362GeV_MC_0702_withrg_rawpt/260709_053918/0000/"

for tagMC in "${!inputsMC[@]}"; do
    infilepath="${inputsMC[$tagMC]}"
    echo "Tag: $tagMC"

    for ptbin in "${ptbins[@]}"; do
        read jtptmin jtptmax <<< "$ptbin"

        outfiletag="${tagMC}_${tagMC2}pt${jtptmin}to${jtptmax}"

        echo "Running $outfiletag"

        nohup ./SaveHistChain \
            $MC \
            $jtptmin \
            $jtptmax \
            $neventcut \
            $outfolder \
            $infilepath \
            $outfiletag \
            $jetTreeName \
            $jetVetoMap \
            $CorrectionFileNewL1FastJet \
            $CorrectionFileNewL2Relative \
            $CorrectionFileNewL3Absolute \
            > $logpath/executable_MC${MC}_${tagMC2}_${tagMC}.log 2>&1 &
    done
done