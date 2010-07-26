#!/usr/bin/Rscript
#
# An implementation of the Gillespie algorithm
#
# Usage: ./gillespie.R output_type path_to_config path_to_plots
#   output_type     the type of output that the R script should create; valid options
#                      are "pdf", "png", and "latex"
#   path_to_config  the path of the config.out file to be read
#   path_to_plots   the path of the output that the R script will create without
#                      the file extension

# Import command line arguments
Args = commandArgs()
if (length(Args) > 8)
{
   sink(stderr())
   print("SIMULATION FAILED: Too many parameters!")
   print("  Usage: ./gillespie.R output_type path_to_config path_to_plots")
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
   path_to_plots = as.character(Args[8])
} else {
   path_to_plots = "plots_gillespie"
}

# Import experimental parameters, parse Elements and
# Reactions, calculate reaction hazards, and build a
# stochastic petri net
source("../scripts/_parse_config.R")

# The Gillespie algorithm, taken from
# Wilkinson, D. J. 2006.
# Stochastic Modelling for Systems Biology.
# Chapman & Hall/CRC: Boca Raton, p.155.
# The function takes a stochastic petri net N (which has
# initial marking M, reactant quantities Pre, product
# quantities Post, and reaction hazard function h), a max
# run duration tmax, and discritization time-step dt as
# arguments.
gillespie = function(N, tmax, dt)
{
   state = N$M
   S = t(N$Post-N$Pre)
   nElements  = nrow(S)
   nReactions = ncol(S)

   maxSteps = tmax %/% dt
   t = 0
   tvec = vector("numeric", maxSteps+1)
   census = matrix(0, ncol=nElements, nrow=maxSteps+1)

   i = 1
   targetTime = 0
   tvec[i] = targetTime
   census[i,] = state

   repeat
   {
      # Calculate time to next reaction and increase t
      h  = N$h(state)
      h0 = sum(h)
      if (h0 < 1e-10)
         t = 1e99
      else
         t = t + rexp(1, h0)

      # If the reaction occurs after the next time a census
      # record needs to be created, copy the latest record
      while (t >= targetTime)
      {
         i = i + 1
         targetTime = targetTime + dt

         # Report progress
         if (i %% 200 == 0)
         {
            print(paste("Step: ", i, " of ", maxSteps, " | ", (100*i)%/%maxSteps, "% complete", sep=""))
            print(state)
         }

         tvec[i] = targetTime
         census[i,] = state

         # Check for running out of time
         if (i > maxSteps)
            return(list(t=tvec, x=census))

         # Check for extinction
         if (haveExtinction(state))
         {
            tvec = tvec[1:i]
            census = census[1:i,]
            return(list(t=tvec, x=census))
         }
      }

      # Pick a reaction and update the state
      j = sample(nReactions, 1, prob=h)
      state = state + S[,j]
   }
}

# Create initial system state
random_types = sample(init, size=atoms, replace=TRUE)
N$M = summary(factor(random_types, levels=ele_names))
names(N$M) = NULL

# Run the Gillespie algorithm
out = gillespie(N, tmax=iters, dt=1)

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
}

###############################
# Observed Density Trajectory #
###############################

# If individual PNG graphics are to be created, open a PNG
# graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_observed.png", sep=""))
   par(font.lab=2)
}

# If individual LaTeX documents are to be created, open a
# TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_observed.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

plot(out$t, out$x[,1]/(x*y),
   main="Observed Density Trajectories (Gillespie)",
   xlab="Time",
   ylab="Density",
   ylim=c(0, max(out$x/(x*y))),
   col=ele_colors[[ ele_names[[1]] ]],
   type="l")

for (i in 2:length(ele_names))
{
   points(out$t, out$x[,i]/(x*y),
      col=ele_colors[[ ele_names[[i]] ]],
      type="l")
}

legend("topright",
   legend=unlist(ele_names),
   fill=unlist(ele_colors),
   bg="white")

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

# End

