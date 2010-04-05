#!/usr/bin/Rscript
#
# Reaction rate analysis for first-order reactions

#dir = "../data/rate/0.005/2000/17/"
dir = ""
path_to_config = paste(dir, "config.out", sep="")
path_to_data   = paste(dir, "census.out", sep="")

# Import experimental parameters
config = read.table(path_to_config, header=TRUE)
version = as.character(config["version"])
seed = as.integer(config["seed"])
iters = as.integer(config["iters"])
x = as.integer(config["x"])
y = as.integer(config["y"])
atoms = as.integer(config["atoms"])

# Import rate data and remove timesteps that had zero A atoms
rate_data = read.table(path_to_data, header=TRUE)
rate_data = rate_data[which(rate_data$A != 0),]

#########
# PLOTS #
#########

par(mfrow=c(2,2))

# Calculate k for atoms
a0_atoms = rate_data$A[1]
b0_atoms = rate_data$B[1]
k_atoms  = lm(-log(rate_data$A/a0_atoms)
               ~ 0 + rate_data$iter)$coefficients

# Calculate k for densities
a0_density = rate_data$A[1]/(x*y)
b0_density = rate_data$B[1]/(x*y)
k_density  = lm(-log((rate_data$A/(x*y))/a0_density)
               ~ 0 + rate_data$iter)$coefficients

# Plot atom data directly with expected trajectory
plot(rate_data$iter,   rate_data$A, col="red", xlab="Iterations", ylab="Atoms", ylim=c(0, max(rate_data$A, rate_data$B)))
points(rate_data$iter, rate_data$B, col="blue")
curve(a0_atoms*exp(-k_atoms*x), add=TRUE)
curve(a0_atoms-a0_atoms*exp(-k_atoms*x), add=TRUE)

# Plot linear-transformed atom data with fitted straight line
plot(rate_data$iter, -log(rate_data$A/a0_atoms), col="red", xlab="Iterations", ylab="")
curve(k_atoms*x, add=TRUE)

# Plot density data directly with expected trajectory
plot(rate_data$iter,   rate_data$A/(x*y), col="red", xlab="Iterations", ylab="Density", ylim=c(0, max(rate_data$A, rate_data$B)/(x*y)))
points(rate_data$iter, rate_data$B/(x*y), col="blue")
curve(a0_density*exp(-k_density*x), add=TRUE)
curve(a0_density-a0_density*exp(-k_density*x), add=TRUE)

# Plot linear-transformed density data with fitted straight line
plot(rate_data$iter, -log((rate_data$A/(x*y))/a0_density), col="red", xlab="Iterations", ylab="")
curve(k_density*x, add=TRUE)

