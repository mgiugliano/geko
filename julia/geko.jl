#=========================================#
#         Julia wrapper for GeKo          #
#                                         #
#      Adam Armada-Moreira, Jan 2023      #
#=========================================#

# External packages
using UnicodePlots
using DelimitedFiles

# Local functions
include("./blocks/gekofun.jl")

# Global variables
global fs = 15000;
global nA = 1;
global mode = "VC";
global filename = "data.txt";
global stimfile = "default.stim";
global cmdGeko = `/home/user/acquisition/adam/geko/src/geko `;
global doPlot = 0;


print("Welcome to GeKo!\n\n")
print("Usage: geko.jl [options]\n")
print("     set -s -c -m -f Define experiment settings: <Sampling rate | Nr of active channels | Recording mode | (opt) Filename>\n")
print("     run -o -p       Run finite acquisition and stimulation: <Output file (.stim) | Plot recorded data>\n")
print("     stim -o         Create a stimulation file: <Output file>\n")
print("     exit            <Quit the program>\n\n")

print("Enter input:\n")
while true
    usr_input_raw = readline();
    if usr_input_raw == "exit"
        break
    end
    parseInput(usr_input_raw)
    print("Enter input:\n")
end