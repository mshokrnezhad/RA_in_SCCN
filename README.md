# RA_in_SCCN
Different Approaches to Allocate Resources in Single Cell Cellular Networks
- channel_assignment_fp.cpp: A heuristic approach to assign frequency channels where transmit powers are set to a fixed predefined value
- node_generator.cpp: Generating nodes in a 2D area
- opt_mediatior.cpp: Initializing the inputs of pcca_opt.zpl
- pcca_opt.zpl: Formulating the Resource Allocation Problem (RAP) in Zimpl
- power_control_hca.cpp: Controlling transmit powers using the non-cooprating game where channels are assigned using the heuristic approach
- power_control_optca.cpp: Controlling transmit powers using the non-cooprating game where channels are assigned optimally 
- resource_allocation.cpp: Assigning frequency channels using the second-price auction and controlling transmit powers using the non-cooprating game
- script.sh: Allocating resources for different single cell cellular networks

Watch the name of code files called in the shell scripts. The called names might be different from the current names!
