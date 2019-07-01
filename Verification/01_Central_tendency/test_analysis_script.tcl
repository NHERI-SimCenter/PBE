numberer RCM
system Umfpack
integrator Newmark 0.5 0.25
test NormUnbalance 0.01 20 
algorithm Newton
analysis Transient
analyze [expr int($numStep+5.0/$dt)] [expr $dt/10.]