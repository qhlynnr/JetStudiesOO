#!/bin/bash

MC=0
jtptmin=30
jtptmax=80
neventcut=0 #0 for full
outfilename="0430QAPlotDiffptBins/Debug/"
logpath="/home/xirong/JetStudiesOO/LynnsCode/Logs/0501QA/"
nohup ./SaveHistChain $MC $jtptmin $jtptmax $neventcut $outfilename > $logpath/executable_${MC}_pt${jtptmin}to${jtptmax}_nevt${neventcut}.log 2>&1 &