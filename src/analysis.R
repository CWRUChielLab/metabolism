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
expected_sd = sqrt(2*d*iters)

# Define the inverse function of
# D_alpha = sqrt(-log(alpha/2)/(2*n))
# for the Kolmogorov-Smirnov test
min_alpha = function(D,n) 2*exp(-2*n*D^2)

# Suggested number of bins in each histogram
bins = 30

# Colors 
green = "#249800"
darkgreen = "#237A09"
blue = "#001055"
lightblue = "#0020AB"
red = "#FF0000"

# Iterate over all experiments in the batch
for (i in 1:experiments)
{
   # Change the working directory and import data for the currect experiment
   this_experiment = formatC(i, flag="0", width=nchar(experiments))
   setwd(this_experiment)
   diffusion_data = read.table(paste("diffusion.out.", this_experiment, sep=""), header=TRUE)

   # Open a PDF graphics device and set a few parameters
   pdf(file = paste("analysis_", batch, "_", this_experiment, ".pdf", sep=""), family="Palatino")
   par(mfrow = c(2,2))
   par(font.lab=2)
   #par(font.axis=2)
   #par(font=3)

   # Draw histograms
   current_data = cbind(diffusion_data$dx_actual,
                        diffusion_data$dx_ideal,
                        diffusion_data$dy_actual,
                        diffusion_data$dy_ideal)
   plot_title = c("Net Horizontal Displacement (Actual)",
                  "Net Horizontal Displacement (Ideal)",
                  "Net Vertical Displacement (Actual)",
                  "Net Vertical Displacement (Ideal)")
   plot_color = c(green,
                  darkgreen,
                  lightblue,
                  blue)
   for (j in 1:4)
   {
      hist(current_data[,j], main=plot_title[j], xlab="Displacement", breaks=bins, col=plot_color[j], freq=FALSE)
      curve(dnorm(x, mean=0, sd=expected_sd), lwd=2, col=red, add=TRUE)
   }

   # Draw CDFs and conduct Kolmogorov-Smirnov test
   current_data = cbind(diffusion_data$dx_actual,
                        diffusion_data$dx_ideal,
                        diffusion_data$dy_actual,
                        diffusion_data$dy_ideal)
   plot_title = c("Net Horizontal Displacement (Actual)",
                  "Net Horizontal Displacement (Ideal)",
                  "Net Vertical Displacement (Actual)",
                  "Net Vertical Displacement (Ideal)")
   plot_color = c(green,
                  darkgreen,
                  lightblue,
                  blue)
   for (j in 1:4)
   {
      plot(ecdf(current_data[,j]), main=plot_title[j], xlab="Displacement", ylab="Cumulative Density", lwd=1, verticals=TRUE, do.points=FALSE, col=plot_color[j])
      curve(pnorm(x, mean=0, sd=expected_sd), lwd=2, lty=2, col=red, add=TRUE)
      ks_results = suppressWarnings(ks.test(current_data[,j], "pnorm", mean=0, sd=expected_sd))
      min_value = min(current_data[,j])
      max_value = max(current_data[,j])
      data_range = max_value-min_value
      text(min_value+0.8*data_range, 0.3, paste("D = ", format(ks_results$statistic, digits=3), sep=""))
      text(min_value+0.8*data_range, 0.2, paste("p = ", format(ks_results$p.value, digits=3), sep=""))
      text(min_value+0.8*data_range, 0.1, bquote(alpha[min] ~ " = " ~ .(format(min_alpha(ks_results$statistic, atoms), digits=3))))
   }

   # Close the PDF graphics device and reset the working directory
   dev.off()
   setwd("..")
}

