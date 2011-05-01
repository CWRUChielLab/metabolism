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

def coef(n, max_rxn_order = 2):
   """Construct a list of all n-tuples of stochiometric coefficients with
   positive sums less than or equal to max_rxn_order.
      n: the length of each tuple.
      max_rxn_order: the maximum allowable sum for a tuple."""

   return [x for x in product(range(max_rxn_order+1),repeat=n) if 0<sum(x)<=max_rxn_order]

############################################################

def buildRxns(names, mass, gibbs):
   """Construct the set of all valid reactions from the given element parameters,
   and return the reactant and product petri net matrices.
      names: a list of strings.
      mass:  a list of molecular weights.
      gibbs: a list of free energy values."""

   assert(len(names) == len(mass) == len(gibbs))

   # Construct a list of all tuples of stochiometric
   # coefficients with positive sums less than or equal to 2
   coefficients = coef(len(names))

   # Check every combination of reactants and products, and
   # store those combinations that meet the requirements of
   # representing a reaction in the simulation: the
   # reactants and products differ, and mass is conserved
   r = []
   p = []
   for reactants in coefficients:
      for products in coefficients:
         if reactants != products:
            reactantMass = dot(mass, reactants)
            productMass  = dot(mass, products)
            if reactantMass == productMass:
               r.append(reactants)
               p.append(products)

   return r, p

############################################################

def printEles(names, mass, gibbs):
   """Print the element table.
      names: a list of strings.
      mass:  a list of molecular weights.
      gibbs: a list of free energy values."""

   assert(len(names) == len(mass) == len(gibbs))

   for i in range(len(names)):
      print("%s\tMass: %d \tFree Energy: %d" % (names[i], mass[i], gibbs[i]))

############################################################

def printRxns(names, gibbs, reactants, products):
   """Print the reaction table.
      names: a list of strings.
      gibbs: a list of free energy values.
      reactants, products: matrices of stocheometric coefficients."""

   assert(len(names) == len(gibbs) == len(x) for x in reactants + products)
   assert(len(reactants) == len(products))

   for i in range(len(reactants)):
      # Find the indices of all reactants with coefficients > 0
      r = [index for index,n in enumerate(reactants[i]) if n>0]

      # Print the reactants
      for j in r:
         if reactants[i][j] == 1:
            print("%s" % names[j], end="")
         else:
            print("%d %s" % (reactants[i][j], names[j]), end="")
         if j != r[-1]:
            print(" + ", end="")

      print(" -> ", end="")

      # Find the indices of all products with coefficients > 0
      p = [index for index,n in enumerate(products[i]) if n>0]

      # Print the products
      for j in p:
         if products[i][j] == 1:
            print("%s" % names[j], end="")
         else:
            print("%d %s" % (products[i][j], names[j]), end="")
         if j != p[-1]:
            print(" + ", end="")

      # Calculate and print the free energy change
      deltag = dot(gibbs, products[i]) - dot(gibbs, reactants[i])
      print("\t  ΔG = %d" % deltag)

############################################################

if __name__ == "__main__":

   import random
   from math import trunc
   from time import time

   seed = trunc(time())
   random.seed(seed)
   print(":: SEED = %d ::" % seed)

   n = random.randint(3,7)
   names = [a for i,a in enumerate("ABCDEFGHIJKLM") if i<n]
   mass  = []
   gibbs = []
   for i in range(n):
      mass.append(random.randint(1,30))
      gibbs.append(random.randint(1,100))
   reactants, products = buildRxns(names, mass, gibbs)

   print(":: ELEMENT TABLE ::")
   printEles(names, mass, gibbs)
   print()
   print(":: REACTION TABLE ::")
   printRxns(names, gibbs, reactants, products)


# End

