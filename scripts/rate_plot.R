#!/usr/bin/Rscript
#
# Creates plots in R of the observed and expected reaction trajectories
#
# Usage: ./rate_plot.R output_type path_to_config path_to_census path_to_plots
#   output_type     the type of output that the R script should create; valid options
#                      are "pdf", "png", and "latex"
#   do_integration  whether or not to run the numerical integration to find the
#                      expected trajectories; valid options are "true" and "false"
#   path_to_config  the path of the config.out file to be read
#   path_to_census  the path of the census.out file to be read
#   path_to_plots   the path of the output that the R script will create without
#                      the file extension

# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./rate_plot.R output_type do_integration path_to_config path_to_census path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Set parameters to default values if they were not
# specified on the command line
if (!is.na(Args[6]))
{
   output_type = as.character(Args[6])
} else {
   output_type = "pdf"
}

if (!is.na(Args[7]))
{
   do_integration = as.character(Args[7])
} else {
   do_integration = "true"
}

if (!is.na(Args[8]))
{
   path_to_config = as.character(Args[8])
} else {
   path_to_config = "config.out"
}

if (!is.na(Args[9]))
{
   path_to_census = as.character(Args[9])
} else {
   path_to_census = "census.out"
}

if (!is.na(Args[10]))
{
   path_to_plots = as.character(Args[10])
} else {
   path_to_plots = "plots"
}

# Import experimental parameters, parse Elements and
# Reactions, calculate reaction hazards, and build a
# stochastic petri net
source("../scripts/_parse_config.R")

# Import particle count data
rate_data = read.table(path_to_census, header=TRUE, check.names=FALSE)
iter_data = rate_data[["iter"]]
ele_data = rate_data[names(rate_data) != "iter" & names(rate_data) != "total"]
ele_data = ele_data[, unlist(ele_names)]
total_data = rate_data[["total"]]

# Define a function that returns the value of the rate laws
# when passed a state vector and perform a numerical
# integration on each rate law to find the expected
# trajectories of each Element
if (do_integration == "true")
{
   library(deSolve)
   initial_values = unlist(ele_data[1,])
   rates = function(t, state, params)
            {
               derivatives = as.vector(t(N$Post-N$Pre) %*% N$h(state, params))
               return(list(derivatives))
            }
   expected_data = ode(initial_values, seq(0, iters, length.out=10000), rates, constants)
} else {
   expected_data = ele_data
}

# Function for plotting a single line as a series of line
# segments; handles transparency by decreasing the alpha
# value of each line segment incrementally; used for phase
# portraits
plotAsSegments = function(x, y, col="black", ...)
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

plot(iter_data, ele_data[[1]]/(x*y),
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Observed Density Trajectories",
   xlab="Time",
   ylab="Density",
   ylim=c(0, max(ele_data/(x*y), expected_data[, names(ele_data)]/(x*y))),
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

if (do_integration == "true")
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

   plotAsSegments(ele_data[[1]]/(x*y), ele_data[[2]]/(x*y),
      col=ele_colors[[ "default" ]],
      main="Observed Density Phase Portrait",
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
# Expected Density Phase Portrait #
###################################

if (do_integration == "true" && length(ele_data) == 2)
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

   plotAsSegments(expected_data[, names(ele_data)[1]]/(x*y), expected_data[, names(ele_data)[2]]/(x*y),
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

   plot(iter_data, ele_data[[i]]/(x*y),
      col=ele_colors[[ names(ele_data)[i] ]],
      main=paste("Density Trajectory of ", names(ele_data)[i], sep=""),
      xlab="Time",
      ylab="Density",
      ylim=c(0, max(ele_data[[i]]/(x*y), expected_data[, names(ele_data)[i]]/(x*y))),
      type="l")

   if (do_integration == "true")
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

plot(iter_data, total_data/(x*y),
   col=ele_colors[["default"]],
   main="Overall Density Trajectory",
   xlab="Time",
   ylab="Density",
   ylim=c(0, max(total_data/(x*y), rowSums(expected_data[, names(ele_data)]/(x*y)))),
   type="l")

if (do_integration == "true")
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

