# Instructions for Compiling and Running cluster
## Introduction
Welcome to Cluster, a powerful tool for identifying and analyzing clusters within neuroimaging data.  This guide will demonstrate how to effectively utilize Cluster for clustering analysis in neuroimaging.


For more information about cluster and related tools, visit the FMRIB Software Library (FSL) website:[FSL Git Repository](https://git.fmrib.ox.ac.uk/fsl) and you can also find additional resources and documentation on cluster on the FSL wiki page: [cluster Documentation](https://fsl.fmrib.ox.ac.uk/fsl/fslwiki/Cluster).
## Clone the Repository

Begin by cloning the project repository from GitHub onto your local machine. You can do this by running the following command in your terminal or command prompt:

```bash
git clone https://github.com/Bostrix/FSL-cluster.git
```
This command will create a local copy of the project in a directory named "FSL-cluster".

## Navigate to Project Directory
Change your current directory to the newly cloned project directory using the following command:
```bash
cd FSL-cluster
```
## Installing Dependencies
To install the necessary dependencies for compiling and building the project, follow these steps:
```bash
sudo apt-get update
sudo apt install g++
sudo apt install make
sudo apt-get install libboost-all-dev
sudo apt-get install libblas-dev libblas3
sudo apt-get install liblapack-dev liblapack3
sudo apt-get install zlib1g zlib1g-dev
```

## Compilation:
To compile Cluster, follow these steps:
- Ensure correct path in Makefile: After installing the necessary tools, verify correct path in the makefile to include additional LDFLAGS for the required libraries. For instance, if utilizing the warpfns, basisfield, meshclass, miscmaths, and znzlib, ensure that the correct path is present in the makefile. Make sure `$(WARPFNS_LDFLAGS)`,`$(ZNZLIB_LDFLAGS)` are included in the compile step of the makefile.

- Confirm that the file `point_list.h` within the warpfns library accurately includes the path to `armawrap/newmat.h`.

- Verify the accurate paths in meshclass's Makefile:
verify the correct path in the makefile of meshclass to include additional LDFLAGS for the required libraries. For instance, if utilizing the newimage, miscmaths, NewNifti, cprob, znzlib and utils libraries as LDFLAGS variable in meshclass makefile,ensure that the correct path is present in the makefile.

Once the necessary adjustments have been confirmed, you can utilize the following command in your terminal to rebuild the meshclass separately:
```bash
make
```
The command executes the Makefile in the meshclass and rebuilds it separately based on the modified configuration and source code changes. After re-running the make command, return to the main cluster directory by `cd ..` and attempt to rebuild the project again.

- Compiling: 
Execute the appropriate compile command to build the cluster tool.
```bash
make clean
make
```
- Resolving Shared Library Errors
When running an executable on Linux, you may encounter errors related to missing shared libraries.This typically manifests as messages like:
```bash
./fsl-cluster: error while loading shared libraries: libexample.so: cannot open shared object file:No such file or directory
```
To resolve these errors,Pay attention to the names of the missing libraries mentioned in the error message.Locate the missing libraries on your system. If they are not present, you may need to install the corresponding packages.If the libraries are installed in custom directories, you need to specify those directories using the `LD_LIBRARY_PATH` environment variable. For example:
```bash
export LD_LIBRARY_PATH=/path/to/custom/libraries:$LD_LIBRARY_PATH
```
Replace `/path/to/custom/libraries` with the actual path to the directory containing the missing libraries.Once the `LD_LIBRARY_PATH` is set, attempt to run the executable again.If you encounter additional missing library errors, repeat steps until all dependencies are resolved.

- Resolving "The environment variable FSLOUTPUTTYPE is not defined" errors
If you encounter an error related to the FSLOUTPUTTYPE environment variable not being set.Setting it to `NIFTI_GZ` is a correct solution, as it defines the output format for FSL tools to NIFTI compressed with gzip.Here's how you can resolve:
```bash
export FSLOUTPUTTYPE=NIFTI_GZ
```
By running this command, you've set the `FSLOUTPUTTYPE` environment variable to `NIFTI_GZ`,which should resolve the error you encountered.
## Running cluster

After successfully compiling, you can run cluster by executing:
```bash
./fsl-cluster --in=<filename> --thresh=<value> [options]
```
This command will execute the cluster tool.
