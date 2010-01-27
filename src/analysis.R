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
batch = as.character(Args[6])
experiments = as.integer(Args[7])
iters = as.integer(Args[8])
x = as.integer(Args[9])
y = as.integer(Args[10])
atoms = as.integer(Args[11])

# Change the working directory to the batch directory
setwd(paste("data/batch/", batch, sep=""))

# The diffusion coefficient and expected standard deviation
d = 3/8
expected.sd = sqrt(2*d*iters)

# Define the inverse function of
# D_alpha = sqrt(-log(alpha/2)/(2*n))
# for the Kolmogorov-Smirnov test
minalpha = function(D,n) 2*exp(-2*n*D^2)

# Suggested number of bins in each histogram
bins = 30

# Colors 
green = "#249800"
darkgreen = "#237A09"
blue = "#001055"
lightblue = "#0020AB"
red = "#FF0000"

for (i in 1:experiments)
{
   # Import data for experiment i
   this.experiment = formatC(i, flag="0", width=nchar(experiments))
   setwd(this.experiment)
   diff.data = read.table(paste("diffusion.out.", this.experiment, sep=""), header=TRUE)

   # Draw histograms
   pdf(file = paste("histograms_", batch, "_", this.experiment, ".pdf", sep=""))
   par(mfrow = c(2,2))
   hist(diff.data$dx_actual, main="Net Horizontal Displacement (Actual)", xlab="Displacement", breaks=bins, col=green,     freq=FALSE)
   curve(dnorm(x, mean=0, sd=expected.sd), lwd=2, col=red, add=TRUE)
   hist(diff.data$dx_ideal,  main="Net Horizontal Displacement (Ideal)",  xlab="Displacement", breaks=bins, col=darkgreen, freq=FALSE)
   curve(dnorm(x, mean=0, sd=expected.sd), lwd=2, col=red, add=TRUE)
   hist(diff.data$dy_actual, main="Net Vertical Displacement (Actual)",   xlab="Displacement", breaks=bins, col=lightblue, freq=FALSE)
   curve(dnorm(x, mean=0, sd=expected.sd), lwd=2, col=red, add=TRUE)
   hist(diff.data$dy_ideal,  main="Net Vertical Displacement (Ideal)",    xlab="Displacement", breaks=bins, col=blue,      freq=FALSE)
   curve(dnorm(x, mean=0, sd=expected.sd), lwd=2, col=red, add=TRUE)

   # Draw CDFs
   par(mfrow = c(2,2))
   plot(ecdf(diff.data$dx_actual), lwd=1, verticals=TRUE, do.points=FALSE, col=green)
   curve(pnorm(x, mean=0, sd=expected.sd), lwd=2, lty=2, col=red, add=TRUE)
   ks = ks.test(diff.data$dx_actual, "pnorm", mean=0, sd=expected.sd)
   text(200, 0.2, paste("D = ", format(ks$statistic, digits=3), sep=""))
   #text(200, 0,1, paste("minalpha = ", format(minalpha(ks$statistic, atoms), digits=3), sep=""))
   plot(ecdf(diff.data$dx_ideal),  lwd=1, verticals=TRUE, do.points=FALSE, col=darkgreen)
   curve(pnorm(x, mean=0, sd=expected.sd), lwd=2, lty=2, col=red, add=TRUE)
   plot(ecdf(diff.data$dy_actual), lwd=1, verticals=TRUE, do.points=FALSE, col=lightblue)
   curve(pnorm(x, mean=0, sd=expected.sd), lwd=2, lty=2, col=red, add=TRUE)
   plot(ecdf(diff.data$dy_ideal),  lwd=1, verticals=TRUE, do.points=FALSE, col=blue)
   curve(pnorm(x, mean=0, sd=expected.sd), lwd=2, lty=2, col=red, add=TRUE)

   dev.off()
   setwd("..")

   # ks = ks.test(x, "pnorm", mean=0, sd=1)
   # ks$statistic
}

