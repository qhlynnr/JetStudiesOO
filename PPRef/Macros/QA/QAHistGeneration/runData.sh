#!/bin/bash

MC=0
neventcut=0 # 0 for full
outfolder="PPRef/0723Testing/Data/"
logpath="../../../RootFiles/${outfolder}/log/"
tag2=""
# pt bins
ptbins=(
  "30 1000"
  #"80 100"
 # "100 300"
 # "300 700"
)
jetTreeName="ak4PFJetAnalyzer/t"
jetVetoMap="Winter24Prompt24_2024BCDEFGHI.root"
CorrectionFileL2Relative="Prompt24HIpp_V1_MC_L2Relative_AK4PF.txt"
CorrectionFileL2L3Residual="Prompt24HIpp_V1_DATA_L2Residual_AK4PF.txt"

CorrectionFileNewL1FastJet="Summer24Prompt24/Summer23BPixPrompt23_V1_MC_L1FastJet_AK4PFPuppi.txt"
CorrectionFileNewL2Relative="Summer24Prompt24/Summer24Prompt24_V1_MC_L2Relative_AK4PFPuppi.txt"
CorrectionFileNewL3Absolute="Summer24Prompt24/Summer23BPixPrompt23_V1_MC_L3Absolute_AK4PFPuppi.txt"
CorrectionFileNewL2L3Residual="Summer24Prompt24/Summer24Prompt24_RunGnib2_V4_DATA_L2L3Residual_AK4PFPuppi.txt"


### Additional pt bins
#ptbins=(
 # "80 200"
 # "80 300"
 # "80 400"
 # "80 500"
 # "80 600"
 # "80 700"
 # "80 800"
 # "80 900"
 # "80 1000"
 # "80 1200"
#)

echo "Outpath $logpath"
if [ ! -d "$logpath" ]; then
    echo "Making Log Path"
    mkdir -p "$logpath"
fi

# input paths + tags
declare -A inputs
inputs["PD0"]="/eos/cms/store/group/phys_heavyions/xirong/Run3_ppref_Data_withrg/PPRefHardProbes0/crab_ppref_5362GeV_Data_withrg_rawpt/260709_053729/0000/"
#inputs["PD1"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics1/OO_Data_PromptReco_IonPhsyics1/260502_215053/0000/"
#inputs["PD2"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics2/OO_Data_PromptReco_IonPhsyics2/260503_161705/0000/"
#inputs["PD3"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics3/OO_Data_PromptReco_IonPhsyics3/260504_135745/0000/"

for tag in "${!inputs[@]}"; do
    infilepath="${inputs[$tag]}"

    for ptbin in "${ptbins[@]}"; do
        read jtptmin jtptmax <<< "$ptbin"

        outfiletag="${tag}_${tag2}pt${jtptmin}to${jtptmax}"

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
            $CorrectionFileL2Relative \
            $CorrectionFileL2L3Residual \
           > $logpath/executable_MC${MC}_${tag2}_${tag}.log 2>&1 &
    done
done