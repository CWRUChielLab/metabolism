#!/usr/bin/Rscript
#
# Analyzes diffusion data in R and creates plots
#
# Usage: ./diffusion.R output_type path_to_config path_to_diffusion path_to_plots path_to_stats
#   output_type        the type of output that the R script should create; valid
#                         options are "pdf", "png", and "latex"
#   path_to_config     the path of the config.out file to be read
#   path_to_diffusion  the path of the diffusion.out file to be read
#   path_to_plots      the path of the output that the R script will create
#                         (without the file extension)
#   path_to_stats      the path of the text file that the R script will create


# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("DIFFUSION SCRIPT FAILED: Too many parameters!")
   print("  Usage: ./diffusion.R output_type path_to_config path_to_diffusion path_to_plots path_to_stats")
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
   path_to_config = as.character(Args[7])
} else {
   path_to_config = "config.out"
}

if (!is.na(Args[8]))
{
   path_to_diffusion = as.character(Args[8])
} else {
   path_to_diffusion = "diffusion.out"
}

if (!is.na(Args[9]))
{
   path_to_plots = as.character(Args[9])
} else {
   path_to_plots = "plots"
}

if (!is.na(Args[10]))
{
   path_to_stats = as.character(Args[10])
} else {
   path_to_stats = "stats.out"
}


# Import experimental parameters, parse Elements and
# Reactions, calculate reaction hazards, and build a
# stochastic petri net
source("../scripts/_parse_config.R")


# Import diffusion data and count the number of particles
source("../scripts/_parse_diffusion.R")


# Conduct a statistic analysis of the diffusion data
source("../scripts/_analyze_diffusion.R")


# Plot the simulated diffusion data
source("../scripts/_plot_diffusion.R")


# End

