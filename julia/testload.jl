using UnicodePlots
using DelimitedFiles

fs = 10000;
mode = "VC";

datafile = "/home/user/acquisition/adam/geko/src/dataStore.txt"

    data = readdlm(datafile, '\t', Any, '\n');
    (nrows, ncols) = size(data);
    tVec = collect(range(1, length=nrows-1, step=1)) .* 1/fs;
    onlyNum = Number.(data[2:end,:]);

    if mode == "VC"
        pUnit = "pA";
        sUnit = "mV";
        oUnit = "mV";
    elseif mode == "CC"
        pUnit = "mV";
        sUnit = "pA";
        oUnit = "pA";
    end
    myTitles = ["Primary input ($pUnit)" "Secondary input ($sUnit)" "Stimulation ($oUnit)"];
    myColors = [:red :cyan :green];
    print("Ready to plot:\n\n")
    
    @time for i = 1:ncols
        display(lineplot(tVec, onlyNum[:,i], 
                width = 80, height = 20, 
                color=myColors[i], title = myTitles[i]))
        print("\n\n")
    end