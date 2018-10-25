# OpenSees -- Open System for Earthquake Engineering Simulation
# Pacific Earthquake Engineering Research Center
# http://opensees.berkeley.edu/
#
# SDOF Example 0.5
# ------------------------
#  Single Degree of Freedom System
#  Linear Elastic Behavior (for now)
# 
# Example Objectives
# -----------------
#  Testing
#
# 
# Units: N, m, sec
#
# Written: adamzs
# Date: October 2018

# --------------------------------------------------------------- set constants
set pi [expr 2*asin(1.0)]

# metric units
set m 1.0;
set cm 0.01;
set mm 0.001;
set m2 1.0;
set cm2 1e-4;
set mm2 1e-6;
set m3 1.0;
set m4 1.0;
set cm4 1e-8;
set mm4 1e-12;
set N 1.0;
set kN 1000.;
set sec 1.0;
set MPa 1e6;
set GPa 1e9;
set kg 1.0;

# imperial units
set in [expr $m*0.0254]
set ft [expr $m*0.3048]
set in2 [expr pow($in,2.0)]
set ft2 [expr pow($ft,2.0)]
set in4 [expr pow($in,4.0)]
set ft4 [expr pow($ft,4.0)]
set kip [expr $kN*4.448222]
set ksi [expr $MPa*6.894757]
set lb [expr $kg*0.453592]

# ==============================================================================
#                                                                         Units
# ==============================================================================

set units [dict create]
dict set units mass $kg
dict set units length $m
dict set units force $kN
dict set units stiffness $GPa

# ==============================================================================
#                                                              Model parameters
# ==============================================================================

pset T_1 1.0
pset xi 0.05
#pset height 3.0
#pset mass 1e3
#pset cs_area 1e-2
#pset init_stiffness 100.0

# fixed
set height 1.0
set mass 1e5
set cs_area 1e-2

# convert the parameters to local units
set h [expr $height*[dict get $units length]]
set M [expr $mass*[dict get $units mass]]
set A [expr $cs_area*pow([dict get $units length],2.0)]
#set E_0 [expr $init_stiffness*[dict get $units stiffness]]

# conditioned on others
set I_z [expr pow($A, 2) /12.0]
set K [expr $M/pow($T_1/(2*$pi), 2.0)]
set E_0 [expr $K * pow($h,3.0) /$I_z /12.0]

# ==============================================================================
#                                                              Model generation
# ==============================================================================

# ---------------------------------------------------------- create environment
# two dimensions, two dof
wipe
model basic -ndm 2 -ndf 3

# ---------------------------------------------------------------- create nodes
#    tag X   Y
node 0   0.0 0.0 
node 1   0.0 $h 

mass 1   $M 0.0 0.0

# --------------------------------------------------------------- add a support
#   tag  x  y  zz
fix 0    1  1  1
fix 1    0  0  1

# ------------------------------------------------------------- create material
# linear elastic, uniaxial
set matTag 1
#                        matTag  E
uniaxialMaterial Elastic $matTag $E_0

# ------------------------------------------------------------- create elements
set columnTR 1
geomTransf Corotational $columnTR
set eleTag 1
#                         eleTag  iNode jNode A  E    I_z  transfTag 
element elasticBeamColumn $eleTag 0     1     $A $E_0 $I_z $columnTR

# ----------------------------------------------------------------- add damping
set lambda [eigen -fullGenLapack 1]
set w_1 [expr pow($lambda, 0.5)]
set w_2 [expr $w_1/16.0]
set a_0 [expr $xi * 2.0 * $w_1 * $w_2 / ($w_1 + $w_2)]
set b_0 [expr $xi * 2.0 / ($w_1 + $w_2)]

rayleigh $a_0 $b_0 0.0 0.0