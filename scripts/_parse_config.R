#!/usr/bin/Rscript
#
# Subroutine for importing the contents of a config file and
# building a petri net for the reaction system; depends on
# the existence of path_to_config


# Check for path_to_config
if (!exists("path_to_config"))
{
   sink(stderr())
   print("PARSE CONFIG FAILED: path_to_config is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Constants used for determining rate constants and initial
# system state (should match the values defined in
# src/sim.h)
MAX_RXNS_PER_SET_OF_REACTANTS = 2
MAX_ELES_NOT_INCLUDING_SOLVENT = 8


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
config = strsplit(config[config != ""], " ")
keywords = sapply(config, head, n=1)
iters = as.integer(unlist(config[keywords == "iters"])[2])
x = as.integer(unlist(config[keywords == "x"])[2])
y = as.integer(unlist(config[keywords == "y"])[2])


# Use defaults if parameters were absent
if (length(iters) != 1)
   iters = 1000000
if (length(x) != 1)
   x = 250
if (length(y) != 1)
   y = 250


# Read in the list of Elements and store their names and
# colors
config_ele = config[keywords == "ele"]
ele_names = list()
ele_colors = list()
ele_conc = list()
for (i in 1:length(config_ele))
{
   ele_names[[i]] = config_ele[[i]][[2]]
   if (!is.null( colors[[ config_ele[[i]][[4]] ]] ))
      ele_colors[[ ele_names[[i]] ]] = colors[[ config_ele[[i]][[4]] ]]
   else
      ele_colors[[ ele_names[[i]] ]] = config_ele[[i]][[4]]
   ele_conc[[ ele_names[[i]] ]] = as.numeric(config_ele[[i]][[5]])
}
ele_colors[[ "default" ]] = colors[[ "black" ]]


# Read in the extinction rules
config_extinct = config[keywords == "extinct"]
extinct = lapply(config_extinct, function(y) return(y[3:length(y)]))
haveExtinction = function(state)
{
   for (i in 1:length(extinct))
   {
      if (all(state[which(unlist(ele_names) %in% extinct[[i]])] == 0))
         return(TRUE)
   }
   return(FALSE)
}


# Read in the list of Reactions, store reactants and
# products in the matrices N$Pre and N$Post, and calculate
# the reaction hazards
config_rxn = config[keywords == "rxn"]
N = list()
N$Pre  = matrix(0, nrow=length(config_rxn), ncol=length(config_ele))
N$Post = matrix(0, nrow=length(config_rxn), ncol=length(config_ele))
constants = vector(length=length(config_rxn))
if (length(config_rxn) > 0)
{
   for (i in 1:length(config_rxn))
   {
      # Grab the probability of reaction (the second word in
      # the line)
      prob = as.numeric(config_rxn[[i]][[2]])

      # Fill in the non-zero elements of the N$Pre matrix with
      # the stoichiometries of the reactants for each reaction
      j = 3
      while (config_rxn[[i]][[j]] != "->")
      {
         if (config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*")
         {
            if (config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2")
            {
               ele = config_rxn[[i]][[j+1]]
               ele_index = which(unlist(ele_names) == ele)
               for (n in 1:as.integer(config_rxn[[i]][[j]]))
               {
                  N$Pre[i, ele_index] = N$Pre[i, ele_index] + 1
               }
               j = j + 2
            } else {
               ele = config_rxn[[i]][[j]]
               ele_index = which(unlist(ele_names) == ele)
               N$Pre[i, ele_index] = N$Pre[i, ele_index] + 1
               j = j + 1
            }
         } else {
            j = j + 1
         }
      }

      # Fill in the non-zero elements of the N$Post matrix with
      # the stoichiometries of the products for each reaction
      j = j + 1
      while (j <= length(config_rxn[[i]]))
      {
         if (config_rxn[[i]][[j]] != "+" && config_rxn[[i]][[j]] != "*")
         {
            if (config_rxn[[i]][[j]] == "1" || config_rxn[[i]][[j]] == "2")
            {
               ele = config_rxn[[i]][[j+1]]
               ele_index = which(unlist(ele_names) == ele)
               for (n in 1:as.integer(config_rxn[[i]][[j]]))
               {
                  N$Post[i, ele_index] = N$Post[i, ele_index] + 1
               }
               j = j + 2
            } else {
               ele = config_rxn[[i]][[j]]
               ele_index = which(unlist(ele_names) == ele)
               N$Post[i, ele_index] = N$Post[i, ele_index] + 1
               j = j + 1
            }
         } else {
            j = j + 1
         }
      }

      # Calculate the stochastic rate constant for this
      # Reaction using the probability of reaction; the
      # calculation depends on the numbers and types of
      # reactants and products as well; see table 1 of
      # "Zeroth-, First-, and Second-Order Chemical Reactions
      # and Michaelis-Menten Enzyme Kinetics in an Artificial
      # Chemistry" (Gill, 2010)
      r = sum(N$Pre[i,])  # number of reactants
      p = sum(N$Post[i,]) # number of products
      m = MAX_RXNS_PER_SET_OF_REACTANTS
      d = 2               # dimensionality of space
      n = 3^d - 1         # number of neighbors (8)
      h = n/2 + 1         # half-neighborhood plus self (5)
      constants[i] = 0
      if (r == 0)
      {
         constants[i] = prob/(m*h) * (x*y)
         if (p == 2)
         {
            constants[i] = constants[i] * n / 2
         }
      }
      if (r == 1)
      {
         constants[i] = prob/(m*h)
         if (p == 2)
         {
            constants[i] = constants[i] * n
         }
      }
      if (r == 2)
      {
         constants[i] = prob/(m*h) * n/(x*y)
         if (length(N$Pre[i, N$Pre[i,] == 2]) > 0 )
         {
            constants[i] = constants[i] / 2
         }
      }
   }
}


# Define a function for calculating the reaction hazards
# given the state of the system
hazard_def = "function(state)\n{\n\treturn(c("
if (length(config_rxn) > 0)
{
   for (i in 1:length(config_rxn))
   {
      hazard_def = paste(hazard_def, constants[i], sep="")
      for (j in 1:length(config_ele))
      {
         if (N$Pre[i,j] == 1)
            hazard_def = paste(hazard_def, "*state[", j, "]", sep="")
         else if (N$Pre[i,j] > 1)
            hazard_def = paste(hazard_def, "*state[", j, "]^", N$Pre[i,j], sep="")
      }
      if (i < nrow(N$Pre))
         hazard_def = paste(hazard_def, ",\n\t\t", sep="")
      else
         hazard_def = paste(hazard_def, "))\n}", sep="")
   }
} else {
   hazard_def = paste(hazard_def, "))\n}", sep="")
}
N$h = eval(parse(text=hazard_def))


# Determine initial system state (should be approximately
# equivalent to the initial state of the simulator)
N$M = round(unlist(ele_conc) * x * y / MAX_ELES_NOT_INCLUDING_SOLVENT)


# Set completion flag
config_parsed = TRUE


# End

