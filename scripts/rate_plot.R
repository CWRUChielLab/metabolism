#!/usr/bin/Rscript
#
# Analyzes reaction rate data in R and creates graphs
#
# Usage: ./rate_plot.R outputtype pathtoconfig pathtocensus pathtoplots pathtostats
#   outputtype    the type of output that the R script should create; valid options
#                    are "pdf", "png", and "latex"
#   pathtoconfig  the path of the config.out file
#   pathtocensus  the path of the census.out file
#   pathtoplots   the path of the output that the R script will create without
#                    the file extension
#   pathtostats   the path of the text file that the R script will create

# Import command line arguments
Args = commandArgs()
if (length(Args) > 10)
{
   sink(stderr())
   print("ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./rate_plot.R outputtype pathtoconfig pathtocensus pathtoplots pathtostats")
   sink()
   q(save="no", status=1, runLast=FALSE)
}
if (!is.na(Args[6]))
{
   output_type = as.character(Args[6])
} else {
   output_type = "pdf"
}

if (!is.na(Args[7]))
{
   path_to_config = as.character(Args[7])
} else {
   path_to_config = "config.out"
}

if (!is.na(Args[8]))
{
   path_to_census = as.character(Args[8])
} else {
   path_to_census = "census.out"
}

if (!is.na(Args[9]))
{
   path_to_plots = as.character(Args[9])
} else {
   path_to_plots = "plots"
}

if (!is.na(Args[10]))
{
   path_to_stats = as.character(Args[10])
} else {
   path_to_stats = "stats.out"
}

# Colors
colors = list()
colors["teal"] = "#008080"
colors["darkorange"] = "#FF8C00"
colors["yellow"] = "#FFFF00"
colors["hotpink"] = "#FF69B4"
colors["red"] = "#FF0000"
colors["green"] = "#008000"
colors["blue"] = "#0000FF"
colors["black"] = "#000000"

# Import experimental parameters
config = readLines(path_to_config)
iters = as.integer(unlist(strsplit(config[grepl("iters", config)], " "))[2])
x = as.integer(unlist(strsplit(config[grepl("x", config)], " "))[2])
y = as.integer(unlist(strsplit(config[grepl("y", config)], " "))[2])

# Parse Elements
config_ele = strsplit(config[grepl("ele", config)], " ")
ele_names = list()
ele_colors = list()
for( i in 1:length(config_ele) )
{
   ele_names[[i]] = config_ele[[i]][[2]]
   if( !is.null( colors[[ config_ele[[i]][[4]] ]] ) )
      ele_colors[[ ele_names[[i]] ]] = colors[[ config_ele[[i]][[4]] ]]
   else
      ele_colors[[ ele_names[[i]] ]] = config_ele[[i]][[4]]
}
ele_colors[[ "total" ]] = colors[[ "black" ]]

# Parse Reactions
config_rxn = strsplit(config[grepl("rxn", config)], " ")
reactants = list()
products = list()
rateconstants = vector()
for( i in 1:length(config_rxn) )
{
   prob = as.numeric(config_rxn[[i]][[2]])

   # Build the list of reactants
   j = 3
   reactants[[i]] = list()
   while( config_rxn[[i]][[j]] != "->" )
   {
      if( config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*")
      {
         if( config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2" )
         {
            for( n in 1:as.integer(config_rxn[[i]][[j]]) )
            {
               reactants[[i]] = c(reactants[[i]], config_rxn[[i]][[j+1]])
            }
            j = j + 2
         } else {
            reactants[[i]] = c(reactants[[i]], config_rxn[[i]][[j]])
            j = j + 1
         }
      } else {
         j = j + 1
      }
   }

   # Built the list of products
   j = j + 1
   products[[i]] = list()
   while( j <= length(config_rxn[[i]]) )
   {
      if( config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*" )
      {
         if( config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2" )
         {
            for( n in 1:as.integer(config_rxn[[i]][[j]]) )
            {
               products[[i]] = c(products[[i]], config_rxn[[i]][[j+1]])
            }
            j = j + 2
         } else {
            products[[i]] = c(products[[i]], config_rxn[[i]][[j]])
            j = j + 1
         }
      } else {
         j = j + 1
      }
   }

   # Calculate rate constant
   r = length(reactants[[i]])
   p = length(products[[i]])
   if( r < 2 && p < 2 )
   {
      rateconstants[i] = 1/10 * prob
   } else {
      rateconstants[i] = 4/5 * prob
      if( (r == 0 && p == 2) || (r == 2 && reactants[[i]][[1]] == reactants[[i]][[2]]) )
      {
         rateconstants[i] = rateconstants[i] / 2
      }
   }
}

# Build rate laws
ratelaws = list()
for( i in 1:length(ele_names))
{
   ratelaws[[ ele_names[[i]] ]] = "0"
   for( j in 1:length(config_rxn) )
   {
      if( length(reactants[[j]]) > 0 )
      {
         for( k in 1:length(reactants[[j]]) )
         {
            if( reactants[[j]][[k]] == ele_names[[i]] )
            {
               # Deduct from rate
               ratelaws[[ ele_names[[i]] ]] = paste(ratelaws[[ ele_names[[i]] ]], "-rateconstants[", j, "]", sep="")
               for( l in 1:length(reactants[[j]]) )
               {
                  ratelaws[[ ele_names[[i]] ]] = paste(ratelaws[[ ele_names[[i]] ]], "*state[\"", reactants[[j]][[l]], "\"]", sep="")
               }
            }
         }
      }

      if( length(products[[j]]) > 0 )
      {
         for( k in 1:length(products[[j]]) )
         {
            if( products[[j]][[k]] == ele_names[[i]] )
            {
               # Add to rate
               ratelaws[[ ele_names[[i]] ]] = paste(ratelaws[[ ele_names[[i]] ]], "+rateconstants[", j, "]", sep="")
               if( length(reactants[[j]]) > 0 )
               {
                  for( l in 1:length(reactants[[j]]) )
                  {
                     ratelaws[[ ele_names[[i]] ]] = paste(ratelaws[[ ele_names[[i]] ]], "*state[\"", reactants[[j]][[l]], "\"]", sep="")
                  }
               }
            }
         }
      }
   }
}

# Import rate data
rate_data = read.table(path_to_census, header=TRUE, check.names=FALSE)
iter_data = rate_data[["iter"]]
ele_data = rate_data[names(rate_data) != "iter" & names(rate_data) != "total"] / (x*y)
total_data = rate_data[["total"]] / (x*y)

# Numerical integration
library(deSolve)
init = unlist(ele_data[1,])
rates = function(t, state, params)
         {
            derivatives = vector()
            for( i in 1:length(names(state)) )
            {
               derivatives[[ names(state)[i] ]] = as.numeric(eval(parse(text=ratelaws[[ names(state)[i] ]])))
            }
            return(list(derivatives))
         }
quadrature = ode(init, seq(0, iters, length.out=10000), rates)

# For corner.label
library(plotrix)

# If individual LaTeX documents are to be created,
# load the TikZ package
if (output_type == "latex")
{
   library(tikzDevice)
}

# If all plots are to be drawn in a single PDF,
# open a PDF graphics device and set a few parameters
if (output_type == "pdf")
{
   pdf(file=paste(path_to_plots, ".pdf", sep=""), family="Palatino")
   par(font.lab=2)
}

##################
# Plot all atoms #
##################

# If individual PNG graphics are to be created,
# open a PNG graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_all.png", sep=""))
   par(font.lab=2)
} 

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_all.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

plot(iter_data, ele_data[[1]],
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Observed Chemical Density Trajectories",
   xlab="Iterations", ylab="Density",
   ylim=c(0, max(ele_data)),
   type="l")
for( i in 2:length(ele_data) )
{
   points(iter_data, ele_data[[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      type="l")
}

legend("topright",
   legend=names(ele_data),
   fill=unlist(ele_colors[ names(ele_data) ]),
   bg="white")

# If individual LaTeX documents or PNG graphics are
# to be created, close the current graphics device
if (output_type != "pdf")
{
   dev.off()
}

##############################
# Plot expected trajectories #
##############################

# If individual PNG graphics are to be created,
# open a PNG graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_expected.png", sep=""))
   par(font.lab=2)
} 

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_expected.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

plot(quadrature[, "time"], quadrature[, names(ele_data)[1]],
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Expected Chemical Density Trajectories",
   xlab="Iterations", ylab="Density",
   ylim=c(0, max(ele_data)),
   type="l")
for( i in 2:length(ele_data) )
{
   points(quadrature[, "time"], quadrature[, names(ele_data)[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      type="l")
}

legend("topright",
   legend=names(ele_data),
   fill=unlist(ele_colors[ names(ele_data) ]),
   bg="white")

# If individual LaTeX documents or PNG graphics are
# to be created, close the current graphics device
if (output_type != "pdf")
{
   dev.off()
}

#########################
# Plot individual atoms #
#########################

for( i in 1:length(ele_data) )
{
   # If individual PNG graphics are to be created,
   # open a PNG graphics device and set a few parameters
   if (output_type == "png")
   {
      png(file=paste(path_to_plots, "_", names(ele_data)[i], ".png", sep=""))
      par(font.lab=2)
   } 

   # If individual LaTeX documents are to be created, 
   # open a TikZ graphics device and set a few parameters
   if (output_type == "latex")
   {
      tikz(file=paste(path_to_plots, "_", names(ele_data)[i], ".tex", sep=""), width=3, height=2.7)
      par(cex=0.7)
      par(font.main=1)
   }

   plot(iter_data, ele_data[[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      main=paste("Density Trajectory of ", names(ele_data)[i], sep=""),
      xlab="Iterations", ylab="Density",
      ylim=c(0, max(ele_data[[i]], quadrature[, names(ele_data)[i]])),
      type="l")

   points(quadrature[, "time"], quadrature[, names(ele_data)[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      lty=2, type="l")

   legend("topright",
      legend=c("Observed", "Expected"),
      col=rep(ele_colors[[ names(ele_data)[i] ]], 2),
      lty=c(1,2),
      bg="white")

   # If individual LaTeX documents or PNG graphics are
   # to be created, close the current graphics device
   if (output_type != "pdf")
   {
      dev.off()
   }
}

####################
# Plot total atoms #
####################

# If individual PNG graphics are to be created,
# open a PNG graphics device and set a few parameters
if (output_type == "png")
{
   png(file=paste(path_to_plots, "_total.png", sep=""))
   par(font.lab=2)
} 

# If individual LaTeX documents are to be created, 
# open a TikZ graphics device and set a few parameters
if (output_type == "latex")
{
   tikz(file=paste(path_to_plots, "_total.tex", sep=""), width=3, height=2.7)
   par(cex=0.7)
   par(font.main=1)
}

plot(iter_data, total_data,
   col=ele_colors[["total"]],
   main="Overall Density Trajectory",
   xlab="Iterations", ylab="Density",
   ylim=c(0, max(total_data)),
   type="l")

points(quadrature[, "time"], rowSums(quadrature[, colnames(quadrature) != "time"]),
   col=ele_colors[["total"]],
   lty=2, type="l")

legend("topright",
   legend=c("Observed", "Expected"),
   col=rep(ele_colors[["total"]], 2),
   lty=c(1,2),
   bg="white")

# If individual LaTeX documents or PNG graphics are
# to be created, close the current graphics device
if (output_type != "pdf")
{
   dev.off()
}

# End

