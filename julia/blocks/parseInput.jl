function parseInput(usr_input_raw)

    fs = 15000; # Sampling rate
    nA = 1; # Number of active channels
    filename = ""; # Name of the experiment
    mode = "VC"; # Mode of the experiment
    stimFile = "default.stim";
    strForGeko_pre = "./geko ";

    usr_input = replace(usr_input_raw, ' ' => "");
    usr_input = split(usr_input, '-')
    usr_input = String.(usr_input)

    #=========================#
    # Configuration arguments #
    #=========================#

    if usr_input[1] == "set"
        print("Setting experiment configuration:\n\n")
        for i=2:lastindex(usr_input)
            if usr_input[i][1] == 's'
                print("Sampling rate: ", usr_input[i][2:end], " Hz", "\n")
                fs = parse(Int, usr_input[i][2:end]);
            elseif usr_input[i][1] == 'c'
                print("Number of active channels: ", usr_input[i][2:end], "\n")
                nA = parse(Int, usr_input[i][2:end]);
            elseif usr_input[i][1] == 'm'
                print("Mode: ", usr_input[i][2:end], "\n")
                mode = usr_input[i][2:end];
            elseif usr_input[i][1] == 'f'
                print("Experiment name: ", usr_input[i][2:end], "\n")
                filename = usr_input[i][2:end];
            else
                print("Invalid option: ", usr_input[i][1], "\n")
            end
        end
        strForGeko_pre = "./geko -s $fs -c $nA -m $mode -f $filename";
    end

    #=========================#
    # Run an experiment       #
    #=========================#

    if usr_input[1] == "run"
        for i=2:lastindex(usr_input)
            if usr_input[i][1] == 'o'
                stimFile = usr_input[i][2:end];
                print("Loading stimulation file $stimFile\n")
            end
        end
        if length(stimFile) == 0
            print("No stimulation file was provided. Using default.\n")
        end
        print("Running experiment...\n\n")
        strForGeko = strForGeko_pre * " -o $stimFile";
        # TODO: SEND THIS COMMAND TO GEKO
        # GET DATA BACK, PLOT IT
    end

    #=========================#
    # Create a new stim file  #
    #=========================#

    if usr_input[1] == "stim"
        print("Creating new stimulation file:\n\n")
    end

end