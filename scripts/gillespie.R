#!/usr/bin/Rscript
#
# An implementation of the Gillespie algorithm in R
#
# Usage: ./gillespie.R output_type should_integrate path_to_config path_to_output seed
#   output_type       the type of output that the R script should create; valid options
#                        are "pdf", "png", and "latex"
#   should_integrate  whether or not to run the numerical integrator to find the
#                        expected trajectories; valid options are "true" and "false"
#   path_to_config    the path of the config.out file to be read
#   path_to_output    the path of the output that the R script will create, such as
#                        plots and text files (without the file extension)
#   seed              seed for the pseudorandom number generator


# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("GILLESPIE SCRIPT FAILED: Too many parameters!")
   print("  Usage: ./gillespie.R output_type should_integrate path_to_config path_to_output seed")
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
   path_to_output = as.character(Args[9])
} else {
   path_to_output = "gillespie"
}

if (!is.na(Args[10]))
{
   seed = as.integer(Args[10])
} else {
   seed = as.integer(runif(1)*2^31)
}
set.seed(seed)


# An R implementation of the Gillespie algorithm, modified
# from Wilkinson, D. J. 2006. Stochastic Modelling for
# Systems Biology. Chapman & Hall/CRC: Boca Raton, p.155.
# The function takes as arguments a stochastic petri net N
# (which has initial marking M, reactant quantities Pre,
# product quantities Post, and reaction hazard function h),
# a max run duration tmax, and discritization time-step dt.
# The function dumps the simulated data to a file and
# returns the amount of time simulated.
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
      # to fill the interim
      while (t >= targetTime)
      {
         i = i + 1
         targetTime = targetTime + dt

         # Report progress
         if (i %% 512 == 0)
         {
            print(paste("Step: ", i, " of ", maxSteps, " | ", (100*i)%/%maxSteps, "% complete", sep=""))
            print(state)
         }

         tvec[i] = targetTime
         census[i,] = state

         # Write census data to file
         cat(paste(targetTime, " ", sep=""), file=path_to_census, append=TRUE)
         cat(state, file=path_to_census, append=TRUE)
         cat(paste(" ", sum(state), "\n", sep=""), file=path_to_census, append=TRUE)

         # Check for running out of time or extinction
         if (i > maxSteps || haveExtinction(state))
            return(tvec[i])
      }

      # Pick a reaction and update the state
      j = sample(nReactions, 1, prob=h)
      state = state + S[,j]
   }
}


# Import experimental parameters, parse Elements and
# Reactions, calculate reaction hazards, and build a
# stochastic petri net
source("../scripts/_parse_config.R")


# Dump seed and petri net to file
petri_file = paste(path_to_output, "_petri.out", sep="")
write(paste("seed =", seed), file=petri_file)
write("\nN$Pre",    file=petri_file, append=TRUE)
write.table(N$Pre,  file=petri_file, append=TRUE, row.names=FALSE, col.names=FALSE)
write("\nN$Post",   file=petri_file, append=TRUE)
write.table(N$Post, file=petri_file, append=TRUE, row.names=FALSE, col.names=FALSE)
write("\nN$h",      file=petri_file, append=TRUE)
sink(file=petri_file, append=TRUE)
print(N$h)
sink()


# Write the census file header
path_to_census = paste(path_to_output, "_census.out", sep="")
cat("iter ", file=path_to_census)
cat(unlist(ele_names), file=path_to_census, append=TRUE)
cat(" total\n", file=path_to_census, append=TRUE)


# Run the Gillespie algorithm (and remove names from initial
# system state vector for improved efficiency)
names(N$M) = NULL
iters = gillespie(N, tmax=iters, dt=1)
names(N$M) = ele_names


# Import simulated kinetics data
source("../scripts/_parse_census.R")


# Perform a numerical intergration on the rate laws
# associated with the reaction system to find the expected
# trajectories of each Element
source("../scripts/_integrate.R")


# Plot the simulation data and numerical integration data
path_to_plots = paste(path_to_output, "_plots", sep="")
source("../scripts/_plot_kinetics.R")


# End

