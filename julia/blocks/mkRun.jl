function mkRun(usr_input)

for i=2:lastindex(usr_input)
    if usr_input[i][1] == 'o'
        stimFile = usr_input[i][2:end];
        print("Loading stimulation file $stimFile\n")
    end
end
if length(stimFile) == 0
    print("No stimulation file was provided. Using default.\n")
end

strout = " -o $stimFile";
return strout
end