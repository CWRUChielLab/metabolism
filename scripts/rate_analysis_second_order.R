#!/usr/bin/Rscript
#
# Reaction rate analysis for second-order reactions

#dir = "../data/rate/0.02/8000/17/"
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

# Import rate data and remove timesteps that had zero A or B atoms
rate_data = read.table(path_to_data, header=TRUE)
rate_data = rate_data[which(rate_data$A != 0 & rate_data$B != 0),]

#########
# PLOTS #
#########

par(mfrow=c(2,2))

# Calculate k for atoms
a0_atoms = rate_data$A[1]
b0_atoms = rate_data$B[1]
if (a0_atoms != b0_atoms)
{
   k_atoms = lm(log((rate_data$A*b0_atoms)/(rate_data$B*a0_atoms))/(a0_atoms-b0_atoms)
                  ~ 0 + rate_data$iter)$coefficients
} else {
   k_atoms = lm(1/rate_data$A - 1/a0_atoms
                  ~ 0 + rate_data$iter)$coefficients
}

# Calculate k for densities
a0_density = rate_data$A[1]/(x*y)
b0_density = rate_data$B[1]/(x*y)
if (a0_density != b0_density)
{
   k_density = lm(log((rate_data$A*b0_density)/(rate_data$B*a0_density))/(a0_density-b0_density)
                  ~ 0 + rate_data$iter)$coefficients
} else {
   k_density = lm((x*y)/rate_data$A - 1/a0_density
                  ~ 0 + rate_data$iter)$coefficients
}

# Plot atom data directly with expected trajectory
plot(rate_data$iter,   rate_data$A, col="red", xlab="Iterations", ylab="Atoms", ylim=c(0,max(rate_data$A, rate_data$B)))
points(rate_data$iter, rate_data$B, col="blue")
if (a0_atoms != b0_atoms)
{
   curve(a0_atoms*(b0_atoms-a0_atoms)/(b0_atoms*exp(k_atoms*(b0_atoms-a0_atoms)*x)-a0_atoms), add=TRUE)
   curve(b0_atoms*(a0_atoms-b0_atoms)/(a0_atoms*exp(k_atoms*(a0_atoms-b0_atoms)*x)-b0_atoms), add=TRUE)
} else {
   curve(1/(k_atoms*x+1/a0_atoms), add=TRUE)
   curve(1/(k_atoms*x+1/b0_atoms), add=TRUE)
}

# Plot linear-transformed atom data with fitted straight line
if (a0_atoms != b0_atoms)
{
   plot(rate_data$iter,   log((rate_data$A*b0_atoms)/(rate_data$B*a0_atoms))/(a0_atoms-b0_atoms), col="red", xlab="Iterations", ylab="1/Atoms")
   points(rate_data$iter, log((rate_data$B*a0_atoms)/(rate_data$A*b0_atoms))/(b0_atoms-a0_atoms), col="blue")
} else {
   plot(rate_data$iter,   1/rate_data$A-1/a0_atoms, col="red", xlab="Iterations", ylab="1/Atoms")
   points(rate_data$iter, 1/rate_data$B-1/b0_atoms, col="blue")
}
curve(k_atoms*x, add=TRUE)

# Plot density data directly with expected trajectory
plot(rate_data$iter,   rate_data$A/(x*y), col="red", xlab="Iterations", ylab="Density", ylim=c(0,max(rate_data$A, rate_data$B)/(x*y)))
points(rate_data$iter, rate_data$B/(x*y), col="blue")
if (a0_density != b0_density)
{
   curve(a0_density*(b0_density-a0_density)/(b0_density*exp(k_density*(b0_density-a0_density)*x)-a0_density), add=TRUE)
   curve(b0_density*(a0_density-b0_density)/(a0_density*exp(k_density*(a0_density-b0_density)*x)-b0_density), add=TRUE)
} else {
   curve(1/(k_density*x+1/a0_density), add=TRUE)
   curve(1/(k_density*x+1/b0_density), add=TRUE)
}

# Plot linear-transformed density data with fitted straight line
if (a0_density != b0_density)
{
   plot(rate_data$iter,   log((rate_data$A*b0_density)/(rate_data$B*a0_density))/(a0_density-b0_density), col="red", xlab="Iterations", ylab="1/Density")
   points(rate_data$iter, log((rate_data$B*a0_density)/(rate_data$A*b0_density))/(b0_density-a0_density), col="blue")
} else {
   plot(rate_data$iter,   (x*y)/rate_data$A-1/a0_density, col="red", xlab="Iterations", ylab="1/Density")
   points(rate_data$iter, (x*y)/rate_data$B-1/b0_density, col="blue")
}
curve(k_density*x, add=TRUE)

