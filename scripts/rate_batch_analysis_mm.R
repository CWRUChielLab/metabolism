#!/usr/bin/Rscript
#
# Analyzes batch Michaelis-Menten kinetics in R and creates graphs
#
# Usage: ./rate_batch_analysis_mm.R pathtobatch nexperiments pathtoplots shufflingused uselatex
#   pathtobatch    the path of the batch directory
#   nexperiments   the number of experiments in the batch
#   pathtoplots    the path of the PDF or LaTeX documents that the R script
#                    will create without the .pdf or .tex extension
#   shufflingused  if "true" is passed, label plots "with Shuffling"; else
#                    label plots "without Shuffling"
#   uselatex       if "true" is passed, create individual plots in the form
#                    of LaTeX documents; else draw all plots in one PDF

# Import command line arguments
Args = commandArgs()
path_to_batch  = as.character(Args[6])
n_experiments  = as.integer(Args[7])
path_to_plots  = as.character(Args[8])
shuffling_used = as.character(Args[9])
use_latex      = as.character(Args[10])

# Import experimental parameters and statistics
config = list()
stats = list()
for (i in 1:n_experiments)
{
   this_experiment = formatC(i, flag="0", width=nchar(n_experiments))
   this_config = paste(path_to_batch, "/", this_experiment, "/config.", this_experiment, ".out", sep="")
   this_stats  = paste(path_to_batch, "/", this_experiment, "/stats.",  this_experiment, ".out", sep="")
   config = rbind(config, read.table(this_config, header=TRUE))
   stats  = rbind(stats,  read.table(this_stats,  header=TRUE))
}

# Calculate densities and extract batch parameters
config[,"density"] = config[,"atoms"] / (config[,"x"] * config[,"y"])
x = config[1,"x"]
y = config[1,"y"]
world_size = x * y
k1_atoms_expected      = stats[1,"k1_atoms_expected"]
k2_atoms_expected      = stats[1,"k2_atoms_expected"]
k3_atoms_expected      = stats[1,"k3_atoms_expected"]
k1_density_expected    = stats[1,"k1_density_expected"]
k2_density_expected    = stats[1,"k2_density_expected"]
k3_density_expected    = stats[1,"k3_density_expected"]
v_max_atoms_expected   = stats[1,"v_max_atoms_expected"]
v_max_density_expected = stats[1,"v_max_density_expected"]
km_atoms_expected      = (k2_atoms_expected   + k3_atoms_expected)   / k1_atoms_expected
km_density_expected    = (k2_density_expected + k3_density_expected) / k1_density_expected

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
if (shuffling_used == "true")
{
   plot.title = "Saturation of Initial Catalytic\nReaction Rate with Shuffling"
} else {
   plot.title = "Saturation of Initial Catalytic\nReaction Rate without Shuffling"
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
   tikz(file=paste(path_to_plots, "_atoms.tex", sep=""), width=3, height=2.7)#, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

# Draw scatter plot of v0_atoms against Substrate density
if (use_latex != "true")
{
   plot(stats[,"s0_atoms"], stats[,"v0_atoms"],
      main=plot.title, xlab=expression(group("[",S,"]")[0]), ylab=expression(v[0]), ylim=c(0,max(stats[,"v0_atoms"],1.05*v_max_atoms_expected)), col=green)
} else {
   plot(stats[,"s0_atoms"], stats[,"v0_atoms"],
      main=plot.title, xlab="$S_0$", ylab="$v_0$", ylim=c(0,max(stats[,"v0_atoms"],1.05*v_max_atoms_expected)), col=green)
}

# Add expected value lines
curve(v_max_atoms_expected * x / (km_atoms_expected + x), col=red, add=TRUE)
abline(h=v_max_atoms_expected, lty=2)
#lines(c(km_atoms_expected,km_atoms_expected), c(0,v_max_atoms_expected/2), lty=2)
#lines(c(-max(stats[,"s0_atoms"]),km_atoms_expected), c(v_max_atoms_expected/2,v_max_atoms_expected/2), lty=2)

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
   tikz(file=paste(path_to_plots, "_density.tex", sep=""), width=3, height=2.7)#, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

# Draw scatter plot of k_density against density
if (use_latex != "true")
{
   plot(stats[,"s0_density"], stats[,"v0_density"],
      main=plot.title, xlab=expression(group("[",S,"]")[0]), ylab=expression(v[0]), ylim=c(0,max(stats[,"v0_density"],1.05*v_max_density_expected)), col=green)
} else {
   plot(stats[,"s0_density"], stats[,"v0_density"],
      main=plot.title, xlab="$\\left[\\;\\!\\text{S}\\;\\!\\right]_0$", ylab="$v_0$", ylim=c(0,max(stats[,"v0_density"],1.05*v_max_density_expected)), col=green)
}

# Add expected value lines
curve(v_max_density_expected * x / (km_density_expected + x), col=red, add=TRUE)
abline(h=v_max_density_expected, lty=2)
#lines(c(km_density_expected,km_density_expected), c(0,v_max_density_expected/2), lty=2)
#lines(c(-max(stats[,"s0_density"]),km_density_expected), c(v_max_density_expected/2,v_max_density_expected/2), lty=2)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

# End

