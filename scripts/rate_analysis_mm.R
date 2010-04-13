#!/usr/bin/Rscript
#
# Analyzes Michaelis-Menten kinetics in R and creates graphs
#
# Usage: ./rate_analysis_mm.R prob1 prob2 prob3 pathtoconfig pathtodata pathtoplots pathtostats uselatex
#   prob1         the probability of the E + S -> ES reaction occurring
#   prob2         the probability of the ES -> E + S reaction occurring
#   prob3         the probability of the ES -> E + P reaction occurring
#   pathtoconfig  the path of the config.out file
#   pathtodata    the path of the census.out file
#   pathtoplots   the path of the PDF or LaTeX documents that the R script
#                    will create without the .pdf or .tex extension
#   pathtostats   the path of the text file that the R script will create
#   uselatex      if "true" is passed, create individual plots in the form
#                    of LaTeX documents; else draw all plots in one PDF

# Import command line arguments
Args = commandArgs()
prob1          = as.numeric(Args[6])
prob2          = as.numeric(Args[7])
prob3          = as.numeric(Args[8])
path_to_config = as.character(Args[9])
path_to_data   = as.character(Args[10])
path_to_plots  = as.character(Args[11])
path_to_stats  = as.character(Args[12])
use_latex      = as.character(Args[13])

# Import experimental parameters
config = read.table(path_to_config, header=TRUE)
version = as.character(config["version"])
seed = as.integer(config["seed"])
iters = as.integer(config["iters"])
x = as.integer(config["x"])
y = as.integer(config["y"])
atoms = as.integer(config["atoms"])

# For corner.label
library(plotrix)

# If individual LaTeX documents are to be created,
# load the TikZ package
if (use_latex == "true")
{
   library(tikzDevice)
}

# List for exporting statistical test results
stats_data = list()

# Colors
green     = "#249800"
darkgreen = "#237A09"
blue      = "#0020AB"
darkblue  = "#001055"
red       = "#FF0000"

# Import rate data
rate_data = read.table(path_to_data, header=TRUE)

# Remove timesteps that have zero reactant atoms
rate_data = rate_data[which(rate_data$Substrate != 0),]

# Calculate rate values for atoms
s0_atoms = rate_data$Substrate[1]
p0_atoms = rate_data$Product[1]
e0_atoms = rate_data$Enzyme[1] + rate_data$ES[1]

k1_atoms_expected = prob1/10 * 8/(x*y)
k2_atoms_expected = prob2/10 * 8
k3_atoms_expected = prob3/10 * 8

es_atoms = mean(rate_data$ES)
#es_atoms_expected = e0_atoms * k1_atoms_expected / (k1_atoms_expected + k2_atoms_expected + k3_atoms_expected)

v0_atoms = lm(rate_data$Product ~ rate_data$iter + 0)$coefficients
#v0_atoms_expected = k3_atoms_expected * es_atoms_expected

v_max_atoms_expected = k3_atoms_expected * e0_atoms

# Calculate k for densities
s0_density = rate_data$Substrate[1]/(x*y)
p0_density = rate_data$Product[1]/(x*y)
e0_density = (rate_data$Enzyme[1] + rate_data$ES[1])/(x*y)

k1_density_expected = prob1/10 * 8
k2_density_expected = prob2/10 * 8
k3_density_expected = prob3/10 * 8

es_density = mean(rate_data$ES)/(x*y)
#es_density_expected = e0_density * k1_density_expected / (k1_density_expected + k2_density_expected + k3_density_expected)

v0_density = lm(rate_data$Product/(x*y) ~ rate_data$iter + 0)$coefficients
#v0_density_expected = k3_density_expected * es_density_expected

v_max_density_expected = k3_density_expected * e0_density

# Save statistics for exporting
stats_data["s0_atoms"] = s0_atoms
stats_data["s0_density"] = s0_density
stats_data["e0_atoms"] = e0_atoms
stats_data["e0_density"] = e0_density
stats_data["k1_atoms_expected"] = k1_atoms_expected
stats_data["k2_atoms_expected"] = k2_atoms_expected
stats_data["k3_atoms_expected"] = k3_atoms_expected
stats_data["k1_density_expected"] = k1_density_expected
stats_data["k2_density_expected"] = k2_density_expected
stats_data["k3_density_expected"] = k3_density_expected
#stats_data["es_atoms"] = es_atoms
#stats_data["es_atoms_expected"] = es_atoms_expected
#stats_data["es_density"] = es_density
#stats_data["es_density_expected"] = es_density_expected
stats_data["v0_atoms"] = v0_atoms
#stats_data["v0_atoms_expected"] = v0_atoms_expected
stats_data["v0_density"] = v0_density
#stats_data["v0_density_expected"] = v0_density_expected
stats_data["v_max_atoms_expected"] = v_max_atoms_expected
stats_data["v_max_density_expected"] = v_max_density_expected

# If all plots are to be drawn in a single PDF,
# open a PDF graphics device and set a few parameters
if (use_latex != "true")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(mfrow=c(2,2))
   par(font.lab=2)
}

###########################
# Plot atom data directly #
###########################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_atom.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

plot(rate_data$iter, rate_data$Enzyme, col=green, xlab="Iterations", ylab="Atoms", ylim=c(0, max(rate_data$Enzyme, rate_data$Substrate, rate_data$ES, rate_data$Product)))
points(rate_data$iter, rate_data$Substrate, col=blue)
points(rate_data$iter, rate_data$ES, col=darkgreen)
points(rate_data$iter, rate_data$Product, col=darkblue)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

####################################################
# Plot Product atom data with fitted straight line #
####################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_atom_v0.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

plot(rate_data$iter, rate_data$Product, col=darkblue, xlab="Iterations", ylab="Atoms")
abline(a=p0_atoms, b=v0_atoms, col=red)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

##############################
# Plot density data directly #
##############################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_density.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

plot(rate_data$iter, rate_data$Enzyme/(x*y), col=green, xlab="Iterations", ylab="Density", ylim=c(0, max(rate_data$Enzyme, rate_data$Substrate, rate_data$ES, rate_data$Product)/(x*y)))
points(rate_data$iter, rate_data$Substrate/(x*y), col=blue)
points(rate_data$iter, rate_data$ES/(x*y), col=darkgreen)
points(rate_data$iter, rate_data$Product/(x*y), col=darkblue)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

#######################################################
# Plot Product density data with fitted straight line #
#######################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_density_v0.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
   par(cex=0.7)
   par(font.main=1)
}

plot(rate_data$iter, rate_data$Product/(x*y), col=darkblue, xlab="Iterations", ylab="Density")
abline(a=p0_density, b=v0_density, col=red)

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

# Export statistical test results
write.table(stats_data, file=path_to_stats, row.names=FALSE, quote=FALSE)

# End

