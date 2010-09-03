#!/usr/bin/Rscript
#
# Subroutine for plotting simulated diffusion data; depends
# on _parse_config.R, _parse_diffusion.R,
# _analyze_diffusion.R, and the existence of path_to_plots


# Check for config_parsed
if (!exists("config_parsed"))
{
   sink(stderr())
   print("PLOT DIFFUSION FAILED: _parse_config.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for diffusion_parsed
if (!exists("diffusion_parsed"))
{
   sink(stderr())
   print("PLOT DIFFUSION FAILED: _parse_diffusion.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for diffusion_analyzed
if (!exists("diffusion_analyzed"))
{
   sink(stderr())
   print("PLOT DIFFUSION FAILED: _analyze_diffusion.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for path_to_plots
if (!exists("path_to_plots"))
{
   sink(stderr())
   print("PLOT DIFFUSION FAILED: path_to_plots is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


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


# Define the suggested number of bins for each histogram
bins = 30


# Load the plotrix package for corner.label
library(plotrix)


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
   par(mfrow=c(2,2))
}


###########################
# Quantile-Quantile Plots #
###########################

for (i in 1:length(diffusion_types))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", diffusion_types[i], "_qq.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      #tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_qq.tex", sep=""), width=3, height=2.7)
      tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_qq.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw a normal Q-Q plot
   qqnorm(diffusion_data[, diffusion_types[i]],
      col=plot_color[[i]],
      main=plot_title[[i]])

   # Add a straight line corresponding to the expected
   # distribution
   abline(a=expected_mean, b=expected_sd, col="red")

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


########################################
# Histograms with Confidence Intervals #
########################################

for (i in 1:length(diffusion_types))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", diffusion_types[i], "_hist.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      #tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_hist.tex", sep=""), width=3, height=2.7)
      tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_hist.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw histogram
   hist(diffusion_data[, diffusion_types[i]],
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Displacement",
      breaks=bins,
      freq=FALSE)

   # Recall the statistics for this data
   data_mean = stats_data[[paste(diffusion_types[i], "_mean", sep="")]]
   data_var  = stats_data[[paste(diffusion_types[i], "_var", sep="")]]
   data_sd   = stats_data[[paste(diffusion_types[i], "_sd", sep="")]]

   # Plot normal PDF using sample mean and sample standard
   # deviation
   curve(dnorm(x, mean=data_mean, sd=data_sd), lwd=1, add=TRUE)

   # Draw the upper bound of the graphical CI as a piecewise
   # function
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
         )))))), lwd=1, lty=2, col="red", add=TRUE)

   # Draw the lower bound of the graphical CI as a piecewise
   # function
   q = sqrt((expected_var_lower_ci*expected_var_upper_ci) / (expected_var_lower_ci-expected_var_upper_ci)
               * log(expected_var_lower_ci/expected_var_upper_ci))
   if (expected_mean_upper_ci - q < expected_mean)
   {
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
            ))), lwd=1, lty=2, col="red", add=TRUE)
   } else {
      curve(ifelse(
               x < expected_mean,
                  dnorm(x, mean=expected_mean_upper_ci, sd=expected_sd_lower_ci),
            #else
                  dnorm(x, mean=expected_mean_lower_ci, sd=expected_sd_lower_ci)
            ), lwd=1, lty=2, col="red", add=TRUE)
   }

   # Label the plot with statistical values
   if (output_type != "latex")
   {
      density_label = substitute(paste(rho == d, "%"),
                           list(d=signif(100*samples/(x*y), digits=3)))
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

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


#################################################
# Empirical CDF with Kolmogorov-Smirnov Results #
#################################################

for (i in 1:length(diffusion_types))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", diffusion_types[i], "_ks.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      #tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_ks.tex", sep=""), width=3, height=2.7)
      tikz(file=paste(path_to_plots, "_", diffusion_types[i], "_ks.tex", sep=""), width=3, height=2.7, bareBones=TRUE)
      par(cex=0.7)
      par(font.main=1)
   }

   # Draw the empirical CDF and expected CDF
   plot(ecdf(diffusion_data[,diffusion_types[i]]),
      col=plot_color[[i]],
      main=plot_title[[i]],
      xlab="Displacement",
      ylab="Cumulative Density",
      lwd=1,
      verticals=TRUE,
      do.points=FALSE)
   curve(pnorm(x, mean=expected_mean, sd=expected_sd),
      col="red",
      lwd=2,
      lty=2,
      add=TRUE)

   # Recall the statistics for this data
   ks_results$statistic = stats_data[[paste(diffusion_types[i], "_D", sep="")]]
   ks_results$p.value   = stats_data[[paste(diffusion_types[i], "_p", sep="")]]

   # Label the plot with statistical values
   if (output_type != "latex")
   {
      n_label = substitute(paste(n == nsamples),
                       list(nsamples=samples))
      d_label = substitute(paste(D == dstatistic),
                       list(dstatistic=signif(ks_results$statistic, digits=3)))
      if (ks_results$p.value > 10^-6)
      {
         p_label = substitute(paste(p == pvalue),
                         list(pvalue=signif(ks_results$p.value, digits=3)))
      } else {
         p_label = substitute(paste(p < 10^-6),
                         list())
      }
      old_size = par("cex")
      par(cex=old_size*0.8)
      corner.label(n_label, x=1, y=-1, yoff=5.5*strheight("m"))
      corner.label(d_label, x=1, y=-1, yoff=3.5*strheight("m"))
      corner.label(p_label, x=1, y=-1, yoff=1.5*strheight("m"))
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

