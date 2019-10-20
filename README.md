# PBE

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3491145.svg)](https://doi.org/10.5281/zenodo.3491145)

### Performance Based Engineering Application ###

PBE is an open-source research application that can be used to assess the performance of a building in an earthquake scenario. The application focuses on quantifying building performance through decision variables. The user can characterize the structural model, the damage and loss model, and the seismic hazard model in this application. All models are interconnected by an uncertainty quantification framework that allows the user to define a flexible stochastic model for the problem. Given the stochastic model, the application first performs nonlinear response history simulations to get the Engineering Demand Parameters (EDPs) that describe structural response. Then, those EDPs are used to assess the Damage Measures (DMs) and Decision Variables (DVs) that characterize structural performance.


Please visit the [PBE Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/pbe-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

### How to Build

##### 1. Download this repo.

##### 2. Download the SimCenterCommon repo: https://github.com/NHERI-SimCenter/SimCenterCommon

Place the SimCenterCommon Repo in the same directory that you placed thePBE repo (note: not inside the PBE directory itself, but directory above it, i.e. SimCenterCommon and PBE directories both exist in same folder)

##### 3. Download the EE-UQ repo: https://github.com/NHERI-SimCenter/EE-UQ

Place the EE-UQ Repo in the same directory that you placed the PBE repo 

##### 4. Download the s3hark repo: https://github.com/NHERI-SimCenter/s3hark
Also place this repo in the same directory that you placed the PBE repo.

##### 5. Download the GroundMotionUtilities repo: https://github.com/NHERI-SimCenter/GroundMotionUtilities
Again placing this repo in the same directory that you placed the PBE repo.

##### 6. Install Qt: https://www.qt.io/

Qt is free for open source developers. Download it and start the Qt Creator application. From Qt Creator open the PBE.pro file located in the directory the PBE repo was downloaded into and select build to build it. For detailed instructions on using Qt, browse their website.

#### 7. To run locally you will need to install and build the SimCenterBackendApplications repo: https://github.com/NHERI-SimCenter/SimCenterBackendApplications

SimCenterBackendApplications contains a number of applications written in C++, C and Python. Follow the build instructions on the SimCenterBackendApplications githib page to build them. Once built inside the PBE applicationss preferences set the applications directory entry to point to the applications folder that the build process creates for BackendAPpplications.

### Acknowledgement

This material is based upon work supported by the National Science Foundation under Grant No. 1612843.
