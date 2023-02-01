
using DelimitedFiles
using UnicodePlots

fs = 20000;
nA = 1;
mode = "VC";

    datafile = "/home/user/acquisition/adam/geko/julia/data.txt"

    print("Entered the plotData function.\n")
    # This function plots the data from the datafile.
    
    # Load data
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
    for i = 1:ncols
        display(lineplot(tVec, onlyNum[:,i], 
                width = 80, height = 20, 
                color=myColors[i], title = myTitles[i]))
                print("\n\n")
    end


    print("Done plotting.\n")
