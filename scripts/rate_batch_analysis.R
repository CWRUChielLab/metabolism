#!/usr/bin/Rscript
#
# Analyzes batch reaction rate data in R and creates graphs
#
# Usage: ./rate_batch_analysis.R pathtobatch nexperiments pathtoplots uselatex
#   pathtobatch    the path of the batch directory
#   nexperiments   the number of experiments in the batch
#   pathtoplots    the path of the PDF or LaTeX documents that the R script
#                    will create without the .pdf or .tex extension
#   uselatex       if "true" is passed, create individual plots in the form
#                    of LaTeX documents; else draw all plots in one PDF

# Import command line arguments
Args = commandArgs()
path_to_batch  = as.character(Args[6])
n_experiments  = as.integer(Args[7])
path_to_plots  = as.character(Args[8])
use_latex      = as.character(Args[9])

# Import experimental parameters and statistics
config = list()
stats = list()
for (i in 1:n_experiments)
{
   this_experiment = formatC(i, flag="0", width=nchar(n_experiments))
   this_config = paste(path_to_batch, "/", this_experiment, "/config.", this_experiment, ".out", sep="")
   this_stats  = paste(path_to_batch, "/", this_experiment, "/stats.",  this_experiment, ".out", sep="")

   keepers = c("version", "seed", "iters", "x", "y", "atoms", "reactions", "shuffle")
   temp = readLines(this_config); f = file(); cat(temp[charmatch(keepers, temp)], sep="\n", file=f); temp = read.table(f, colClasses=c("character", "character"));
   write(as.matrix(temp), ncolumns=length(keepers), file=f)
   config = rbind(config, read.table(f,          header=TRUE))
   close(f);
   stats  = rbind(stats,  read.table(this_stats, header=TRUE))
}

# Calculate densities and extract batch parameters
config[,"density"] = config[,"atoms"] / (config[,"x"] * config[,"y"])
x = config[1,"x"]
y = config[1,"y"]
world_size = x * y
k_atoms_expected   = stats[1,"k_atoms_expected"]
k_density_expected = stats[1,"k_density_expected"]

# For corner.label
library(plotrix)

# If individual LaTeX documents are to be created,
# load the TikZ package
if (use_latex == "true" )
{
   library(tikzDevice)
}

# Colors 
green     = "#249800"
darkgreen = "#237A09"
blue      = "#0020AB"
darkblue  = "#001055"
red       = "#FF0000"

# Plot title
if (config[1, "shuffle"] == "on")
{
   plot.title = "Rate Constant with Shuffling"
} else {
   plot.title = "Rate Constant without Shuffling"
}

# If all plots are to be drawn in a single PDF,
# open a PDF graphics device and set a few parameters
if (use_latex != "true")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(mfrow=c(2,1))
   par(font.lab=2)
}

#################
# PLOT OF ATOMS #
#################

# If individual LaTeX documents are to be created,
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_atoms.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

# Draw scatter plot of k_atoms against density
if (use_latex != "true")
{
   plot(config[,"density"], stats[,"k_atoms"],
      main=plot.title, xlab="Density", ylab="k_atoms", col=green)
} else {
   plot(config[,"density"], stats[,"k_atoms"],
      main=plot.title, xlab="Density", ylab="$k_\text{atoms}$", col=green)
}

# Add expected value line
abline(h=k_atoms_expected,
   lwd=1, lty=5, col=red)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

###################
# PLOT OF DENSITY #
###################

# If individual LaTeX documents are to be created,
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_density.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

# Draw scatter plot of k_density against density
if (use_latex != "true")
{
   plot(config[,"density"], stats[,"k_density"],
      main=plot.title, xlab="Density", ylab="k_density", col=green)
} else {
   plot(config[,"density"], stats[,"k_density"],
      main=plot.title, xlab="Density", ylab="$k$", col=green)
}

# Add expected value line
abline(h=k_density_expected,
   lwd=1, lty=5, col=red)

# End

