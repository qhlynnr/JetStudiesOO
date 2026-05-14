#!/bin/bash

MC=0
neventcut=0 # 0 for full
outfilename="051126NewBinHLT2/"
logpath="/home/xirong/JetStudiesOO/LynnsCode/RootFiles/${outfolder}/log/"
tag2="finebin"
# pt bins
ptbins=(
  "30 80"
  "80 100"
  "100 300"
  "300 700"
)

if [ ! -d "$logpath" ]; then
    mkdir -p "$logpath"
fi

# input paths + tags
declare -A inputs
inputs["PD0"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics0/OO_Data_PromptReco_IonPhsyics0/260306_195006/0000/"
inputs["PD1"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics1/OO_Data_PromptReco_IonPhsyics1/260502_215053/0000/"
inputs["PD2"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics2/OO_Data_PromptReco_IonPhsyics2/260503_161705/0000/"
inputs["PD3"]="/eos/cms/store/group/phys_heavyions/hbossi/OOJetSubstructure/DataForests/IonPhysics3/OO_Data_PromptReco_IonPhsyics3/260504_135745/0000/"

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
            $outfilename \
            $infilepath \
            $outfiletag \
            > $logpath/executable_MC${MC}_${tag2}_pt${jtptmin}to${jtptmax}_${tag}.log 2>&1 &
    done
done