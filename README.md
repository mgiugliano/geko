
# geKo (geKo electrophysiology Kommander)

geKo is a minimalistic software suite, based on a command-line interface (CLI), for standard cellular electrophysiological experiments.

It leverages over previous [work](https://www.sciencedirect.com/science/article/abs/pii/S0165027014001198) from our lab, defining stimulation protocols for conventional voltage- and current-clamp experiments in terms of a concise and flexible meta description. This allows representing complex waveforms as a piece-wise parametric decomposition of elementary sub-waveforms, abstracting the stimulation hardware. 


[![License: CC0-1.0](https://img.shields.io/badge/License-CC0_1.0-lightgrey.svg)](http://creativecommons.org/publicdomain/zero/1.0/)

<img src="/img/logo.jpg?raw=true" alt="geKo logo" height="200px">

## Motivations

We are after standardization and reproducibility in simple cellular electrophysiological experiments, but unhappy with the majority of software available, bloated, inefficient, and slow to use in the context of *industrialised neuroscience*.


## Libraries used

Ben Hoyt's "inih (INI Not Invented Here)" [library](https://github.com/benhoyt/inih) for parsing configuration files (see [here](https://en.wikipedia.org/wiki/INI_file)).

## Authors

- [@mgiugliano](https://www.github.com/mgiugliano)
- [@adam-says](https://github.com/adam-says)
- [@jgiorg](https://github.com/jgiorg)


## Appendix


### Installing NI-DAQmx on Linux

National Instruments (NI) makes its NI-DAQmx library freely available for using or developing
applications that interact with its data acquisition and signal conditioning devices.
The NI website and its documentation is suboptimal in terms of ease of use (but see [here](https://www.ni.com/pdf/manuals/ni-linux-device-drivers-2022-q4.html)) and the installation instructions for Linux are poor. We summarise below the key features and important steps.

For Linux (x86/64-bit architecture), NI's drivers are offered for the package managers of RedHat, 
Ubuntu, and SUSE. Installing these drivers (and libraries) is a two step procedure:

- The NI *repository* must be added to the list of (internet) *sources*, so that the package manager knows where to search and fetch the NI's packages to be installed;
- The actual installation of the packages can be performed, now that the repositories are made available to the package manager.

For the first step, NI gives relatively good indications, as summarised below:
- navigate to [downloads](https://www.ni.com/it-it/support/downloads/drivers/download.ni-daqmx.html)
- choose *Linux* from the *Supported OS* dropdown menu
- and select the latest version (e.g. *2022 Q4*)
- press **Download**

Note: you may be required to create a (free) NI's account.

Once downloaded, the small zipped archive (e.g. ~400KB, NILinux2022Q4DeviceDrivers.zip) is NOT a binary distribution of the driver, but a collection of *.rpm and *.deb files as mentioned above.

Unzip this file by:

```bash
> unzip NILinux2022Q4DeviceDrivers.zip
```

The content will look like:
```
> ls -1 ./NILinux2022Q4DeviceDrivers
ni-opensuse153firstlook-drivers-2022Q4.rpm
ni-opensuse153firstlook-drivers-stream.rpm
ni-opensuse154firstlook-drivers-2022Q4.rpm
ni-opensuse154firstlook-drivers-stream.rpm

ni-rhel8-drivers-2022Q4.rpm
ni-rhel8-drivers-stream.rpm
ni-rhel9firstlook-drivers-2022Q4.rpm
ni-rhel9firstlook-drivers-stream.rpm

ni-ubuntu2004-drivers-2022Q4.deb
ni-ubuntu2004-drivers-stream.deb
ni-ubuntu2204firstlook-drivers-2022Q4.deb
ni-ubuntu2204firstlook-drivers-stream.deb
```

From the filename, it is easy to identify the files related to the package manager
of the supported Linux distribution of choice (i.e. opensuse, rhel, ubuntu).
Then, tThere are other choices such as *static*, *stream*, and *firstlook*. Only one package should be used (see below how) as it adds a distinct *source* to the package manager. This refers to *stable* (updated only exceptionally by NI), *constantly updated* (by NI), and *new, roll out* binaries, respectively (i.e. corresponding to *static*, *stream*, and *firstlook*).  


Once choosen, the source can be added by :
```bash
> sudo apt install ./NILinux2022Q4DeviceDrivers/ni-ubuntu2204firstlook-drivers-2022Q4.deb
```

Next install the following: 
```bash
> sudo apt update
> sudo apt install ni-scope
> sudo apt install ni-hwcfg-utility
> sudo apt install ni-scope-daqmx
> sudo apt-get install ni-daqmx
> sudo apt-get install libnidaqmx-devel
> sudo dkms autoinstall
```

*libnidaqmx-devel* is probably the most important, as it installs the header files (i.e. /usr/include/NIDAQmx.h) and the proper links to the static binary linked library (i.e. installed at /usr/lib/x86_64-linux-gnu/libnidaqmx.so). 

Most likely *ni-scope*, *ni-scope-daqmx*, and *ni-hwcfg-utility* are not essential.

Test the NI's devices by invoking the NI's utility for listing detected hardware:

```bash
> nilsdev --verbose
```
...that prints (e.g.)
```bash
Dev1
   BusType: PCI
   DevSerialNum: 0x....
   PCI.BusNum: 0x3
   PCI.DevNum: 0x0
   ProductNum: 0x70AA
   ProductType: PCI-6229
```

and 
```bash
> nidaqmxconfig --self-test Dev1
```

...that (hopefully) returns
```bash
Success
```

### Installing and compiling geKo
.....


## Operation and features   

- geko is invoked by

> geko [-c configfile.ini]

If omitted, it will use the geko.ini file in the current folder...


- the hardware configuration is specified in the configuration file
- each data acquisition generates 
  - a) a uniquely names output data file (e.g. 20230117_232718.dat)
  - b) a "hidden" folder by the same name (i.e. ./20230117_232718.dat ), containing
      - a copy of the configuration file used
      - the exact command line geko was invoked by
      - the stimulus/stimuli synthesized (both as short and extended file)
      - the seed of the random number generator



## Funding

<img src="/img/EU.png?raw=true" alt="EU logo" height="70px"> <img src="/img/MUR.jpg?raw=true" alt="MUR logo" height="100px"> <img src="/img/PNRR.jpg?raw=true" alt="PNRR logo" height="100px"> 

<img src="/img/eBRAINSItaly.jpg?raw=true" alt="eBRAINS-Italy logo" height="100px">
