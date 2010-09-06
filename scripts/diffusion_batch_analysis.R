#!/usr/bin/Rscript
#
# Analyzes batch diffusion data in R and creates plots
#
# Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots
#   path_to_batch  the path of the batch directory
#   n_simulations  the number of simulations in the batch
#   output_type    the type of output that the R script should create; valid
#                     options are "pdf", "png", and "latex"
#   path_to_plots  the path of the output that the R script will create (without
#                     the file extension)


# Import command line arguments
Args = commandArgs()
if (length(Args) < 7 )
{
   sink(stderr())
   print("DIFFUSION BATCH ANALYSIS FAILED: Missing required parameters path_to_batch and/or n_simulations!")
   print("  Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}
if (length(Args) > 9)
{
   sink(stderr())
   print("DIFFUSION BATCH ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Set parameters to default values if they were not
# specified on the command line
path_to_batch = as.character(Args[6])

n_simulations = as.integer(Args[7])

if (!is.na(Args[8]))
{
   output_type = as.character(Args[8])
} else {
   output_type = "pdf"
}

if (!is.na(Args[9]))
{
   path_to_plots = as.character(Args[9])
} else {
   path_to_plots = paste(path_to_batch, "/plots", sep="")
}


# Import statistics from each simulation in the batch
source("../scripts/_parse_batch_stats.R")


# Plot the batch statistics
source("../scripts/_plot_diffusion_batch.R")


# End

