function mkSettings(usr_input)
    fs = 15000; # Sampling rate
    nA = 1; # Number of active channels
    filename = ""; # Name of the experiment
    mode = "VC"; # Mode of the experiment

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
    strout = "-s $fs -c $nA -m $mode -f $filename";
    return strout
end