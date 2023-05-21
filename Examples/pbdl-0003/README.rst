.. _pbdl-0003:

Seismic performance assessment for transportation network components using Hazus methodology
============================================================================================

This example demonstrates the steps required to conduct seismic performance assessment of transportation network components using the HAZUS methodology and the PBE tool. In specific, the example concentrates on evaluating the seismic performance of three distinct components: 

#. A conventionally reinforced (i.e., non-prestressed), single-span, reinforced concrete bridge, built before 1975 and located in CA. This bridge is non-retrofitted and hence has limited seismic resistance. For this reason, it is best represented by the Hazus archetype HWB3.

#. A freeway segment that can be satisfactorily defined using the Hazus archetype HRD1

#. A cut-and-cover railway tunnel that is characterized by the Hazus archetype HTU1.

For brevity, the steps to calculate the seismic demands on all three of these structures are omitted, and only one type of Intensity Measure (IM) is assumed to determine the performance of each structure, namely Spectral Acceleration at 1 sec period, Permanent Ground Deformation, and Peak Ground Acceleration are the only IMs deemed to affect the bridge, roadway segment, and the tunnel respectively. As a result, similar to :download:`Example 1 <.../pbdl-0001>`, this example only focuses on the **DL** panel in PBE.  

All files mentioned in the subsequent discussion are provided in the :download:`src </src>` folder for ease in following the steps. Also included in this folder is the ``input.json`` file containing all the settings described in this example. One can simply jump to this example's Analysis and Results step by first opening ``input.json`` via File>Open to automatically populate all the required fields, then clicking the RUN button on the bottom left corner of the PBE user interface.
