#!/usr/bin/Rscript
#
# Subroutine for plotting simulation data and numerical
# integration data; depends on _parse_config.R,
# _parse_census.R, _integrate.R, and the existence of
# output_type and path_to_plots; if output_type is any one
# of "pdf", "png", or "latex", appropriate output files will
# be created, otherwise none will be


# Check for config_parsed
if (!exists("config_parsed"))
{
   sink(stderr())
   print("PLOT DATA FAILED: _parse_config.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for census_parsed
if (!exists("census_parsed"))
{
   sink(stderr())
   print("PLOT DATA FAILED: _parse_census.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for integration_done
if (!exists("integration_done"))
{
   sink(stderr())
   print("PLOT DATA FAILED: _integrate.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for output_type
if (!exists("output_type"))
{
   sink(stderr())
   print("PLOT DATA FAILED: output_type is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for path_to_plots
if (!exists("path_to_plots"))
{
   sink(stderr())
   print("PLOT DATA FAILED: path_to_plots is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Define a function for plotting a single line as a series
# of line segments; handles transparency by decreasing the
# alpha value of each line segment incrementally; used for
# phase portraits
plot_as_segments = function(x, y, col="black", ...)
{
   n = length(x)
   plot(c(x[1], x[2]), c(y[1], y[2]),
      col=rgb(t(col2rgb(col))/255, alpha=1),    # decay from 1
      #col=rgb(t(col2rgb(col))/255, alpha=0.5),  # decay from 0.5
      ...)
   for (i in 2:(n-1))
   {
      points(c(x[i], x[i+1]), c(y[i], y[i+1]),
         #col=rgb(t(col2rgb(col))/255, alpha=(1-(i-1)/(n-1))),         # linear decay from 1 to 0
         #col=rgb(t(col2rgb(col))/255, alpha=(0.5-(i-1)/(2*n-2))),     # linear decay from 0.5 to 0
         col=rgb(t(col2rgb(col))/255, alpha=0.95*exp(-10/n*i)+0.05),  # exponential decay from 1 to 0.05
         #col=rgb(t(col2rgb(col))/255, alpha=0.5*exp(-20/n*i)),        # exponential decay from 0.5 to 0
         ...)
   }
}


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
}


#################################
# Observed Density Trajectories #
#################################

# If individual PNG graphics are to be created, open a PNG
# graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_observed.png", sep=""))
   par(font.lab=2)
}

# If individual LaTeX documents are to be created, open a
# TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_observed.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

# Determine the appropriate plot range
if (integration_done)
{
   ylim=c(0, max(ele_data/(x*y), expected_data[, names(ele_data)]/(x*y)))
} else {
   ylim=c(0, max(ele_data/(x*y)))
}

plot(iter_data, ele_data[[1]]/(x*y),
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Observed Density Trajectories",
   xlab="Time",
   ylab="Density",
   ylim=ylim,
   type="l")
for (i in 2:length(ele_data))
{
   points(iter_data, ele_data[[i]]/(x*y),
      col=ele_colors[[ names(ele_data)[i] ]],
      type="l")
}

legend("topright",
   legend=names(ele_data),
   fill=unlist(ele_colors[ names(ele_data) ]),
   bg="white")

# If individual LaTeX documents or PNG graphics are to be
# created, close the current graphics device
if (output_type == "png" || output_type == "latex")
{
   dev.off()
}


#################################
# Expected Density Trajectories #
#################################

if (integration_done)
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_expected.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      tikz(file=paste(path_to_plots, "_expected.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   plot(expected_data[, "time"], expected_data[, names(ele_data)[1]]/(x*y),
      col=ele_colors[[ names(ele_data)[1] ]],
      main="Expected Density Trajectories",
      xlab="Time",
      ylab="Density",
      ylim=c(0, max(ele_data/(x*y), expected_data[, names(ele_data)]/(x*y))),
      type="l")
   for (i in 2:length(ele_data))
   {
      points(expected_data[, "time"], expected_data[, names(ele_data)[i]]/(x*y),
         col=ele_colors[[ names(ele_data)[i] ]],
         type="l")
   }

   legend("topright",
      legend=names(ele_data),
      fill=unlist(ele_colors[ names(ele_data) ]),
      bg="white")

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


###################################
# Observed Density Phase Portrait #
###################################

if (length(ele_data) == 2)
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_obs_phase.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      tikz(file=paste(path_to_plots, "_obs_phase.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   # Determine the appropriate plot range
   if (integration_done)
   {
      xlim=c(0, max(ele_data[[1]]/(x*y), expected_data[, names(ele_data)[1]]/(x*y)))
      ylim=c(0, max(ele_data[[2]]/(x*y), expected_data[, names(ele_data)[2]]/(x*y)))
   } else {
      xlim=c(0, max(ele_data[[1]]/(x*y)))
      ylim=c(0, max(ele_data[[2]]/(x*y)))
   }

   plot_as_segments(ele_data[[1]]/(x*y), ele_data[[2]]/(x*y),
      col=ele_colors[[ "default" ]],
      main="Observed Density Phase Portrait",
      xlab=paste("Density of ", names(ele_data)[1], sep=""),
      ylab=paste("Density of ", names(ele_data)[2], sep=""),
      xlim=xlim,
      ylim=ylim,
      type="l")

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


###################################
# Expected Density Phase Portrait #
###################################

if (integration_done && length(ele_data) == 2)
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_exp_phase.png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      tikz(file=paste(path_to_plots, "_exp_phase.tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   plot_as_segments(expected_data[, names(ele_data)[1]]/(x*y), expected_data[, names(ele_data)[2]]/(x*y),
      col=ele_colors[[ "default" ]],
      main="Expected Density Phase Portrait",
      xlab=paste("Density of ", names(ele_data)[1], sep=""),
      ylab=paste("Density of ", names(ele_data)[2], sep=""),
      xlim=c(0, max(ele_data[[1]]/(x*y), expected_data[, names(ele_data)[1]]/(x*y))),
      ylim=c(0, max(ele_data[[2]]/(x*y), expected_data[, names(ele_data)[2]]/(x*y))),
      type="l")

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


###################################
# Individual Density Trajectories #
###################################

for (i in 1:length(ele_data))
{
   # If individual PNG graphics are to be created, open a PNG
   # graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", names(ele_data)[i], ".png", sep=""))
      par(font.lab=2)
   }

   # If individual LaTeX documents are to be created, open a
   # TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      tikz(file=paste(path_to_plots, "_", names(ele_data)[i], ".tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   # Determine the appropriate plot range
   if (integration_done)
   {
      ylim=c(0, max(ele_data[[i]]/(x*y), expected_data[, names(ele_data)[i]]/(x*y)))
   } else {
      ylim=c(0, max(ele_data[[i]]/(x*y)))
   }

   plot(iter_data, ele_data[[i]]/(x*y),
      col=ele_colors[[ names(ele_data)[i] ]],
      main=paste("Density Trajectory of ", names(ele_data)[i], sep=""),
      xlab="Time",
      ylab="Density",
      ylim=ylim,
      type="l")

   if (integration_done)
   {
      points(expected_data[, "time"], expected_data[, names(ele_data)[i]]/(x*y),
         col=ele_colors[[ names(ele_data)[i] ]],
         lty=2, type="l")

      legend("topright",
         legend=c("Observed", "Expected"),
         col=rep(ele_colors[[ names(ele_data)[i] ]], 2),
         lty=c(1,2),
         bg="white")
   }

   # If individual LaTeX documents or PNG graphics are to be
   # created, close the current graphics device
   if (output_type == "png" || output_type == "latex")
   {
      dev.off()
   }
}


##############################
# Overall Density Trajectory #
##############################

# If individual PNG graphics are to be created, open a PNG
# graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_total.png", sep=""))
   par(font.lab=2)
}

# If individual LaTeX documents are to be created, open a
# TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_total.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

# Determine the appropriate plot range
if (integration_done)
{ 
   ylim=c(0, max(total_data/(x*y), rowSums(expected_data[, names(ele_data)]/(x*y))))
} else {
   ylim=c(0, max(total_data/(x*y)))
}

plot(iter_data, total_data/(x*y),
   col=ele_colors[["default"]],
   main="Overall Density Trajectory",
   xlab="Time",
   ylab="Density",
   ylim=ylim,
   type="l")

if (integration_done)
{
   points(expected_data[, "time"], rowSums(expected_data[, names(ele_data)]/(x*y)),
      col=ele_colors[["default"]],
      lty=2, type="l")

   legend("topright",
      legend=c("Observed", "Expected"),
      col=rep(ele_colors[["default"]], 2),
      lty=c(1,2),
      bg="white")
}

# If individual LaTeX documents or PNG graphics are to be
# created, close the current graphics device
if (output_type == "png" || output_type == "latex")
{
   dev.off()
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

