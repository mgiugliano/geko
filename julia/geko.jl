#=========================================#
#         Julia wrapper for GeKo          #
#                                         #
#      Adam Armada-Moreira, Jan 2023      #
#=========================================#

# Global variables
global fs::Int64 = 15000;
global nA::Int64 = 1;
global mode::String = "VC";
global filename::String = "data.txt";
global stimfile::String = "default.stim";
global cmdGeko::Cmd = `/home/user/acquisition/adam/geko/src/geko -s1 -c1 `;
global doPlot::Bool = 0;

using UnicodePlots
using DelimitedFiles
include("./blocks/gekofun.jl")


print("Welcome to GeKo!\n\n")
print("Usage: geko.jl [options]\n")
print("     set -s -c -m -f <Sampling rate | Nr of active channels | Recording mode | (opt) Filename>\n")
print("     run -o -p       <Output file (.stim) | Plot recorded data>\n")
print("     stim            <WIP>\n")
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