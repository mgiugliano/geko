function parseInput(usr_input_raw)

    strForGeko_pre = "";
    strForGeko = "";
    pathGeko = "../../src/geko";
    pathData = "../../src/data.txt";

    usr_input = replace(usr_input_raw, ' ' => "");
    usr_input = split(usr_input, '-')
    usr_input = String.(usr_input)

    #=========================#

    if usr_input[1] == "set" # Set configuration arguments
        strForGeko_pre = mkSettings(usr_input);

    elseif usr_input[1] == "run" # Run experiment
        strForGeko = strForGeko_pre * mkRun(usr_input);
        print("Running experiment...\n\n")
        run(`$pathGeko $strForGeko`)
        
        plotData(pathData)

    elseif usr_input[1] == "stim" # Create stimulation file
        print("Creating new stimulation file:\n\n")
    end

end