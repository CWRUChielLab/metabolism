#!/usr/bin/Rscript
#
# Subroutine for initialization, importing the contents of a
# config file, and building a petri net for the reaction
# system

# Check for path_to_config
if (!exists("path_to_config"))
{
   sink(stderr())
   print("PARSE CONFIG FAILED: path_to_config not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
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
config = strsplit(config[config != ""], " ")
keywords = sapply(config, head, n=1)
iters = as.integer(unlist(config[keywords == "iters"])[2])
atoms = as.integer(unlist(config[keywords == "atoms"])[2])
x = as.integer(unlist(config[keywords == "x"])[2])
y = as.integer(unlist(config[keywords == "y"])[2])

# Use defaults if parameters were absent
if (length(iters) != 1)
   iters = 1000000
if (length(atoms) != 1)
   atoms = 2000
if (length(x) != 1)
   x = 250
if (length(y) != 1)
   y = 250

# Read in the list of Elements and store their names and
# colors
config_ele = config[keywords == "ele"]
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

# Read in the list of init's
config_init = unlist(config[keywords == "init"])
if (config_init[2] == "0")
{
   init = vector(length=0)
} else {
   init = config_init[3:length(config_init)]
}

# Read in the list of extinct's
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
   m = 2               # max_rxns_per_set_of_reactants
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

# Define a function for calculating the reaction hazard
# given the state of the system and the stochastic rate
# constants
N$h = function(state, c)
{
   hazards = vector(length=length(c))
   for (i in 1:length(c))
   {
      hazards[i] = c[i] * prod(state^N$Pre[i,])
   }
   return(hazards)
}

