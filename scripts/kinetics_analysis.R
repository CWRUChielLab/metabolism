#!/usr/bin/Rscript
#
# Creates plots in R of the observed and expected reaction trajectories
#
# Usage: ./kinetics_analysis.R output_type should_integrate path_to_config path_to_census path_to_plots
#   output_type       the type of output that the R script should create; valid options
#                        are "pdf", "png", and "latex"
#   should_integrate  whether or not to run the numerical integrator to find the
#                        expected trajectories; valid options are "true" and "false"
#   path_to_config    the path of the config.out file to be read
#   path_to_census    the path of the census.out file to be read
#   path_to_plots     the path of the output that the R script will create (without
#                        the file extension)


# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./kinetics_analysis.R output_type should_integrate path_to_config path_to_census path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Set parameters to default values if they were not
# specified on the command line
if (!is.na(Args[6]))
{
   output_type = as.character(Args[6])
} else {
   output_type = "pdf"
}

if (!is.na(Args[7]))
{
   should_integrate = as.character(Args[7])
} else {
   should_integrate = "true"
}

if (!is.na(Args[8]))
{
   path_to_config = as.character(Args[8])
} else {
   path_to_config = "config.out"
}

if (!is.na(Args[9]))
{
   path_to_census = as.character(Args[9])
} else {
   path_to_census = "census.out"
}

if (!is.na(Args[10]))
{
   path_to_plots = as.character(Args[10])
} else {
   path_to_plots = "plots"
}


# Import experimental parameters, parse Elements and
# Reactions, calculate reaction hazards, and build a
# stochastic petri net
source("../scripts/_parse_config.R")


# Import simulated kinetics data
source("../scripts/_parse_census.R")


# Perform a numerical integration on the rate laws
# associated with the reaction system to find the expected
# trajectories of each Element
source("../scripts/_integrate.R")


# Plot the simulation data and numerical integration data
source("../scripts/_plot_data.R")


# End

