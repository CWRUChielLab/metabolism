#!/usr/bin/Rscript
#
# Analyzes reaction rate data in R and creates graphs
#
# Usage: ./rate_analysis.R output_type path_to_config path_to_census path_to_plots path_to_stats
#   output_type       the type of output that the R script should create; valid options
#                        are "pdf", "png", and "latex"
#   path_to_config    the path of the config.out file to be read
#   path_to_census    the path of the census.out file to be read
#   path_to_plots     the path of the output that the R script will create (without
#                        the file extension)
#   path_to_stats     the path of the text file that the R script will create


# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("RATE ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./rate_analysis.R output_type path_to_config path_to_census path_to_plots path_to_stats")
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
   path_to_census = as.character(Args[8])
} else {
   path_to_census = "census.out"
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


# Check that the number of Reactions is exactly one
if (nrow(N$Pre) != 1)
{
   sink(stderr())
   print("RATE ANALYSIS FAILED: Reaction count must equal 1!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Import simulated kinetics data
source("../scripts/_parse_census.R")


# Construct a list for exporting statistical test results
stats_data = list()


# Colors
green     = "#249800"
darkgreen = "#237A09"
blue      = "#0020AB"
darkblue  = "#001055"
red       = "#FF0000"


# ...
null = as.numeric((x*y)-total_data)


# ...
if (sum(N$Pre) == 0 && sum(N$Post) == 1)
{
   # Reaction: * -> A
   rxn_type = 1
   k_ideal_theoretical = constants
   k_adjusted_theoretical = constants / (x*y)
   observable = ele_data[, which(N$Post != 0)[1]]  # Element: A
   ideal_transformed_data    = observable - observable[1]
   adjusted_transformed_data = -log(((x*y)-observable)/null[1])
} else {
   if (sum(N$Pre) == 0 && sum(N$Post) == 2)
   {
      # Reaction: * -> A + B, * -> 2A
      rxn_type = 2
      k_ideal_theoretical = constants
      k_adjusted_theoretical = constants / (x*y)^2
      observable = ele_data[, which(N$Post != 0)[1]]  # Element: A
      coefficient = N$Post[which(N$Post != 0)[1]]
      ideal_transformed_data = (observable - observable[1])/coefficient
      if (coefficient == 1)
      {
         adjusted_transformed_data = (observable-observable[1])/(null[1]^2-2*null[1]*(observable-observable[1]))
      } else {
         if (coefficient == 2)
         {
            adjusted_transformed_data = (observable-observable[1])/(2*null[1]^2-2*null[1]*(observable-observable[1]))
         }
      }
   } else {
      if (sum(N$Pre) == 1 && sum(N$Post) <= 1)
      {
         # Reaction: A -> B, A -> *
         rxn_type = 3
         k_ideal_theoretical = constants
         k_adjusted_theoretical = k_ideal_theoretical
         observable = ele_data[, which(N$Pre != 0)[1]]  # Element: A
         keepers    = which(observable != 0)
         iter_data  = iter_data[keepers]
         observable = observable[keepers]
         ideal_transformed_data    = -log(observable/observable[1])
         adjusted_transformed_data = ideal_transformed_data
      } else {
         if (sum(N$Pre) == 1 && sum(N$Post) == 2)
         {
            # Reaction: A -> B + C, A -> 2B
            rxn_type = 4
            k_ideal_theoretical = constants
            k_adjusted_theoretical = constants / (x*y)
            observable = ele_data[, which(N$Pre != 0)[1]]  # Element: A
            keepers    = which(observable != 0)
            iter_data  = iter_data[keepers]
            observable = observable[keepers]
            ideal_transformed_data = -log(observable/observable[1])
            if (observable[1] != null[1])
            {
               adjusted_transformed_data = log((observable[1]*(null[1]-observable[1]+observable))/(observable*null[1]))/(null[1]-observable[1])
            } else {
               adjusted_transformed_data = (observable[1]-observable)/(observable[1]*observable)
            }
         } else {
            if (sum(N$Pre) == 2 && max(N$Pre) == 1)
            {
               # Reaction: A + B -> C, A + B -> C + D, A + B -> 2C, A + B -> *
               rxn_type = 5
               k_ideal_theoretical = constants
               k_adjusted_theoretical = k_ideal_theoretical
               observable1 = ele_data[, which(N$Pre != 0)[1]]  # Element: A
               observable2 = ele_data[, which(N$Pre != 0)[2]]  # Element: B
               keepers     = which(observable1 != 0 & observable2 != 0)
               iter_data   = iter_data[keepers]
               observable1 = observable1[keepers]
               observable2 = observable2[keepers]
               if (observable1[1] != observable2[1])
               {
                  ideal_transformed_data = 1/(observable2[1]-observable1[1]) * log((observable2*observable1[1])/(observable1*observable2[1]))
               } else {
                  ideal_transformed_data = (observable1[1]-observable1)/(observable1[1]*observable1)
               }
               adjusted_transformed_data = ideal_transformed_data
            } else {
               if(sum(N$Pre) == 2 && max(N$Pre) == 2)
               {
                  # Reaction: 2A -> B, 2A -> B + C, 2A -> 2B, 2A -> *
                  rxn_type = 6
                  k_ideal_theoretical = constants 
                  k_adjusted_theoretical = k_ideal_theoretical
                  observable = ele_data[, which(N$Pre != 0)[1]]  # Element: A
                  keepers    = which(observable != 0)
                  iter_data  = iter_data[keepers]
                  observable = observable[keepers]
                  ideal_transformed_data = (observable[1]-observable)/(2*observable[1]*observable)
                  adjusted_transformed_data = ideal_transformed_data
               } else {
                  sink(stderr())
                  print("RATE ANALYSIS FAILED: Reaction type unrecognized!")
                  sink()
                  q(save="no", status=1, runLast=FALSE)
               }
            }
         }
      }
   }
}
k_ideal_empirical    = lm(ideal_transformed_data    ~ 0 + iter_data)$coefficients
k_adjusted_empirical = lm(adjusted_transformed_data ~ 0 + iter_data)$coefficients


# Save statistics for exporting
stats_data["rxn_type"]               = rxn_type
stats_data["k_ideal_theoretical"]    = k_ideal_theoretical
stats_data["k_adjusted_theoretical"] = k_adjusted_theoretical
stats_data["k_ideal_empirical"]      = k_ideal_empirical
stats_data["k_adjusted_empirical"]   = k_adjusted_empirical


# Load the plotrix package for corner.label
library(plotrix)


# If individual LaTeX documents are to be created, load the
# TikZ package
if (output_type == "latex")
{
   library(tikzDevice)
}


# If all plots are to be drawn in a single PDF, open a PDF
# graphics device and set a few parameters
if (output_type == "pdf")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(font.lab=2)
   par(mfrow=c(1,2))
}


######################
# STRAIGHT LINE PLOT #
######################

# If individual PNG graphics are to be created, open a PNG
# graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_straight.png", sep=""))
   par(font.lab=2)
}

# If individual LaTeX documents are to be created, open a
# TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_straight.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

plot(iter_data, ideal_transformed_data,
   col=green)
curve(k_ideal_empirical*x,
   add=TRUE)
curve(k_ideal_theoretical*x,
   col=red,
   add=TRUE)

plot(iter_data, adjusted_transformed_data,
   col=blue)
curve(k_adjusted_empirical*x,
   add=TRUE)
curve(k_adjusted_theoretical*x,
   col=red,
   add=TRUE)

# If individual LaTeX documents or PNG graphics are to be
# created, close the current graphics device
if (output_type == "png" || output_type == "latex")
{
   dev.off()
}


#############
# End Plots #
#############

# If all plots are to be drawn in a single PDF, close the
# current graphics device
if (output_type == "pdf")
{
   dev.off()
}


# Dump statistics to file
f = file(path_to_stats, "w")
for( i in 1:length(stats_data) )
{
   cat(names(stats_data)[i], as.numeric(stats_data[i]), "\n", file=f)
}
close(f)


# End

