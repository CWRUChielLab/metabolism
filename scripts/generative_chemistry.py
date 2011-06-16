#!/usr/bin/env python3

############################################################
#                   GENERATIVE CHEMISTRY                   #
############################################################


def dot(v1, v2):
   """Find the dot product of two vectors of equal length."""

   assert(len(v1) == len(v2))

   return sum(x*y for x,y in zip(v1,v2))

############################################################

from itertools import product

def coefs(n, max_rxn_order=2):
   """Construct a matrix of all legal combinations of reactant or product
   stoichiometric coefficients for a system.
   Takes as arguments
      n: the number of species in the system
      max_rxn_order: the greatest possible reaction order in the system
   Returns
      a matrix of stoichiometric coefficients; rows are legal tuples and
         columns are species"""

   return [x for x in product(range(max_rxn_order+1),repeat=n) if 0<sum(x)<=max_rxn_order]

############################################################

def buildRxns(mass):
   """Construct the set of all valid reactions from the given species parameters.
   Takes as arguments
      mass:  the molecular weights of the species
   Returns
      reactants, products: matrices of stoichiometric coefficients; rows are
         reactions and columns are species"""

   n = len(mass)

   # Construct a list of all legal combinations of
   # stoichiometric coefficients for a system of n species
   # with a maximum order of reaction of 2
   coefficients = coefs(n, 2)

   # Check every legal combination of reactants and
   # products, and store those combinations that meet the
   # requirements of representing a reaction in the
   # system: the reactants and products differ, and mass is
   # conserved
   reactants = []
   products  = []
   for r in coefficients:
      for p in coefficients:
         if r != p:
            reactantMass = dot(mass, r)
            productMass  = dot(mass, p)
            if reactantMass == productMass:
               reactants.append(r)
               products.append(p)

   return reactants, products

############################################################

import random
from math import trunc
from time import time

def randChem(n=None, seed=None):
   """Create a complete chemistry with randomly assigned species mass and free
   energy values.
   Takes as arguments
      n: the number of species
      seed: the seed for the random number generator
   Returns
      names:  the names of the species
      colors: the colors of the species
      mass:   the molecular weights of the species
      gibbs:  the free energy values of the species
      reactants, products: matrices of stoichiometric coefficients; rows are
         reactions and columns are species
      seed: the seed used by the random number generator"""

   if seed == None:
      seed = trunc(time())
   random.seed(seed)
   if n == None:
      n = random.randint(3,7)

   assert(0<=n<=10)

   names  = list("ABCDEFGHIJ")[0:n]
   colors = ['teal','darkorange','hotpink','yellow','red','blue','lime','fuchsia','blueviolet','maroon'][0:n]
   mass   = []
   gibbs  = []
   for i in range(n):
      mass.append(random.randint(1,30))
      gibbs.append(random.randint(1,4000))
   reactants, products = buildRxns(mass)

   return names, colors, mass, gibbs, reactants, products, seed

############################################################

def printEles(names, colors):
   """Print the table of species.
   Takes as arguments
      names:  the names of the species
      colors: the colors of the species"""

   assert(len(names) == len(colors))

   for name,color in zip(names, colors):
      print("ele %s %s %s %.2f" % (name, name, color, random.uniform(0,1)))

############################################################

def printRxns(names, gibbs, reactants, products):
   """Print the table of reactions.
   Takes as arguments
      names: the names of the species
      gibbs: the free energy values of the species
      reactants, products: matrices of stoichiometric coefficients; rows are
         reactions and columns are species"""

   assert(len(names) == len(gibbs) == len(x) for x in reactants + products)
   assert(len(reactants) == len(products))

   for r,p in zip(reactants, products):
      # Calculate and print the free energy change
      deltag = dot(gibbs, p) - dot(gibbs, r)
      print("rxn %.2f " % deltag, end="")

      # Find the name-coefficient pairs of all reactants and
      # products with positive stoichiometric coefficients
      r = [(name,coef) for name,coef in zip(names, r) if coef>0]
      p = [(name,coef) for name,coef in zip(names, p) if coef>0]

      # Print each reactant
      for name,coef in r:
         if coef == 1:
            print("%s" % name, end="")
         else:
            print("%d %s" % (coef,name), end="")
         if (name,coef) != r[-1]:
            print(" + ", end="")
         else:
            print(" -> ", end="")

      # Print each product
      for name,coef in p:
         if coef == 1:
            print("%s" % name, end="")
         else:
            print("%d %s" % (coef,name), end="")
         if (name,coef) != p[-1]:
            print(" + ", end="")
         else:
            print()

############################################################

if __name__ == "__main__":

   names, colors, mass, gibbs, reactants, products, seed = randChem()

   printEles(names, colors)
   print()
   printRxns(names, gibbs, reactants, products)
   print()

   # Determine probabilities of reaction
   from math import exp
   temp = 298
   R = 8.3145
   probs = []
   for r,p in zip(reactants, products):
      deltag = dot(gibbs, p) - dot(gibbs, r)
      activation_energy = max(0, deltag)  # intermediates pose no energy barrier;
                                          # prob = 1 when deltag <= 0
      probs.append(exp(-activation_energy / temp / R))
   #print("RXN PROBS at T =", temp, "K:", " ".join("%.2f" % x for x in probs))


# End

