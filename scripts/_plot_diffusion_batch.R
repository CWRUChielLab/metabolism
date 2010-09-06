#!/usr/bin/Rscript
#
# Subroutine for plotting statistical data from a batch of
# simulated diffusion data; depends on _parse_batch_stats.R
# and the existence of path_to_plots


# Check for batch_stats_parsed
if (!exists("batch_stats_parsed"))
{
   sink(stderr())
   print("PLOT DIFFUSION BATCH FAILED: _parse_batch_stats.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for path_to_plots
if (!exists("path_to_plots"))
{
   sink(stderr())
   print("PLOT DIFFUSION BATCH FAILED: path_to_plots is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Define plot attributes
source("../scripts/_prep_diffusion_plot.R")


# Load the plotrix package for corner.label
library(plotrix)


# If individual LaTeX documents are to be created, load the
# TikZ package
if (output_type == "latex" )
{
   library(tikzDevice)
}


# If all plots are to be drawn in a single PDF, open a PDF
# graphics device and set a few parameters
if (output_type == "pdf")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(font.lab=2)
   par(mfrow=c(2,2))
}


###############
# SAMPLE MEAN #
###############

for (i in 1:length(diffusion_types))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", diffusion_types[i], "_mean.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      #tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_mean.tex", sep=""), width=3, height=2.7)
      tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_mean.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
      par(cex=0.7)
      par(font.main=1)
   } 

   # Draw scatter plot of sample means against density
   plot(batch_stats[["density"]], batch_stats[[paste(diffusion_types[i], "_mean", sep="")]],
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Density",
      ylab="Sample Mean")

   # Add lower bound on the acceptable sample mean as a function of density
   curve(expected_mean - qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(batch_stats[["density"]]),
      to=max(batch_stats[["density"]]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Add upper bound on the acceptable sample mean as a function of density
   curve(expected_mean + qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(batch_stats[["density"]]),
      to=max(batch_stats[["density"]]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Label the plot with statistical values
   if (output_type != "latex")
   {
      alpha_label = substitute(paste(alpha == a),
                           list(a=alpha))
      old_size = par("cex")
      par(cex=old_size*0.8)
      corner.label(alpha_label, x=1, y=1, yoff=1.0*strheight("m"))
      par(cex=old_size)
   }

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


#############################
# SAMPLE STANDARD DEVIATION #
#############################

for (i in 1:length(diffusion_types))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", diffusion_types[i], "_sd.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      #tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_sd.tex", sep=""), width=3, height=2.7)
      tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_sd.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw scatter plot of sample standard deviations against density
   plot(batch_stats[["density"]], batch_stats[[paste(diffusion_types[i], "_sd", sep="")]],
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Density",
      ylab="Sample Standard Deviation")

   # Add lower bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(alpha/2, df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(batch_stats[["density"]]),
      to=max(batch_stats[["density"]]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Add upper bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(1-alpha/2, df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(batch_stats[["density"]]),
      to=max(batch_stats[["density"]]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Label the plot with statistical values
   if (output_type != "latex")
   {
      alpha_label = substitute(paste(alpha == a),
                           list(a=alpha))
      old_size = par("cex")
      par(cex=old_size*0.8)
      corner.label(alpha_label, x=1, y=1, yoff=1.0*strheight("m"))
      par(cex=old_size)
   }

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
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

