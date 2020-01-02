.. Performance Based Engineering Application documentation master file, created by
   sphinx-quickstart on Mon Dec  9 12:35:25 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Performance Based Engineering Application
=====================================================================

The intended audience for the |full tool name| (|short tool name|) are researchers and practitioners interested in predicting the seismic performance of buildings.

This is an open-source research application. The source code at the |tool github link| provides an application that can be used to assess the performance of a building in an earthquake scenario. The application focuses on quantifying building performance through decision variables. Given that the properties of the buildings and the earthquake events are not known exactly, and that the simulation software and the user make simplifying assumptions in the numerical modeling of the structure, the estimated response of the structure already exhibits significant variability. Such response can be estimated using our |EE-UQ app link|. The |short tool name| builds on the |EE-UQ short name| and uses its response estimates to assess the damage to building components and the consequences of such damage. Users who have response estimates from external simulations, have the option to load those and focus only on the damage and loss estimation in the |short tool name|.

The user can characterize the structural model, the damage and loss model, and the seismic hazard model in this application. All models are interconnected by an uncertainty quantification framework that allows the user to define a flexible stochastic model for the problem. Given the stochastic model, the application first performs nonlinear response history simulations to get the Engineering Demand Parameters (EDPs) that describe structural response. Then, those EDPs are used to assess the Damage Measures (DMs) and Decision Variables (DVs) that characterize structural performance.

Depending on the type of structural system, the fidelity of the numerical model and the number of EDP samples requested, the response history simulations can be computationally prohibitively expensive. To overcome this impediment, the user has the option to perform the response simulations on the Stampede2 supercomputer. Stampede2 is located at the Texas Advanced Computing Center (TACC) and made available to the user through NHERI DesignSafe-CI, the cyberinfrastructure provider for the distributed NSF-funded Natural Hazards Engineering Research Infrastructure (NHERI) facility.

The computations are performed, as discussed in the :ref:`lbl-technical-manual`, in a workflow application. That is, the numerical simulations are performed by a sequence of applications. The |short tool id| backend software runs these applications for the user, taking the outputs from some programs and providing them as inputs to others. The design of the |short tool name| is such that researchers are able to modify the backend application to utilize their own application in the workflow computations. This ensures that researchers are not limited to using the default applications we provide and will be enthused to provide their own applications for others to use.

This is Version |tool version| of the tool. Users are encouraged to comment on what additional features and capabilities they would like to see in this application. These requests and feedback can be submitted through an anonymous |user survey link|. We greatly appreciate any input you have. If there are features you want, chances are many of your colleagues also would benefit from them. Users are encouraged to review the :ref:`lbl-requirements` to see what features are planned for this application.


.. _lbl-user-manual:

.. toctree::
   :caption: User Manual
   :maxdepth: 2
   :numbered: 2

   common/user_manual/installation/installation
   common/user_manual/usage/usage
   common/user_manual/requirements/requirements
   common/user_manual/troubleshooting/troubleshooting
   common/user_manual/examples/examples
   common/license


.. _lbl-technical-manual:

.. toctree::
   :caption: Technical Manual
   :maxdepth: 2
   :numbered: 2

   common/technical_manual/technical_manual


.. _lbl-developer-manual:

.. toctree::
   :caption: Developer Manual
   :maxdepth: 2
   :numbered: 2

   common/developer_manual/how_to_build/how_to_build
   common/developer_manual/architecture/architecture
   common/developer_manual/API/API
   common/developer_manual/how_to_extend/how_to_extend
   common/developer_manual/verification/verification
   common/developer_manual/coding_style/coding_style

Developers
==========

Adam Zsarnóczay 

Frank McKenna

Wael Elhaddad

Chaofeng Wang

Michael Gardner

License
=======

The |full tool name| is distributed under the BSD 3-Clause license, see :ref:`lbl-license` for details.

Acknowledgement
===============

This material is based upon work supported by the National Science Foundation under Grant No. 1612843. Any opinions, findings, and conclusions or recommendations expressed in this material are those of the authors and do not necessarily reflect the views of the National Science Foundation.

Contact
=======
Adam Zsarnóczay, NHERI SimCenter, Stanford University, adamzs@stanford.edu