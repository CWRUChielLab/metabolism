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

# For corner.label
library(plotrix)

# Change the working directory to the batch directory
setwd(paste("data/batch/", batch, sep=""))

# The diffusion coefficient and expected standard deviation
d = 3/8
expected_sd = sqrt(2*d*iters)

# Suggested number of bins in each histogram
bins = 30

# Colors 
green = "#249800"
darkgreen = "#237A09"
blue = "#001055"
lightblue = "#0020AB"
red = "#FF0000"

# Plot Attributes
plot_title = c("Net Horizontal Displacement (Actual)",
               "Net Horizontal Displacement (Ideal)",
               "Net Vertical Displacement (Actual)",
               "Net Vertical Displacement (Ideal)")
plot_color = c(green,
               darkgreen,
               lightblue,
               blue)

# Kolmogorov-Smirnov test results for batch
batch_data = matrix(0, nrow=experiments, ncol=9, dimnames=list(NULL,c("n",
                                                                      "dx_actual_D",
                                                                      "dx_actual_p",
                                                                      "dx_ideal_D",
                                                                      "dx_ideal_p",
                                                                      "dy_actual_D",
                                                                      "dy_actual_p",
                                                                      "dy_ideal_D",
                                                                      "dy_ideal_p")))

# Iterate over all experiments in the batch
for (i in 1:experiments)
{
   batch_data[i,1] = atoms

   # Change the working directory and import data for the currect experiment
   this_experiment = formatC(i, flag="0", width=nchar(experiments))
   setwd(this_experiment)
   diffusion_data = read.table(paste("diffusion.out.", this_experiment, sep=""), header=TRUE)
   current_data = cbind(diffusion_data$dx_actual,
                        diffusion_data$dx_ideal,
                        diffusion_data$dy_actual,
                        diffusion_data$dy_ideal)

   # Open a PDF graphics device and set a few parameters
   pdf(file = paste("analysis_", batch, "_", this_experiment, ".pdf", sep=""), family="Palatino")
   par(mfrow = c(2,2))
   par(font.lab=2)

   # Draw histograms and find confidence intervals
   library(lattice)
   for (j in 1:4)
   {
      hist(current_data[,j], main=plot_title[j], xlab="Displacement", breaks=bins, col=plot_color[j], freq=FALSE)
      curve(dnorm(x, mean=0, sd=expected_sd), lwd=2, col=red, add=TRUE)

      alpha = 0.05
      data_mean = mean(current_data[,j])
      data_var = var(current_data[,j])
      data_sd = sqrt(data_var)
      data_mean_lower_ci = data_mean - qt(1-alpha/2, df=atoms-1)*data_sd/sqrt(atoms)
      data_mean_upper_ci = data_mean + qt(1-alpha/2, df=atoms-1)*data_sd/sqrt(atoms)
      data_var_lower_ci = (atoms-1) * data_var / qchisq(1-alpha/2, df=atoms-1)
      data_var_upper_ci = (atoms-1) * data_var / qchisq(alpha/2,   df=atoms-1)
      data_sd_lower_ci = sqrt(data_var_lower_ci)
      data_sd_upper_ci = sqrt(data_var_upper_ci)

      ##############################
      # Draw CI range as piecewise #
      ##############################

      curve(ifelse(
               x < data_mean_lower_ci - data_sd_upper_ci,
                  dnorm(x, mean=data_mean_lower_ci, sd=data_sd_upper_ci),
            ifelse(
               x < data_mean_lower_ci - data_sd_lower_ci,
                  -1/((x-data_mean_lower_ci)*sqrt(2*exp(1)*pi)),
            ifelse(
               x < data_mean_lower_ci,
                  dnorm(x, mean=data_mean_lower_ci, sd=data_sd_lower_ci),
            ifelse(
               x < data_mean_upper_ci,
                  dnorm(0, mean=0, sd=data_sd_lower_ci),
            ifelse(
               x < data_mean_upper_ci + data_sd_lower_ci,
                  dnorm(x, mean=data_mean_upper_ci, sd=data_sd_lower_ci),
            ifelse(
               x < data_mean_upper_ci + data_sd_upper_ci,
                  1/((x-data_mean_upper_ci)*sqrt(2*exp(1)*pi)),
            #else
                  dnorm(x, mean=data_mean_upper_ci, sd=data_sd_upper_ci)
            )))))), lwd=1, lty=5, col=red, add=TRUE)

      q = sqrt((data_var_lower_ci*data_var_upper_ci)/(data_var_lower_ci-data_var_upper_ci)*log(data_var_lower_ci/data_var_upper_ci))
      if (data_mean_upper_ci - q < data_mean)
         curve(ifelse(
                  x < data_mean_upper_ci - q,
                     dnorm(x, mean=data_mean_upper_ci, sd=data_sd_lower_ci),
               ifelse(
                  x < data_mean,
                     dnorm(x, mean=data_mean_upper_ci, sd=data_sd_upper_ci),
               ifelse(
                  x < data_mean_lower_ci + q,
                     dnorm(x, mean=data_mean_lower_ci, sd=data_sd_upper_ci),
               #else
                     dnorm(x, mean=data_mean_lower_ci, sd=data_sd_lower_ci)
               ))), lwd=1, lty=5, col=red, add=TRUE)
      else
         curve(ifelse(
                  x < data_mean,
                     dnorm(x, mean=data_mean_upper_ci, sd=data_sd_lower_ci),
                  #else
                     dnorm(x, mean=data_mean_lower_ci, sd=data_sd_lower_ci)
               ), lwd=1, lty=5, col=red, add=TRUE)

      ########################
      # Statistical labeling #
      ########################

      n_label = substitute(paste(N == n),
                           list(n=atoms))
      mean_label = substitute(paste(bar(x) == x_mean, " (", mean_lower_ci, ", ", mean_upper_ci, ")"),
                           list(x_mean=signif(data_mean, digits=3),
                                mean_lower_ci=signif(data_mean_lower_ci, digits=3),
                                mean_upper_ci=signif(data_mean_upper_ci, digits=3)))
      sd_label = substitute(paste(s == x_sd, " (", sd_lower_ci, ", ", sd_upper_ci, ")"),
                           list(x_sd=signif(data_sd, digits=4),
                                sd_lower_ci=signif(data_sd_lower_ci, digits=4),
                                sd_upper_ci=signif(data_sd_upper_ci, digits=4)))
      ci_label = substitute(paste("(", ci, "% CIs)"),
                           list(ci=100*(1-alpha)))
      expected_label = substitute(paste(mu == theory_mean, ", ", sigma == theory_sd),
                           list(theory_mean=0,
                                theory_sd=signif(expected_sd, digits=4)))
      old_size = par("cex")
      par(cex=old_size*0.8)
      corner.label(n_label,        x=-1, y=1, yoff=1.0*strheight("m"))
      corner.label(mean_label,     x=-1, y=1, yoff=2.5*strheight("m"))
      corner.label(sd_label,       x=-1, y=1, yoff=4.0*strheight("m"))
      corner.label(ci_label,       x=-1, y=1, yoff=5.5*strheight("m"))
      corner.label(expected_label, x=1,  y=1, yoff=1.0*strheight("m"))
      par(cex=old_size)
   }

   # Draw CDFs and conduct Kolmogorov-Smirnov test
   for (j in 1:4)
   {
      plot(ecdf(current_data[,j]), main=plot_title[j], xlab="Displacement", ylab="Cumulative Density", lwd=1, verticals=TRUE, do.points=FALSE, col=plot_color[j])
      curve(pnorm(x, mean=0, sd=expected_sd), lwd=2, lty=2, col=red, add=TRUE)
      ks_results = suppressWarnings(ks.test(current_data[,j], "pnorm", mean=0, sd=expected_sd))
      corner.label(paste("D = ", format(ks_results$statistic, digits=3), sep=""), x=1, y=-1, yoff=3.0*strheight("m"))
      corner.label(paste("p = ", format(ks_results$p.value,   digits=3), sep=""), x=1, y=-1, yoff=1.5*strheight("m"))

      batch_data[i,2*j]   = ks_results$statistic
      batch_data[i,2*j+1] = ks_results$p.value
   }

   # Close the PDF graphics device and reset the working directory
   dev.off()
   setwd("..")
}

# Export batch_data
write.table(batch_data, file="batch_data", row.names=FALSE, quote=FALSE)


####################################
# Run statistics on batch p-values #
####################################

# Open a PDF graphics device and set a few parameters
pdf(file = paste(batch, "_", "ks_test.pdf", sep=""), family="Palatino")
par(mfrow = c(2,2))
par(font.lab=2)

current_data = cbind(batch_data[,"dx_actual_p"],
                     batch_data[,"dx_ideal_p"],
                     batch_data[,"dy_actual_p"],
                     batch_data[,"dy_ideal_p"])

# Draw histograms
for (i in 1:4)
{
   hist(current_data[,i], main=plot_title[i], xlab="p-value", col=plot_color[i], freq=FALSE)
   curve(dunif(x, min=0, max=1), lwd=2, col=red, add=TRUE)
}

# Draw CDFs and conduct Kolmogorov-Smirnov test
for (i in 1:4)
{
   plot(ecdf(current_data[,i]), main=plot_title[i], xlab="p-value", ylab="Cumulative Density", lwd=1, verticals=TRUE, do.points=FALSE, col=plot_color[i])
   curve(punif(x, min=0, max=1), lwd=2, lty=2, col=red, add=TRUE)
   ks_results = suppressWarnings(ks.test(current_data[,i], "punif", min=0, max=1))
   corner.label(paste("D = ", format(ks_results$statistic, digits=3), sep=""), x=1, y=-1, yoff=3.0*strheight("m"))
   corner.label(paste("p = ", format(ks_results$p.value,   digits=3), sep=""), x=1, y=-1, yoff=1.5*strheight("m"))
}

# End

