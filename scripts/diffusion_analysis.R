#!/usr/bin/Rscript
#
# Analyzes diffusion data in R and creates graphs
#
# Usage: ./diffusion_analysis.R pathtoconfig pathtodata pathtoplots pathtostats uselatex
#   pathtoconfig  the path of the config.out file
#   pathtodata    the path of the diffusion.out file
#   pathtoplots   the path of the PDF or LaTeX documents that the R script
#                    will create without the .pdf or .tex extension
#   pathtostats   the path of the text file that the R script will create
#   uselatex      if "true" is passed, create individual plots in the form
#                    of LaTeX documents; else draw all plots in one PDF

# Import command line arguments
Args = commandArgs()
path_to_config = as.character(Args[6])
path_to_data   = as.character(Args[7])
path_to_plots  = as.character(Args[8])
path_to_stats  = as.character(Args[9])
use_latex      = as.character(Args[10])

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
stats_data = list();

# Diffusion coefficient
d = 3/8

# Calculate expected statistical parameters
alpha = 0.05
expected_mean = 0
expected_var  = 2*d*iters
expected_sd   = sqrt(expected_var)
expected_mean_lower_ci = expected_mean - qnorm(1-alpha/2) * expected_sd / sqrt(atoms)
expected_mean_upper_ci = expected_mean + qnorm(1-alpha/2) * expected_sd / sqrt(atoms)
expected_var_lower_ci  = qchisq(alpha/2,   df=atoms-1) * expected_var / (atoms-1)
expected_var_upper_ci  = qchisq(1-alpha/2, df=atoms-1) * expected_var / (atoms-1)
expected_sd_lower_ci   = sqrt(expected_var_lower_ci)
expected_sd_upper_ci   = sqrt(expected_var_upper_ci)

# Save statistics for exporting
stats_data["alpha"] = alpha
stats_data["expected_mean"] = expected_mean
stats_data["expected_var"]  = expected_var
stats_data["expected_sd"]   = expected_sd
stats_data["expected_mean_lower_ci"] = expected_mean_lower_ci
stats_data["expected_mean_upper_ci"] = expected_mean_upper_ci
stats_data["expected_var_lower_ci"]  = expected_var_lower_ci
stats_data["expected_var_upper_ci"]  = expected_var_upper_ci
stats_data["expected_sd_lower_ci"]   = expected_sd_lower_ci
stats_data["expected_sd_upper_ci"]   = expected_sd_upper_ci

# Suggested number of bins in each histogram
bins = 30

# Colors 
green     = "#249800"
darkgreen = "#237A09"
blue      = "#0020AB"
darkblue  = "#001055"
red       = "#FF0000"

# Plot attributes
plot_type  = c("dx_actual",
               "dx_ideal",
               "dy_actual",
               "dy_ideal")
{
   if (use_latex == "true")
      plot_title = c("Displacement in the\n$x$-Dimension with Collisions",
                     "Displacement in the\n$x$-Dimension without Collisions",
                     "Displacement in the\n$y$-Dimension with Collisions",
                     "Displacement in the\n$y$-Dimension without Collisions")
   else
      plot_title = c("Displacement in the\nx-Dimension with Collisions",
                     "Displacement in the\nx-Dimension without Collisions",
                     "Displacement in the\ny-Dimension with Collisions",
                     "Displacement in the\ny-Dimension without Collisions")
}
plot_color = c(green,
               darkgreen,
               blue,
               darkblue)

# Import diffusion data
diffusion_data = read.table(path_to_data, header=TRUE)

# If all plots are to be drawn in a single PDF,
# open a PDF graphics device and set a few parameters
if (use_latex != "true")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(mfrow=c(2,2))
   par(font.lab=2)
}

############
# QQ PLOTS #
############

for (i in 1:length(plot_type))
{
   # If individual LaTeX documents are to be created,
   # open a TikZ graphics device and set a few parameters
   if (use_latex == "true")
   {
      tikz(file=paste(path_to_plots, "_", plot_type[i], "_qq.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw normal Q-Q plot
   qqnorm(diffusion_data[,plot_type[i]], main=plot_title[i], col=plot_color[i])

   #Add a straight line corresponding to the expected distribution
   abline(a=expected_mean, b=expected_sd, col=red)

   # If individual LaTeX documents are to be created,
   # close the current TikZ graphics device
   if (use_latex == "true")
   {
      dev.off()
   }
}

##############
# HISTOGRAMS #
##############

for (i in 1:length(plot_type))
{
   # If individual LaTeX documents are to be created,
   # open a TikZ graphics device and set a few parameters
   if (use_latex == "true")
   {
      tikz(file=paste(path_to_plots, "_", plot_type[i], "_hist.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw histogram
   hist(diffusion_data[,plot_type[i]], main=plot_title[i], xlab="Displacement", breaks=bins, col=plot_color[i], freq=FALSE)

   # Calculate statistics on data
   data_mean = mean(diffusion_data[,plot_type[i]])
   data_var  = var(diffusion_data[,plot_type[i]])
   data_sd   = sqrt(data_var)
   data_mean_lower_ci = data_mean - qt(1-alpha/2, df=atoms-1) * data_sd / sqrt(atoms)
   data_mean_upper_ci = data_mean + qt(1-alpha/2, df=atoms-1) * data_sd / sqrt(atoms)
   data_var_lower_ci  = (atoms-1) * data_var / qchisq(1-alpha/2, df=atoms-1)
   data_var_upper_ci  = (atoms-1) * data_var / qchisq(alpha/2,   df=atoms-1)
   data_sd_lower_ci   = sqrt(data_var_lower_ci)
   data_sd_upper_ci   = sqrt(data_var_upper_ci)

   # Save statistics for exporting
   stats_data[paste(plot_type[i], "_mean", sep="")] = data_mean
   stats_data[paste(plot_type[i], "_var", sep="")]  = data_var
   stats_data[paste(plot_type[i], "_sd", sep="")]   = data_sd
   stats_data[paste(plot_type[i], "_mean_lower_ci", sep="")] = data_mean_lower_ci
   stats_data[paste(plot_type[i], "_mean_upper_ci", sep="")] = data_mean_upper_ci
   stats_data[paste(plot_type[i], "_var_lower_ci", sep="")]  = data_var_lower_ci
   stats_data[paste(plot_type[i], "_var_upper_ci", sep="")]  = data_var_upper_ci
   stats_data[paste(plot_type[i], "_sd_lower_ci", sep="")]   = data_sd_lower_ci
   stats_data[paste(plot_type[i], "_sd_upper_ci", sep="")]   = data_sd_upper_ci

   # Plot normal PDF using sample mean and sample standard deviation
   curve(dnorm(x, mean=data_mean, sd=data_sd), lwd=1, add=TRUE)

   ###################################################
   # Draw expected PDF CI as two piecewise functions #
   ###################################################

   # Upper bound
   curve(ifelse(
            x < expected_mean_lower_ci - expected_sd_upper_ci,
               dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_upper_ci),
         ifelse(
            x < expected_mean_lower_ci - expected_sd_lower_ci,
               -1/((x-expected_mean_lower_ci)*sqrt(2*exp(1)*pi)),
         ifelse(
            x < expected_mean_lower_ci,
               dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_lower_ci),
         ifelse(
            x < expected_mean_upper_ci,
               dnorm(0, mean=0, sd=expected_sd_lower_ci),
         ifelse(
            x < expected_mean_upper_ci + expected_sd_lower_ci,
               dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_lower_ci),
         ifelse(
            x < expected_mean_upper_ci + expected_sd_upper_ci,
               1/((x-expected_mean_upper_ci)*sqrt(2*exp(1)*pi)),
         #else
               dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_upper_ci)
         )))))), lwd=1, lty=2, col=red, add=TRUE)

   # Lower bound
   q = sqrt((expected_var_lower_ci*expected_var_upper_ci) / (expected_var_lower_ci-expected_var_upper_ci)
               * log(expected_var_lower_ci/expected_var_upper_ci))
   if (expected_mean_upper_ci - q < expected_mean)
      curve(ifelse(
               x < expected_mean_upper_ci - q,
                  dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_lower_ci),
            ifelse(
               x < expected_mean,
                  dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_upper_ci),
            ifelse(
               x < expected_mean_lower_ci + q,
                  dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_upper_ci),
            #else
                  dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_lower_ci)
            ))), lwd=1, lty=2, col=red, add=TRUE)
   else
      curve(ifelse(
               x < expected_mean,
                  dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_lower_ci),
            #else
                  dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_lower_ci)
            ), lwd=1, lty=2, col=red, add=TRUE)

   # Label the plot with statistical values
   if (use_latex != "true")
   {
      density_label = substitute(paste(rho == d, "%"),
                           list(d=signif(100*atoms/(x*y), digits=3)))
      alpha_label = substitute(paste(alpha == a),
                           list(a=alpha))
      test_label = substitute(paste(H[0], " ", result),
                           list(result=ifelse((data_mean > expected_mean_lower_ci) &
                                              (data_mean < expected_mean_upper_ci) &
                                              (data_var  > expected_var_lower_ci) &
                                              (data_var  < expected_var_upper_ci),
                                              "ACCEPTED", "REJECTED")))
      old_size = par("cex")
      par(cex=old_size*0.8) 
      corner.label(density_label, x=1, y=1, yoff=1.0*strheight("m"))
      corner.label(alpha_label,   x=1, y=1, yoff=2.5*strheight("m"))
      corner.label(test_label,    x=1, y=1, yoff=4.0*strheight("m"))
      par(cex=old_size)
   }

   # If individual LaTeX documents are to be created,
   # close the current TikZ graphics device
   if (use_latex == "true")
   {
      dev.off()
   }
}

######################
# KOLMOGOROV-SMIRNOV #
######################

for (i in 1:length(plot_type))
{
   # If individual LaTeX documents are to be created,
   # open a TikZ graphics device and set a few parameters
   if (use_latex == "true")
   {
      tikz(file=paste(path_to_plots, "_", plot_type[i], "_ks.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw the empirical CDF and expected CDF
   plot(ecdf(diffusion_data[,plot_type[i]]),
      main=plot_title[i], xlab="Displacement", ylab="Cumulative Density", lwd=1, verticals=TRUE, do.points=FALSE, col=plot_color[i])
   curve(pnorm(x, mean=expected_mean, sd=expected_sd), lwd=2, lty=2, col=red, add=TRUE)

   # Conduct the Kolmogorov-Smirnov goodness-of-fit test
   ks_results = suppressWarnings(ks.test(diffusion_data[,plot_type[i]], "pnorm", mean=expected_mean, sd=expected_sd))

   # Save statistics for exporting
   stats_data[paste(plot_type[i], "_D", sep="")] = ks_results$statistic
   stats_data[paste(plot_type[i], "_p", sep="")] = ks_results$p.value

   # Label the plot with statistical values
   if (use_latex != "true")
   {
      d_label = substitute(paste(D == dstatistic),
                       list(dstatistic=signif(ks_results$statistic, digits=3)))
      if (ks_results$p.value > 10^-6)
         p_label = substitute(paste(p == pvalue),
                         list(pvalue=signif(ks_results$p.value, digits=3)))
      else
         p_label = substitute(paste(p < 10^-6),
                         list())
      old_size = par("cex")
      par(cex=old_size*0.8)
      corner.label(d_label, x=1, y=-1, yoff=3.5*strheight("m"))
      corner.label(p_label, x=1, y=-1, yoff=1.5*strheight("m"))
      par(cex=old_size)
   }

   # If individual LaTeX documents are to be created,
   # close the current TikZ graphics device
   if (use_latex == "true")
   {
      dev.off()
   }
}

# Export statistical test results
write.table(stats_data, file=path_to_stats, row.names=FALSE, quote=FALSE)

# End

