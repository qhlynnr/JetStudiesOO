#!/bin/bash

MC=1
neventcut=100000 # 0 for full
outfolder="PPRef/070926PPRefMCData/Test/"
logpath="/home/xirong/JetStudiesOO/LynnsCode/RootFiles/${outfolder}/log/"
tagMC2="test"
# pt bins
ptbins=(
  "30 1000"
  #"80 100"
 # "100 300"
  #"300 700"
)
jetTreeName="ak4PFJetAnalyzer/t"
jetVetoMap="Winter24Prompt24_2024BCDEFGHI.root"
CorrectionFile="Prompt24HIpp_V1_MC_L2Relative_AK4ChsPF.txt"
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

        outfiletag="${tagMC}_${tagMC2}_Pt${jtptmin}to${jtptmax}"

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
            $CorrectionFile \
            > $logpath/executable_MC${MC}_${tagMC2}_pt${jtptmin}to${jtptmax}_${tagMC}.log 2>&1 &
    done
done