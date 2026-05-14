#!/bin/bash

MC=1
neventcut=0 # 0 for full
outfolder="051126NewBinHLT2/"
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
inputs["MC"]="/eos/cms/store/group/phys_heavyions/hbossi/mc_productions/QCD-dijet_pThat15-event-weighted_TuneCP5_5p36TeV_pythia8/OO_MC_DijetEmbedded_pThat-15to1200_TuneCP5_5p36TeV_pythia8/260306_002843/0000/"

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
            > $logpath/executable_MC${MC}_${tag2}_pt${jtptmin}to${jtptmax}_${tag}.log 2>&1 &
    done
done