# EXPONENTIAL INTEGRATE AND FIRE STOCHASTIC POINT NEURON
# Heun's integration method with exact generation of wh

using PyPlot, Distributions;

# MODEL PARAMETERS (from Sterrat's book)
Rm      =  10;       # kΩ
Cm      =   1;       # μF
Em      = -70;       # mV
V_thres = -52;       # mV
V_reset = -70;       # mV
dT      =   3;       # mV

# INTEGRATION PARAMETERS
h = 10^-2;           # intrgration step
h_sqrt = h^0.5;      # square root of integration step
null_steps = 100;    # not saved steps

# BASIC INTEGRATE AND FIRE NEURON  (use Em=0, V_thres=20, V_reset=0)
# IF(Vm, I) = ((Em - Vm)/Rm + I); #/Cm 

# EXPONENTIAL INTEGRATE AND FIRE NEURON 
EXP_IF(Vm, I) = (-((Vm -Em)/Rm - dT/Rm*exp((Vm-V_thres)/dT))+ I); #/Cm

# INTEGRATING FUNCTION
# Vm = initial membrane potential [mV], I = injected current [uA], sigma = white noise's standard deviation (e.g 10^-1), T = integration time [ms]
function integrator(Vm,I,sigma,T)  
    w_steps = T * 100                                            # writing steps * sampling_rate (100 Hz) (?)
    M = w_steps * null_steps                                     # total number of steps
    t = 0                                                        # initialize time (to 0)  
    Vm_array = zeros(w_steps+1)                                  # array of membrane potential
    Vm_array[1] = Vm                                             # initial membrane potential   
    wh = 0.0                                                     # initialize white noise
    for i in 1:w_steps                                           # saved steps
        for null in 1:null_steps                                 # not saved steps
            if sigma != 0.0
                wh = h_sqrt * rand(Normal(0, 1)) * sigma         # generate white noise (only if sigma != 0)
            end
            k = h * EXP_IF(Vm_array[i], I) + wh                  # k=h*q(x(t_i),t_i)+wh(t_i)*g(x(t_i),t_i)                           
Vm_array[i+1] = Vm_array[i]+0.5*(k+h*EXP_IF(Vm_array[i]+k,I)+wh) # x(t_i+1)=x(t_i)+h*0.5*[q(x(t_i_,t_i)+q(x(t_i)+k,t_i+1)]+0.5*wh(t_i)*[g(x(t_i),t_i)+g(x(t_i)+k,t_i+1)]
            if Vm_array[i+1] > V_thres                           # spiking mechanism
                Vm_array[i+1] = V_reset
            end
            t += h                                               # increment time step
        end
    end
    return Vm_array                                             
end;

#==========   TEST   =============================================================================
voltage_sub = integrator(V_reset, 1.5, 0.0, 100)  # subthreshold
voltage_sup = integrator(V_reset, 2.02, 0.0, 100) # suprathreshold
T = 100
w_steps = T * 100
M = w_steps * null_steps 
plot(voltage_sub, label=L"1.5 \mu A")
plot(voltage_sup, label=L"2.02 \mu A")
xticks([i for i in 0:2000:w_steps], [Int(i) for i in 0:20:M/w_steps]) #rescale ticks labels
ylabel("membrane potential [mV]")
xlabel("time [ms]")
==================================================================================================#

# =======    DELIVERING A PULSE OF CURRENT      ===================================================
# rest = time before stimulus, intensity = stimulus intensity, stim = stimulus duration, relax = time after stimulus, sigma = white noise'standard deviation (e.g 10^-1)
function pulse(rest, intensity, stim, relax, sigma)                 
        before = integrator(Em, 0.0, sigma, rest)                   # before = rest period   
        stimulus = integrator(before[end], intensity, sigma, stim)  # stimulus = stimululation
        after = integrator(stimulus[end], 0.0, sigma, relax)        # after = relax period
    return vcat(before, stimulus, after)                            # return the whole pulse
end;

#==========   TEST   =============================================================================
voltage = pulse(100, 1.6, 200, 100, 0.0)
plot(voltage, label=L"02 \mu A")
T = 400
w_steps = T * 100
M = w_steps * null_steps 
xticks([i for i in 0:5000:w_steps], [Int(i) for i in 0:50:M/w_steps*4]) #rescale ticks labels
ylabel("membrane potential [mV]")
xlabel("time [ms]")
==================================================================================================#

# ==========    IV CURVE     === (current clamp mode) ============================================

function IV_curve(min_I,max_I)                     # min_I = minimum injected current [uA], max_I = maximum injected current [uA]
    I = min_I:0.1:max_I                            # injected current array
    Vm = zeros(length(I))                          # membrane potential array
    for i in 1:length(I)                           # for each injected current
        Vm[i] = integrator(Em, I[i], 0.0, 50)[end] # membrane potential after 50 ms
    end
    return I, Vm                                   # return injected current and membrane potential 
end;

#==========   TEST   =============================================================================
current, voltage = IV_curve(-0.5, 1.5)
plot(voltage, current)
xlabel("membrane potential [mV]")
ylabel("current [uA]")
==================================================================================================#


