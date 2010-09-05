#!/usr/bin/Rscript
#
# Analyzes batch diffusion data in R and creates graphs
#
# Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots
#   path_to_batch  the path of the batch directory
#   n_simulations  the number of simulations in the batch
#   output_type    the type of output that the R script should create; valid
#                     options are "pdf", "png", and "latex"
#   path_to_plots  the path of the output that the R script will create (without
#                     the file extension)


# Import command line arguments
Args = commandArgs()
if (length(Args) < 7)
{
   sink(stderr())
   print("DIFFUSION BATCH ANALYSIS FAILED: Missing required parameters path_to_batch and/or n_simulations!")
   print("  Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}
if (length(Args) > 9)
{
   sink(stderr())
   print("DIFFUSION BATCH ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./diffusion_batch_analysis.R path_to_batch n_simulations output_type path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Set parameters to default values if they were not
# specified on the command line
path_to_batch = as.character(Args[6])

n_simulations = as.integer(Args[7])

if (!is.na(Args[8]))
{
   output_type = as.character(Args[8])
} else {
   output_type = "pdf"
}

if (!is.na(Args[9]))
{
   path_to_plots = as.character(Args[9])
} else {
   path_to_plots = paste(path_to_batch, "/plots", sep="")
}


# Import simulation parameters and statistics
batch_config = list()
batch_stats = list()
batch_name = tail(unlist(strsplit(path_to_batch, "/")), n=1)
for (i in 1:n_simulations)
{
   simulation_name = formatC(i, flag="0", width=nchar(n_simulations))
   path_to_stats = paste(path_to_batch, "/", simulation_name, "/stats.",  batch_name, ".", simulation_name, ".out", sep="")

   f = file(); temp = read.table(path_to_stats, colClasses=c("character", "character"))
   write(as.matrix(temp), ncolumns=length(temp[, 1]), file=f)
   batch_stats = rbind(batch_stats, read.table(f, header=TRUE))
   close(f)
}


# Extract batch parameters and expected values
world_size = batch_stats[1, "x"] * batch_stats[1, "y"]
alpha = batch_stats[1,"alpha"]
expected_mean = batch_stats[1,"expected_mean"]
expected_var  = batch_stats[1,"expected_var"]
expected_sd   = batch_stats[1,"expected_sd"]


# List the names of the diffusion data types
diffusion_types = c("dx_actual", "dx_ideal", "dy_actual", "dy_ideal")


# Define the plot titles
plot_title = list()
if (output_type == "latex")
{
   plot_title[which(diffusion_types == "dx_actual")] = "Displacement in the\n$x$-Dimension with Collisions"
   plot_title[which(diffusion_types == "dx_ideal")]  = "Displacement in the\n$x$-Dimension without Collisions"
   plot_title[which(diffusion_types == "dy_actual")] = "Displacement in the\n$y$-Dimension with Collisions"
   plot_title[which(diffusion_types == "dy_ideal")]  = "Displacement in the\n$y$-Dimension without Collisions"
} else {
   plot_title[which(diffusion_types == "dx_actual")] = "Displacement in the\nx-Dimension with Collisions"
   plot_title[which(diffusion_types == "dx_ideal")]  = "Displacement in the\nx-Dimension without Collisions"
   plot_title[which(diffusion_types == "dy_actual")] = "Displacement in the\ny-Dimension with Collisions"
   plot_title[which(diffusion_types == "dy_ideal")]  = "Displacement in the\ny-Dimension without Collisions"
}


# Define the plot color schemes
plot_color = list()
plot_color[which(diffusion_types == "dx_actual")] = "#249800"  # green
plot_color[which(diffusion_types == "dx_ideal")]  = "#237A09"  # darkgreen
plot_color[which(diffusion_types == "dy_actual")] = "#0020AB"  # blue
plot_color[which(diffusion_types == "dy_ideal")]  = "#001055"  # darkblue


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
   plot(batch_stats[, "density"], batch_stats[, paste(diffusion_types[i], "_mean", sep="")],
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Density",
      ylab="Sample Mean")

   # Add lower bound on the acceptable sample mean as a function of density
   curve(expected_mean - qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(batch_stats[, "density"]),
      to=max(batch_stats[, "density"]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Add upper bound on the acceptable sample mean as a function of density
   curve(expected_mean + qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(batch_stats[, "density"]),
      to=max(batch_stats[,"density"]),
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
   plot(batch_stats[, "density"], batch_stats[, paste(diffusion_types[i], "_sd", sep="")],
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Density",
      ylab="Sample Standard Deviation")

   # Add lower bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(alpha/2, df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(batch_stats[, "density"]),
      to=max(batch_stats[, "density"]),
      col="red",
      lwd=1,
      lty=5,
      add=TRUE)

   # Add upper bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(1-alpha/2, df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(batch_stats[, "density"]),
      to=max(batch_stats[, "density"]),
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

