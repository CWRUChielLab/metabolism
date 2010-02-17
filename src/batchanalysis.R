#!/usr/bin/Rscript
#
# Analyzes batch data in R and creates graphs
#
# Usage: ./batchanalysis.R pathtobatch nexperiments pathtopdf
#   pathtobatch   the path of the batch directory
#   nexperiments  the number of experiments in the batch
#   pathtopdf     the path of the PDF that the R script will create

# Import command line arguments
Args = commandArgs()
path_to_batch = as.character(Args[6])
n_experiments = as.integer(Args[7])
path_to_pdf = as.character(Args[8])

# Import experimental parameters and statistics
config = list()
stats = list()
for (i in 1:n_experiments)
{
   this_experiment = formatC(i, flag="0", width=nchar(n_experiments))
   this_config = paste(path_to_batch, "/", this_experiment, "/config.", this_experiment, ".out", sep="")
   this_stats  = paste(path_to_batch, "/", this_experiment, "/stats.",  this_experiment, ".out", sep="")
   config = rbind(config, read.table(this_config, header=TRUE))
   stats  = rbind(stats,  read.table(this_stats,  header=TRUE))
}

# Calculate densities and extract batch parameters
config[,"density"] = config[,"atoms"] / (config[,"x"] * config[,"y"])
x = config[1,"x"]
y = config[1,"y"]
world_size = x * y
alpha = stats[1,"alpha"]
expected_mean = stats[1,"expected_mean"]
expected_var  = stats[1,"expected_var"]
expected_sd   = stats[1,"expected_sd"]

# For corner.label
library(plotrix)

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
plot_title = c("Net Horizontal Displacement\nwith Collisions",
               "Net Horizontal Displacement\nwithout Collisions",
               "Net Vertical Displacement\nwith Collisions",
               "Net Vertical Displacement\nwithout Collisions")
plot_color = c(green,
               darkgreen,
               blue,
               darkblue)

# Open a PDF graphics device and set a few parameters
pdf(file=path_to_pdf, family="Palatino")
par(mfrow=c(2,2))
par(font.lab=2)

################
# PLOTS PAGE 1 #
################

for (i in 1:length(plot_type))
{
   # Draw scatter plot of sample means against density
   plot(config[,"density"], stats[,paste(plot_type[i], "_mean", sep="")],
      main=plot_title[i], xlab="Density", ylab="Sample Mean", col=plot_color[i])

   # Add lower bound on the acceptable sample mean as a function of density
   curve(expected_mean - qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(config[,"density"]), to=max(config[,"density"]), lwd=1, lty=5, col=red, add=TRUE)

   # Add upper bound on the acceptable sample mean as a function of density
   curve(expected_mean + qnorm(1-alpha/2) * expected_sd / sqrt(x*world_size),
      from=min(config[,"density"]), to=max(config[,"density"]), lwd=1, lty=5, col=red, add=TRUE)

   # Label the plot with statistical values
   alpha_label = substitute(paste(alpha == a),
                        list(a=alpha))
   old_size = par("cex")
   par(cex=old_size*0.8)
   corner.label(alpha_label, x=1, y=1, yoff=1.0*strheight("m"))
   par(cex=old_size)
}

################
# PLOTS PAGE 2 #
################

for (i in 1:length(plot_type))
{
   # Draw scatter plot of sample standard deviations against density
   plot(config[,"density"], stats[,paste(plot_type[i], "_sd", sep="")],
      main=plot_title[i], xlab="Density", ylab="Sample Standard Deviation", col=plot_color[i])

   # Add lower bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(alpha/2,   df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(config[,"density"]), to=max(config[,"density"]), lwd=1, lty=5, col=red, add=TRUE)

   # Add upper bound on the acceptable sample standard deviation as a function of density
   curve(sqrt(qchisq(1-alpha/2, df=x*world_size-1) * expected_var / (x*world_size-1)),
      from=min(config[,"density"]), to=max(config[,"density"]), lwd=1, lty=5, col=red, add=TRUE)

   # Label the plot with statistical values
   alpha_label = substitute(paste(alpha == a),
                        list(a=alpha))
   old_size = par("cex")
   par(cex=old_size*0.8)
   corner.label(alpha_label, x=1, y=1, yoff=1.0*strheight("m"))
   par(cex=old_size)
}

# End

