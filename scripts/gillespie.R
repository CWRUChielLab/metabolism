#!/usr/bin/Rscript
#
# An implementation of the Gillespie algorithm
#
# Usage: ./gillespie.R output_type path_to_plots
#   output_type     the type of output that the R script should create; valid options
#                      are "pdf", "png", and "latex"
#   path_to_plots   the path of the output that the R script will create without
#                      the file extension

# Import command line arguments
Args = commandArgs()
if (length(Args) > 7)
{
   sink(stderr())
   print("SIMULATION FAILED: Too many parameters!")
   print("  Usage: ./gillespie.R output_type path_to_plots")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Set parameters to default values if they were not
# specified on the command line
if (!is.na(Args[6]))
{
   output_type = as.character(Args[6])
} else {
   output_type = "png"
}

if (!is.na(Args[7]))
{
   path_to_plots = as.character(Args[7])
} else {
   path_to_plots = "plots_gillespie"
}


# The Gillespie algorithm, taken from
# Wilkinson, D. J. 2006.
# Stochastic Modelling for Systems Biology.
# Chapman & Hall/CRC: Boca Raton, p.155.
# The function takes a stochastic petri net N (which has
# initial marking M, reactant quantities Pre, product
# quantities Post, and reaction hazard function h), a run
# duration T, and discritization time-step dt as arguments.
gillespie = function(N, T, dt, ...)
{
   x = N$M
   S = t(N$Post-N$Pre)
   u = nrow(S)
   v = ncol(S)

   n    = T %/% dt
   tt   = 0
   tvec = vector("numeric", n+1)
   xmat = matrix(0, ncol=u, nrow=n+1)

   i = 1
   target = 0
   tvec[i]  = target
   xmat[i,] = x

   repeat
   {
      h  = N$h(x, ...)
      h0 = sum(h)
      if (h0 < 1e-10)
         tt = 1e99
      else
         tt = tt + rexp(1, h0)

      while (tt >= target)
      {
         i = i + 1
         target = target + dt

         # Report progress
         if (i %% 500 == 0)
         {
            #print(paste("Step: ", i, " of ", n, " | ", (100*i)%/%n, "% complete\n", sep=""))
            #print(x)
         }

         tvec[i]  = target
         xmat[i,] = x

         if (i > n)
            return(list(t=tvec, x=xmat))

         # Check for extinction
         if ((x[1]==0 && x[2]==0 && x[3]==0) || (x[4]==0 && x[5]==0))
         {
            tvec = tvec[1:i]
            xmat = xmat[1:i,]
            return(list(t=tvec, x=xmat))
         }
      }

      j = sample(v, 1, prob=h)
      x = x + S[,j]
   }
}


# The stochastic petri net
N = list()

# The initial marking (particle counts):
# Ant, Ant-Inter, Ant-Store, GH, GH-Inter, Food
N$M = c(250, 0, 0, 250, 0, 1500)

# Quantities of reactants consumed in each reaction
N$Pre  = matrix(c( 1, 0, 0, 0, 0, 1,
                   0, 1, 0, 0, 0, 0,
                   0, 1, 0, 0, 0, 0,
                   0, 0, 1, 0, 0, 0,
                   0, 0, 0, 1, 0, 1,
                   0, 0, 0, 0, 1, 0,
                   1, 0, 0, 0, 0, 0,
                   0, 1, 0, 0, 0, 0,
                   0, 0, 1, 0, 0, 0,
                   0, 0, 0, 1, 0, 0,
                   0, 0, 0, 0, 1, 0,
                   0, 0, 0, 0, 0, 0), ncol=6, byrow=TRUE)

# Quantities of products produced in each reaction
N$Post = matrix(c( 0, 1, 0, 0, 0, 0,
                   2, 0, 0, 0, 0, 0,
                   0, 0, 1, 0, 0, 0,
                   0, 1, 0, 0, 0, 0,
                   0, 0, 0, 0, 1, 0,
                   0, 0, 0, 2, 0, 0,
                   0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 1), ncol=6, byrow=TRUE)

# Hazards (propensities) for each reaction, which are
# functions of the state vector y and depend on the rate
# constants k
N$h = function(y, k)
{
   return(c(k[1]*y[1]*y[6],
            k[2]*y[2],
            k[3]*y[2],
            k[4]*y[3],
            k[5]*y[4]*y[6],
            k[6]*y[5],
            k[7]*y[1],
            k[8]*y[2],
            k[9]*y[3],
            k[10]*y[4],
            k[11]*y[5],
            k[12]))
}

# The rate constants (see Gill, "Zeroth-, First-, and
# Second-Order Chemical Reactions and Michaelis-Menten
# Enzyme Kinetics in an Artificial Chemistry", table 1)
k = function(x, y)
{
   return(c(0.5*8/10/(x*y),
            0.05*8/10,
            0.02*1/10,
            0.01*1/10,
            0.5*8/10/(x*y),
            0.05*8/10,
            0.08*1/10,
            0.08*1/10,
            0.00*1/10,  # set this one to 0.08*1/10 for competition.load or 0.00*1/10 for antwostorage.load
            0.08*1/10,
            0.08*1/10,
            0.02*1/10*(x*y)))
}

# World size and max iterations
x = 250; y = 250; iters=1000000;
out = gillespie(N, T=iters, dt=1, k(x, y))

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

col = list()
col[1] = colors["darkorange"]
col[2] = colors["red"]
col[3] = colors["yellow"]
col[4] = colors["green"]
col[5] = colors["blue"]
col[6] = colors["hotpink"]

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

plot(out$t, out$x[,1]/(x*y),
   main="Observed Density Trajectories (Gillespie Algorithm)",
   xlab="Time",
   ylab="Density",
   ylim=c(0, max(out$x/(x*y))),
   col=col[[1]],
   type="l")
for (i in 2:6)
{
   points(out$t, out$x[,i]/(x*y),
      col=col[[i]],
      type="l")
}

legend("topright",
   legend=c("Ant", "Ant-Inter", "Ant-Store", "GH", "GH-Inter", "Food"),
   fill=unlist(col),
   bg="white")

# If individual LaTeX documents or PNG graphics are to be
# created, close the current graphics device
if (output_type == "png" || output_type == "latex")
{
   dev.off()
}

