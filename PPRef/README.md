# Setup

1. Set up a CMSSW environment by navigating to `CMSSW_15_0_11/src` and running:

   ```bash
   cmsenv
   ```

# Running Histogram Generation

1. Navigate to the histogram generation directory and build the executable:

   ```bash
   cd Macros/QA/QAHistGeneration
   make
   ```

2. To run on **data**, check the settings in `runData.sh`, then execute:

   ```bash
   source runData.sh
   ```

3. To run on **MC**, check the settings in `runMC.sh`, then execute:

   ```bash
   source runMC.sh
   ```

4.  Once thats done, a .root file would appear in the directory of your choice containing the histograms. 

## Drawing JER and JES

Open `Macros/QAPlotting/drawJERJES.C`.

Edit the arguments to `drawJERJES()` by setting:

- `filename` to the path of the ROOT file containing the histograms.
- `dirname` to the directory where the output plots should be saved.

```cpp
void drawJERJES(bool MC = true,
                const char* filename = "",
                const char* dirname  = "");
```

and then do

```
root drawJERJES.C
```

plots will be automatically generated.

## Drawing MC and Data comparison or MC Reco Gen Comparison

Open 'Macros/QAPlotting/drawComparisons.C'

- Edit Inside the function
- `dataFolder` to the path of the ROOT file containing the histograms.
- `outfolder` to the directory where the output plots should be saved.
- `dataFiles` to the corresponding file names

This macro is designed to plot multiple data and mc files from pt bins, so with one bin, just put in the one path.

and then do

```
root drawComparison.C
```

plots will be automatically generated.

# Foresting