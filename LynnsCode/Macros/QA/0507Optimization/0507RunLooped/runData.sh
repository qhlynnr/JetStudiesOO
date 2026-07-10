#!/bin/bash

MC=0
neventcut=0 # 0 for full
outfolder="0706pprefDataPD0/"
logpath="/home/xirong/JetStudiesOO/LynnsCode/RootFiles/${outfolder}/log/"
tag2=""
# pt bins
ptbins=(
  "30 80"
  "80 100"
  "100 300"
  "300 700"
)
jetTreeName="ak4PFJetAnalyzer/t"


###
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
inputs["PD0"]="/eos/cms/store/group/phys_heavyions/xirong/Run3_ppref_Data/PPRefHardProbes0/crab_ppref_5362GeV_Data/260703_144116/0000/"
#inputs["PD1"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics1/OO_Data_PromptReco_IonPhsyics1/260502_215053/0000/"
#inputs["PD2"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics2/OO_Data_PromptReco_IonPhsyics2/260503_161705/0000/"
#inputs["PD3"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics3/OO_Data_PromptReco_IonPhsyics3/260504_135745/0000/"

for tag in "${!inputs[@]}"; do
    infilepath="${inputs[$tag]}"

    for ptbin in "${ptbins[@]}"; do
        read jtptmin jtptmax <<< "$ptbin"

        outfiletag="${tag}_${tag2}_Pt${jtptmin}to${jtptmax}"

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
            > $logpath/executable_MC${MC}_${tag2}_pt${jtptmin}to${jtptmax}_${tag}.log 2>&1 &
    done
done