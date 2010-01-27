#!/usr/bin/Rscript
#
# Analyzes batch data in R and creates graphs
#
# Usage: ./analysis.R batch experiments iters x y atoms
#   batch        the name of the batch of experiments
#   experiments  the number of experiments belonging to the batch
#   iters        the number of iterations each experiment ran in the batch
#   x            the width of the world in each experiment in the batch
#   y            the height of the world in each experiment in the batch
#   atoms        the number of atoms in each experiment in the batch

# Import command line arguments
Args = commandArgs()
batch = Args[6]
experiments = Args[7]
iters = Args[8]
x = Args[9]
y = Args[10]
atoms = Args[11]

# Change the working directory to the batch directory
setwd(paste("data/batch/", batch, sep=""))

for (i in 1:experiments)
{
   # Import data for experiment i
   this.experiment = formatC(i, flag="0", width=nchar(experiments))
   setwd(this.experiment)
   diff.data = read.table(paste("diffusion.out.", this.experiment, sep=""), header=TRUE)

   # Draw histograms
   pdf(file = paste("histograms_", batch, "_", this.experiment, ".pdf", sep=""))
   par(mfrow = c(2,2))
   hist(diff.data$dx_actual, main="Net Horizontal Displacement (Actual)", xlab="Displacement", ylab="Number of Atoms", breaks=25, col=rgb(0,0.75,0.15))
   hist(diff.data$dx_ideal,  main="Net Horizontal Displacement (Ideal)",  xlab="Displacement", ylab="Number of Atoms", breaks=25, col=rgb(0,0.5,0.1))
   hist(diff.data$dy_actual, main="Net Vertical Displacement (Actual)",   xlab="Displacement", ylab="Number of Atoms", breaks=25, col=rgb(0,0.15,0.75))
   hist(diff.data$dy_ideal,  main="Net Vertical Displacement (Ideal)",    xlab="Displacement", ylab="Number of Atoms", breaks=25, col=rgb(0,0.1,0.5))

   #hist(diff.data$dx_actual, main="Net Horizontal Displacement (Actual)", xlab="Displacement", freq=FALSE)
   #hist(diff.data$dx_ideal,  main="Net Horizontal Displacement (Ideal)",  xlab="Displacement", freq=FALSE)
   #hist(diff.data$dy_actual, main="Net Vertical Displacement (Actual)",   xlab="Displacement", freq=FALSE)
   #hist(diff.data$dy_ideal,  main="Net Vertical Displacement (Ideal)",    xlab="Displacement", freq=FALSE)

   dev.off()
   setwd("..")

   # ks = ks.test(x, "pnorm", mean=0, sd=1)
   # ks$statistic
}

