#!/usr/bin/Rscript
#
# Subroutine for importing the contents of each stats.out
# file for an entire batch of simulations; depends on the
# existence of path_to_batch and n_simulations


# Check for path_to_batch
if (!exists("path_to_batch"))
{
   sink(stderr())
   print("PARSE BATCH STATS FAILED: path_to_batch is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for n_simulations
if (!exists("n_simulations"))
{
   sink(stderr())
   print("PARSE BATCH STATS FAILED: n_simulations is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Determine the batch name from path_to_batch
batch_name = tail(unlist(strsplit(path_to_batch, "/")), n=1)


batch_stats = list()
for (i in 1:n_simulations)
{
   # Determine the simulation name
   simulation_name = formatC(i, flag="0", width=nchar(n_simulations))

   # Determine the path to the stats.out file and read it
   path_to_stats = paste(path_to_batch, "/", simulation_name, "/stats.",  batch_name, ".", simulation_name, ".out", sep="")
   stats = readLines(path_to_stats)
   stats = strsplit(stats[stats != ""], " ")

   # Copy the data to the batch_stats list
   for (j in 1:length(stats))
   {
      batch_stats[[ stats[[j]][1] ]] = c(batch_stats[[ stats[[j]][1] ]], as.numeric(stats[[j]][2]))
   }
}


# Extract batch parameters and expected values
world_size = batch_stats[["x"]][1] * batch_stats[["y"]][1]
alpha = batch_stats[["alpha"]][1]
expected_mean = batch_stats[["expected_mean"]][1]
expected_var  = batch_stats[["expected_var"]][1]
expected_sd   = batch_stats[["expected_sd"]][1]


# Set completion flag
batch_stats_parsed = TRUE


# End

