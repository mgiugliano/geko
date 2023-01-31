function parseInput(usr_input_raw)

    strForGeko_pre = " ";
    strForGeko = "";
    pathData = "../../src/data.txt";

    usr_input = replace(usr_input_raw, ' ' => "");
    usr_input = split(usr_input, '-')
    usr_input = String.(usr_input)

    #=========================#
    print("\n----------------------------------\n\n")
    if usr_input[1] == "set" # Set configuration arguments
        strForGeko_pre = mkSettings(usr_input);
        global cmdGeko = `$cmdGeko $strForGeko_pre`;

    elseif usr_input[1] == "run" # Run experiment
        strForGeko = mkRun(usr_input);
        finalCmd = `$cmdGeko $strForGeko`;
        print("Running experiment with command: $finalCmd\n")
        run(`$finalCmd`)
        if doPlot == 1
            plotData(pathData)
        end

    elseif usr_input[1] == "stim" # Create stimulation file
        print("Creating new stimulation file:\n\n")
        mkStim(usr_input)
    end
    print("\n----------------------------------\n\n")
end

function mkSettings(usr_input)
    # fs = 15000; # Sampling rate
    # nA = 1; # Number of active channels
    # filename = ""; # Name of the experiment
    # mode = "VC"; # Mode of the experiment

    print("Setting experiment configuration:\n\n")
    for i=2:lastindex(usr_input)
        if usr_input[i][1] == 's'
            print("Sampling rate: ", usr_input[i][2:end], " Hz", "\n")
            fs = parse(Int, usr_input[i][2:end]);
            global fs = fs;
        elseif usr_input[i][1] == 'c'
            print("Number of active channels: ", usr_input[i][2:end], "\n")
            nA = parse(Int, usr_input[i][2:end]);
            global nA = nA;
        elseif usr_input[i][1] == 'm'
            print("Mode: ", usr_input[i][2:end], "\n")
            mode = usr_input[i][2:end];
            global mode = mode;
        elseif usr_input[i][1] == 'f'
            print("Experiment name: ", usr_input[i][2:end], "\n")
            filename = usr_input[i][2:end];
            global filename = filename;
        else
            print("Invalid option: ", usr_input[i][1], "\n")
        end
    end
    #strout = "-s $fs -c $nA -m $mode -f $filename";
    strout = `-s $fs -c $nA`;
    return strout
end

function mkRun(usr_input)
    strout = ``;
    stimFile = "";
    for i=2:lastindex(usr_input)
        if usr_input[i][1] == 'p'
            global doPlot = 1;
        else
            global doPlot = 0;
        end

        if usr_input[i][1] == 'o'
            stimFile = usr_input[i][2:end];
            print("Loading stimulation file $stimFile\n")
            strout = ` -o $stimFile`;
        else
            print("No stimulation file was provided. Using default.\n")
            stimFile = "default.stim";
            strout = ``;
        end
    end
    
    return strout
end

function plotData(datafile)

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

    for i = 1:ncols
        display(lineplot(tVec, onlyNum[:,i], 
                width = 80, height = 20, 
                color=myColors[i], title = myTitles[i]))
        print("\n\n")
    end


end

function mkStim(usr_input)
end
