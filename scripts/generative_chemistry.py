#!/usr/bin/env python3

############################################################
#                   GENERATIVE CHEMISTRY                   #
############################################################


def dotProduct(v1, v2):
   """Find the dot product of two vectors."""
   assert(len(v1) == len(v2))
   return sum([x*y for x,y in zip(v1,v2)])

############################################################

def printEles(names, mass, charge, gibbs):
   """Print the element table.
      names:  a list of strings.
      mass:   a list of molecular weights.
      charge: a list of valences.
      gibbs:  a list of free energy values."""

   assert(len(names) == len(mass) == len(charge) == len(gibbs))

   for i in range(len(names)):
      print("%s\tMass: %d \tCharge: %d \tFree Energy: %d" % (names[i], mass[i], charge[i], gibbs[i]))

############################################################

def printRxn(names, gibbs, reactants, products):
   """Print the reaction equation. Chemicals named "Solvent" are ignored.
      names: a list of strings.
      gibbs: a list of free energy values.
      reactants and products: lists of stocheometic coefficients."""

   assert(len(names) == len(gibbs) == len(reactants) == len(products))

   # Find the indices of all reactants with coefficients > 0
   r = [i for i,n in enumerate(reactants) if n > 0]

   # Print the reactants
   for i in r:
      if names[i] != "Solvent":
         if reactants[i] == 1:
            print("%s" % names[i], end="")
         elif reactants[i] > 1:
            print("%d %s" % (reactants[i], names[i]), end="")
         if i != r[-1]:
            print(" + ", end="")

   print(" -> ", end="")

   # Find the indices of all products with coefficients > 0
   p = [i for i,n in enumerate(products) if n > 0]

   # Print the products
   for i in p:
      if names[i] != "Solvent":
         if products[i] == 1:
            print("%s" % names[i], end="")
         elif products[i] > 1:
            print("%d %s" % (products[i], names[i]), end="")
         if i != p[-1]:
            print(" + ", end="")

   # Calculate and print the free energy change
   deltag = dotProduct(gibbs, products) - dotProduct(gibbs, reactants)
   print("\t  ΔG = %d" % deltag)

############################################################

def buildRxns(names, mass, charge, gibbs):
   """List all valid reactions.
      names:  a list of strings.
      mass:   a list of molecular weights.
      charge: a list of valences.
      gibbs:  a list of free energy values."""

   assert(len(names) == len(mass) == len(charge) == len(gibbs))

   # Construct a matrix of combinations of stochiometic
   # coefficients with the constraints that rows are unique
   # and add up to 2
   #
   # e.g. if len(names)==3, then
   #      matrix = [[ 2, 0, 0 ],
   #                [ 1, 1, 0 ],
   #                [ 1, 0, 1 ],
   #                [ 0, 2, 0 ],
   #                [ 0, 1, 1 ],
   #                [ 0, 0, 2 ]]
   matrix = []
   for i in range(0, len(names)):
      for j in range(i, len(names)):
         row = [0]*len(names)
         row[i] += 1
         row[j] += 1
         matrix.append(row)

   # Check every unique combination of reactants and
   # products, and print out only those combinations that
   # conserve mass and charge
   for reactants in matrix:
      for products in matrix:
         if reactants != products:
            reactantMass = dotProduct(mass, reactants)
            productMass  = dotProduct(mass, products)
            if reactantMass == productMass:
               reactantCharge = dotProduct(charge, reactants)
               productCharge  = dotProduct(charge, products)
               if reactantCharge == productCharge:
                  printRxn(names, gibbs, reactants, products)

############################################################

if __name__ == "__main__":
   #names  = ["Solvent", "A", "B", "C", "D"]
   #mass   = [0, 1, 2, 3, 4]
   #charge = [0, 0, 0, 0, 0]
   #gibbs  = [0, 0, 0, 0, 0]

   names  = ["Solvent", "A", "B", "C", "D", "X", "Y", "Z"]
   mass   = [0, 3, 7, 10, 6, 9, 1, 4]
   charge = [0, 0, 0, 1, 0, 0, -1, 0]
   #charge = [0, 0, 0, 0, 0, 0, 0, 0]
   gibbs  = [0, 1, 4, 10, 11, 15, 30, 8]

   print(":: ELEMENT TABLE ::")
   printEles(names, mass, charge, gibbs)
   print()
   print(":: REACTION TABLE ::")
   buildRxns(names, mass, charge, gibbs)


# End

