#!/usr/bin/Rscripts
#
# Subroutine for importing the contents of a diffusion file;
# depends on the existence of path_to_diffusion


# Check for path_to_diffusion
if (!exists("path_to_diffusion"))
{
   sink(stderr())
   print("PARSE DIFFUSION FAILED: path_to_diffusion is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Import diffusion data
diffusion_data = read.table(path_to_diffusion, header=TRUE)


# Grab the relevant column labels from the diffusion data
diffusion_types = c("dx_actual", "dx_ideal", "dy_actual", "dy_ideal")


# Count the number of particles
samples = nrow(diffusion_data)


# Set completion flag
diffusion_parsed = TRUE


# End

