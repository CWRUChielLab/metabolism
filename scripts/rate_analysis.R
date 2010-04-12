#!/usr/bin/Rscript
#
# Analyzes reaction rate data in R and creates graphs
#
# Usage: ./rate_analysis.R rxnorder prob pathtoconfig pathtodata pathtoplots pathtostats uselatex
#   rxnorder      the order of the reaction; "first" or "second"
#   prob          the probability of the reaction occurring
#   pathtoconfig  the path of the config.out file
#   pathtodata    the path of the census.out file
#   pathtoplots   the path of the PDF or LaTeX documents that the R script
#                    will create without the .pdf or .tex extension
#   pathtostats   the path of the text file that the R script will create
#   uselatex      if "true" is passed, create individual plots in the form
#                    of LaTeX documents; else draw all plots in one PDF

# Import command line arguments
Args = commandArgs()
rxn_order      = as.character(Args[6])
prob           = as.numeric(Args[7])
path_to_config = as.character(Args[8])
path_to_data   = as.character(Args[9])
path_to_plots  = as.character(Args[10])
path_to_stats  = as.character(Args[11])
use_latex      = as.character(Args[12])

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
if (rxn_order == "first")
{
   rate_data = rate_data[which(rate_data$A != 0),]
}
if (rxn_order == "second")
{
   rate_data = rate_data[which(rate_data$A != 0 & rate_data$B != 0),]
}

# Calculate k for atoms
a0_atoms = rate_data$A[1]
b0_atoms = rate_data$B[1]
if (rxn_order == "first")
{
   k_atoms = lm(-log(rate_data$A/a0_atoms)
               ~ 0 + rate_data$iter)$coefficients
   k_atoms_expected = prob/10
}
if (rxn_order == "second")
{
   if (a0_atoms != b0_atoms)
   {
      k_atoms = lm(log((rate_data$A*b0_atoms)/(rate_data$B*a0_atoms))/(a0_atoms-b0_atoms)
                  ~ 0 + rate_data$iter)$coefficients
   } else {
      k_atoms = lm(1/rate_data$A - 1/a0_atoms
                  ~ 0 + rate_data$iter)$coefficients
   }
   k_atoms_expected = prob/10 * 8/(x*y)
}

# Calculate k for densities
a0_density = rate_data$A[1]/(x*y)
b0_density = rate_data$B[1]/(x*y)
if (rxn_order == "first")
{
   k_density = lm(-log((rate_data$A/(x*y))/a0_density)
                  ~ 0 + rate_data$iter)$coefficients
   k_density_expected = prob/10
}
if (rxn_order == "second")
{
   if (a0_density != b0_density)
   {
      k_density = lm(log((rate_data$A*b0_density)/(rate_data$B*a0_density))/(a0_density-b0_density)
                     ~ 0 + rate_data$iter)$coefficients
   } else {
      k_density = lm((x*y)/rate_data$A - 1/a0_density
                     ~ 0 + rate_data$iter)$coefficients
   }
   k_density_expected = prob/10 * 8
}

# Save statistics for exporting
stats_data["k_atoms"] = k_atoms
stats_data["k_atoms_expected"] = k_atoms_expected
stats_data["k_density"] = k_density
stats_data["k_density_expected"] = k_density_expected

# If all plots are to be drawn in a single PDF,
# open a PDF graphics device and set a few parameters
if (use_latex != "true")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(mfrow=c(2,2))
   par(font.lab=2)
}

##################################################
# Plot atom data directly with fitted trajectory #
##################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_atom_exp.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

if (rxn_order == "first")
{
   plot(rate_data$iter,   rate_data$A, col=green, xlab="Iterations", ylab="Atoms", ylim=c(0, max(rate_data$A, rate_data$B)))
   points(rate_data$iter, rate_data$B, col=blue)
   curve(a0_atoms*exp(-k_atoms*x), add=TRUE)
   curve(a0_atoms-a0_atoms*exp(-k_atoms*x), add=TRUE)
   curve(a0_atoms*exp(-k_atoms_expected*x), col=red, add=TRUE)
   curve(a0_atoms-a0_atoms*exp(-k_atoms_expected*x), col=red, add=TRUE)
}
if (rxn_order == "second")
{
   plot(rate_data$iter,   rate_data$A, col=green, xlab="Iterations", ylab="Atoms", ylim=c(0,max(rate_data$A, rate_data$B)))
   points(rate_data$iter, rate_data$B, col=blue)
   if (a0_atoms != b0_atoms)
   {
      curve(a0_atoms*(b0_atoms-a0_atoms)/(b0_atoms*exp(k_atoms*(b0_atoms-a0_atoms)*x)-a0_atoms), add=TRUE)
      curve(b0_atoms*(a0_atoms-b0_atoms)/(a0_atoms*exp(k_atoms*(a0_atoms-b0_atoms)*x)-b0_atoms), add=TRUE)
      curve(a0_atoms*(b0_atoms-a0_atoms)/(b0_atoms*exp(k_atoms_expected*(b0_atoms-a0_atoms)*x)-a0_atoms), col=red, add=TRUE)
      curve(b0_atoms*(a0_atoms-b0_atoms)/(a0_atoms*exp(k_atoms_expected*(a0_atoms-b0_atoms)*x)-b0_atoms), col=red, add=TRUE)
   } else {
      curve(1/(k_atoms*x+1/a0_atoms), add=TRUE)
      curve(1/(k_atoms*x+1/b0_atoms), add=TRUE)
      curve(1/(k_atoms_expected*x+1/a0_atoms), col=red, add=TRUE)
      curve(1/(k_atoms_expected*x+1/b0_atoms), col=red, add=TRUE)
   }
}

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

###############################################################
# Plot linear-transformed atom data with fitted straight line #
###############################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_atom_linear.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

if (rxn_order == "first")
{
   plot(rate_data$iter, -log(rate_data$A/a0_atoms), col=green, xlab="Iterations", ylab="")
   curve(k_atoms*x, add=TRUE)
   curve(k_atoms_expected*x, col=red, add=TRUE)
}
if (rxn_order == "second")
{
   if (a0_atoms != b0_atoms)
   {
      plot(rate_data$iter,   log((rate_data$A*b0_atoms)/(rate_data$B*a0_atoms))/(a0_atoms-b0_atoms), col=green, xlab="Iterations", ylab="1/Atoms")
      points(rate_data$iter, log((rate_data$B*a0_atoms)/(rate_data$A*b0_atoms))/(b0_atoms-a0_atoms), col=blue)
   } else {
      plot(rate_data$iter,   1/rate_data$A-1/a0_atoms, col=green, xlab="Iterations", ylab="1/Atoms")
      points(rate_data$iter, 1/rate_data$B-1/b0_atoms, col=blue)
   }
   curve(k_atoms*x, add=TRUE)
   curve(k_atoms_expected*x, col=red, add=TRUE)
}

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

#####################################################
# Plot density data directly with fitted trajectory #
#####################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_density_exp.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

if (rxn_order == "first")
{
   plot(rate_data$iter,   rate_data$A/(x*y), col=green, xlab="Iterations", ylab="Density", ylim=c(0, max(rate_data$A, rate_data$B)/(x*y)))
   points(rate_data$iter, rate_data$B/(x*y), col=blue)
   curve(a0_density*exp(-k_density*x), add=TRUE)
   curve(a0_density-a0_density*exp(-k_density*x), add=TRUE)
   curve(a0_density*exp(-k_density_expected*x), col=red, add=TRUE)
   curve(a0_density-a0_density*exp(-k_density_expected*x), col=red, add=TRUE)
}
if (rxn_order == "second")
{
   plot(rate_data$iter,   rate_data$A/(x*y), col=green, xlab="Iterations", ylab="Density", ylim=c(0,max(rate_data$A, rate_data$B)/(x*y)))
   points(rate_data$iter, rate_data$B/(x*y), col=blue)
   if (a0_density != b0_density)
   {
      curve(a0_density*(b0_density-a0_density)/(b0_density*exp(k_density*(b0_density-a0_density)*x)-a0_density), add=TRUE)
      curve(b0_density*(a0_density-b0_density)/(a0_density*exp(k_density*(a0_density-b0_density)*x)-b0_density), add=TRUE)
      curve(a0_density*(b0_density-a0_density)/(b0_density*exp(k_density_expected*(b0_density-a0_density)*x)-a0_density), col=red, add=TRUE)
      curve(b0_density*(a0_density-b0_density)/(a0_density*exp(k_density_expected*(a0_density-b0_density)*x)-b0_density), col=red, add=TRUE)
   } else {
      curve(1/(k_density*x+1/a0_density), add=TRUE)
      curve(1/(k_density*x+1/b0_density), add=TRUE)
      curve(1/(k_density_expected*x+1/a0_density), col=red, add=TRUE)
      curve(1/(k_density_expected*x+1/b0_density), col=red, add=TRUE)
   }
}

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

##################################################################
# Plot linear-transformed density data with fitted straight line #
##################################################################

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (use_latex == "true")
{
   tikz(file=paste(path_to_plots, "_density_linear.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

if (rxn_order == "first")
{
   plot(rate_data$iter, -log((rate_data$A/(x*y))/a0_density), col=green, xlab="Iterations", ylab="")
   curve(k_density*x, add=TRUE)
   curve(k_density_expected*x, col=red, add=TRUE)
}
if (rxn_order == "second")
{
   if (a0_density != b0_density)
   {
      plot(rate_data$iter,   log((rate_data$A*b0_density)/(rate_data$B*a0_density))/(a0_density-b0_density), col=green, xlab="Iterations", ylab="1/Density")
      points(rate_data$iter, log((rate_data$B*a0_density)/(rate_data$A*b0_density))/(b0_density-a0_density), col=blue)
   } else {
      plot(rate_data$iter,   (x*y)/rate_data$A-1/a0_density, col=green, xlab="Iterations", ylab="1/Density")
      points(rate_data$iter, (x*y)/rate_data$B-1/b0_density, col=blue)
   }
   curve(k_density*x, add=TRUE)
   curve(k_density_expected*x, col=red, add=TRUE)
}

# If individual LaTeX documents are to be created,
# close the current TikZ graphics device
if (use_latex == "true")
{
   dev.off()
}

# Export statistical test results
write.table(stats_data, file=path_to_stats, row.names=FALSE, quote=FALSE)

# End

