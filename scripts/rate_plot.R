#!/usr/bin/Rscript
#
# Creates plots in R of the observed and expected reaction trajectories
#
# Usage: ./rate_plot.R output_type path_to_config path_to_census path_to_plots
#   output_type     the type of output that the R script should create; valid options
#                      are "pdf", "png", and "latex"
#   path_to_config  the path of the config.out file to be read
#   path_to_census  the path of the census.out file to be read
#   path_to_plots   the path of the output that the R script will create without
#                      the file extension

# Import command line arguments
Args = commandArgs()
if (length(Args) > 9)
{
   sink(stderr())
   print("ANALYSIS FAILED: Too many parameters!")
   print("  Usage: ./rate_plot.R output_type path_to_config path_to_census path_to_plots")
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

# Read in the list of Elements and store their names and
# colors
config_ele = strsplit(config[grepl("ele", config)], " ")
ele_names = list()
ele_colors = list()
for (i in 1:length(config_ele))
{
   ele_names[[i]] = config_ele[[i]][[2]]
   if (!is.null( colors[[ config_ele[[i]][[4]] ]] ))
      ele_colors[[ ele_names[[i]] ]] = colors[[ config_ele[[i]][[4]] ]]
   else
      ele_colors[[ ele_names[[i]] ]] = config_ele[[i]][[4]]
}
ele_colors[[ "default" ]] = colors[[ "black" ]]

# Read in the list of Reactions, separate the reactants from
# the products, and calculate the rate constants
config_rxn = strsplit(config[grepl("rxn", config)], " ")
reactants = list()
products = list()
rate_constants = vector()
for (i in 1:length(config_rxn))
{
   # Grab the probability of reaction (the second word in
   # the line)
   prob = as.numeric(config_rxn[[i]][[2]])

   # Build the list of reactants for this Reaction by
   # reading in words until a reaction arrow is found;
   # '*'s are ignored, and so zeroth-order reactions
   # will have zero reactants listed
   j = 3
   reactants[[i]] = list()
   while (config_rxn[[i]][[j]] != "->")
   {
      if (config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*")
      {
         if (config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2")
         {
            for (n in 1:as.integer(config_rxn[[i]][[j]]))
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

   # Built the list of products for this Reaction by reading
   # in all remaining words; '*'s are ignored again, and so
   # annihilation reactions will have zero products listed
   j = j + 1
   products[[i]] = list()
   while (j <= length(config_rxn[[i]]))
   {
      if (config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*")
      {
         if (config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2")
         {
            for (n in 1:as.integer(config_rxn[[i]][[j]]))
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

   # Calculate the rate constant for this Reaction using the
   # probability of reaction; the calculation depends on the
   # numbers and type of reactants and products as well; see
   # table 1 of "Zeroth-, First-, and Second-Order Chemical
   # Reactions and Michaelis-Menten Enzyme Kinetics in an
   # Artificial Chemistry" (Gill, 2010)
   r = length(reactants[[i]])
   p = length(products[[i]])
   if (r < 2 && p < 2)
   {
      rate_constants[i] = 1/10 * prob
   } else {
      rate_constants[i] = 4/5 * prob
      if ((r == 0 && p == 2) || (r == 2 && reactants[[i]][[1]] == reactants[[i]][[2]]))
      {
         rate_constants[i] = rate_constants[i] / 2
      }
   }
}

# Determine the rate laws for each Element and store them
# as strings containing R commands that can be evaluated
# dynamically when needed if values for a state vector
# (particle concentrations at any given time) are provided
rate_laws = list()
for (i in 1:length(ele_names))
{
   rate_laws[[ ele_names[[i]] ]] = "0"

   # Walk through the list of reactions
   for (j in 1:length(config_rxn))
   {
      if (length(reactants[[j]]) > 0)
      {
         # Walk through the list of reactants for this reaction
         for (k in 1:length(reactants[[j]]))
         {
            if (reactants[[j]][[k]] == ele_names[[i]])
            {
               # If a reactant matches this Element (the Element is
               # consumed in the reaction), subtract an appropriate term
               # representing this reaction to the rate law string; the
               # vector "state" will contain the concentration value of
               # each Element
               rate_laws[[ ele_names[[i]] ]] = paste(rate_laws[[ ele_names[[i]] ]], " - rate_constants[", j, "]", sep="")
               for (l in 1:length(reactants[[j]]))
               {
                  rate_laws[[ ele_names[[i]] ]] = paste(rate_laws[[ ele_names[[i]] ]], "*state[\"", reactants[[j]][[l]], "\"]", sep="")
               }
            }
         }
      }

      if (length(products[[j]]) > 0)
      {
         # Walk through the list of products for this reaction
         for (k in 1:length(products[[j]]))
         {
            if (products[[j]][[k]] == ele_names[[i]])
            {
               # If a product matches this Element (the Element is created
               # in the reaction), add an appropriate term representing
               # this reaction to the rate law string; the vector "state"
               # will contain the concentration value of each Element
               rate_laws[[ ele_names[[i]] ]] = paste(rate_laws[[ ele_names[[i]] ]], " + rate_constants[", j, "]", sep="")
               if (length(reactants[[j]]) > 0)
               {
                  for (l in 1:length(reactants[[j]]))
                  {
                     rate_laws[[ ele_names[[i]] ]] = paste(rate_laws[[ ele_names[[i]] ]], "*state[\"", reactants[[j]][[l]], "\"]", sep="")
                  }
               }
            }
         }
      }
   }
}

# Import particle count data
rate_data = read.table(path_to_census, header=TRUE, check.names=FALSE)
iter_data = rate_data[["iter"]]
ele_data = rate_data[names(rate_data) != "iter" & names(rate_data) != "total"] / (x*y)
total_data = rate_data[["total"]] / (x*y)

# Define a function that returns the value of the rate laws
# when passed a state vector and perform a numerical
# integration on each rate law to find the expected
# trajectories of each Element
library(deSolve)
init = unlist(ele_data[1,])
rates = function(t, state, params)
         {
            derivatives = vector()
            for( i in 1:length(names(state)) )
            {
               derivatives[[ names(state)[i] ]] = as.numeric(eval(parse(text=rate_laws[[ names(state)[i] ]])))
            }
            return(list(derivatives))
         }
expected_data = ode(init, seq(0, iters, length.out=10000), rates)

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

plot(iter_data, ele_data[[1]],
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Observed Density Trajectories",
   xlab="Iterations",
   ylab="Density",
   ylim=c(0, max(ele_data, expected_data[, names(ele_data)])),
   type="l")
for (i in 2:length(ele_data))
{
   points(iter_data, ele_data[[i]],
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

plot(expected_data[, "time"], expected_data[, names(ele_data)[1]],
   col=ele_colors[[ names(ele_data)[1] ]],
   main="Expected Density Trajectories",
   xlab="Iterations",
   ylab="Density",
   ylim=c(0, max(ele_data, expected_data[, names(ele_data)])),
   type="l")
for (i in 2:length(ele_data))
{
   points(expected_data[, "time"], expected_data[, names(ele_data)[i]],
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

   plot(ele_data[[1]], ele_data[[2]],
      col=ele_colors[[ "default" ]],
      main="Observed Density Phase Portrait",
      xlab=paste("Density of ", names(ele_data)[1], sep=""),
      ylab=paste("Density of ", names(ele_data)[2], sep=""),
      xlim=c(0, max(ele_data[[1]], expected_data[, names(ele_data)[1]])),
      ylim=c(0, max(ele_data[[2]], expected_data[, names(ele_data)[2]])),
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

if (length(ele_data) == 2)
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

   plot(expected_data[, names(ele_data)[1]], expected_data[, names(ele_data)[2]],
      col=ele_colors[[ "default" ]],
      main="Expected Density Phase Portrait",
      xlab=paste("Density of ", names(ele_data)[1], sep=""),
      ylab=paste("Density of ", names(ele_data)[2], sep=""),
      xlim=c(0, max(ele_data[[1]], expected_data[, names(ele_data)[1]])),
      ylim=c(0, max(ele_data[[2]], expected_data[, names(ele_data)[2]])),
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

   plot(iter_data, ele_data[[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      main=paste("Density Trajectory of ", names(ele_data)[i], sep=""),
      xlab="Iterations",
      ylab="Density",
      ylim=c(0, max(ele_data[[i]], expected_data[, names(ele_data)[i]])),
      type="l")

   points(expected_data[, "time"], expected_data[, names(ele_data)[i]],
      col=ele_colors[[ names(ele_data)[i] ]],
      lty=2, type="l")

   legend("topright",
      legend=c("Observed", "Expected"),
      col=rep(ele_colors[[ names(ele_data)[i] ]], 2),
      lty=c(1,2),
      bg="white")

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

plot(iter_data, total_data,
   col=ele_colors[["default"]],
   main="Overall Density Trajectory",
   xlab="Iterations",
   ylab="Density",
   ylim=c(0, max(total_data, rowSums(expected_data[, names(ele_data)]))),
   type="l")

points(expected_data[, "time"], rowSums(expected_data[, names(ele_data)]),
   col=ele_colors[["default"]],
   lty=2, type="l")

legend("topright",
   legend=c("Observed", "Expected"),
   col=rep(ele_colors[["default"]], 2),
   lty=c(1,2),
   bg="white")

# If individual LaTeX documents or PNG graphics are to be
# created, close the current graphics device
if (output_type == "png" || output_type == "latex")
{
   dev.off()
}

# End

