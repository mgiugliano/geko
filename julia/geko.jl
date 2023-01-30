#=========================================#
#         Julia wrapper for GeKo          #
#                                         #
#      Adam Armada-Moreira, Jan 2023      #
#=========================================#

include("./blocks/parseInput.jl")

print("Welcome to GeKo!\n\n")
print("Usage: geko.jl [options]\n")
print("     set -s -c -m -f <Sampling rate | Nr of active channels | Recording mode | (opt) Filename>\n")
print("     run -o <Output file (.stim)>\n")
print("     stim <WIP>\n")
print("     exit <Quit the program>\n\n")


while true
    usr_input_raw = readline();
    parseInput(usr_input_raw)
    if usr_input_raw == "exit"
        break
    end
end