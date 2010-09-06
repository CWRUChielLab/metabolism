#!/usr/bin/Rscript
#
# Subroutine for conducting a statistical analysis of the
# diffusion data; depends on _parse_config.R,
# _parse_diffusion.R, and the existence of path_to_stats


# Check for config_parsed
if (!exists("config_parsed"))
{
   sink(stderr())
   print("ANALYZE DIFFUSION FAILED: _parse_config.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for diffusion_parsed
if (!exists("diffusion_parsed"))
{
   sink(stderr())
   print("ANALYZE DIFFUSION FAILED: _parse_diffusion.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for path_to_stats
if (!exists("path_to_stats"))
{
   sink(stderr())
   print("ANALYZE DIFFUSION FAILED: path_to_stats is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Construct a list for exporting statistical test results
stats_data = list();


# Define the diffusion coefficient for the simulation
d = 3/8


# Set a level of significance
alpha = 0.05


# Calculate the theoretical population mean and variance,
# and find the confidence intervals around these values
# which contain 100*(1-alpha)% of sample mean and variance
# values assuming they are drawn from the theoretical
# population
expected_mean = 0
expected_var  = 2*d*iters
expected_sd   = sqrt(expected_var)
expected_mean_lower_ci = expected_mean - qnorm(1-alpha/2) * expected_sd / sqrt(samples)
expected_mean_upper_ci = expected_mean + qnorm(1-alpha/2) * expected_sd / sqrt(samples)
expected_var_lower_ci  = qchisq(alpha/2,   df=samples-1) * expected_var / (samples-1)
expected_var_upper_ci  = qchisq(1-alpha/2, df=samples-1) * expected_var / (samples-1)
expected_sd_lower_ci   = sqrt(expected_var_lower_ci)
expected_sd_upper_ci   = sqrt(expected_var_upper_ci)

# Save statistics for exporting
stats_data[["x"]] = x
stats_data[["y"]] = y
stats_data[["samples"]] = samples
stats_data[["density"]] = samples / (x*y)
stats_data[["alpha"]] = alpha
stats_data[["expected_mean"]] = expected_mean
stats_data[["expected_var"]]  = expected_var
stats_data[["expected_sd"]]   = expected_sd
stats_data[["expected_mean_lower_ci"]] = expected_mean_lower_ci
stats_data[["expected_mean_upper_ci"]] = expected_mean_upper_ci
stats_data[["expected_var_lower_ci"]]  = expected_var_lower_ci
stats_data[["expected_var_upper_ci"]]  = expected_var_upper_ci
stats_data[["expected_sd_lower_ci"]]   = expected_sd_lower_ci
stats_data[["expected_sd_upper_ci"]]   = expected_sd_upper_ci

diffusion_types = c("dx_actual", "dx_ideal", "dy_actual", "dy_ideal")
for (i in 1:length(diffusion_types))
{
   # Calculate the sample mean and variance, and find the
   # 100*(1-alpha)% confidence intervals around these values
   # which contain the population mean and variance from
   # which the samples were drawn
   data_mean = mean(diffusion_data[, diffusion_types[i]])
   data_var  = var(diffusion_data[, diffusion_types[i]])
   data_sd   = sqrt(data_var)
   data_mean_lower_ci = data_mean - qt(1-alpha/2, df=samples-1) * data_sd / sqrt(samples)
   data_mean_upper_ci = data_mean + qt(1-alpha/2, df=samples-1) * data_sd / sqrt(samples)
   data_var_lower_ci  = (samples-1) * data_var / qchisq(1-alpha/2, df=samples-1)
   data_var_upper_ci  = (samples-1) * data_var / qchisq(alpha/2,   df=samples-1)
   data_sd_lower_ci   = sqrt(data_var_lower_ci)
   data_sd_upper_ci   = sqrt(data_var_upper_ci)

   # Save statistics for exporting
   stats_data[[paste(diffusion_types[i], "_mean", sep="")]] = data_mean
   stats_data[[paste(diffusion_types[i], "_var", sep="")]]  = data_var
   stats_data[[paste(diffusion_types[i], "_sd", sep="")]]   = data_sd
   stats_data[[paste(diffusion_types[i], "_mean_lower_ci", sep="")]] = data_mean_lower_ci
   stats_data[[paste(diffusion_types[i], "_mean_upper_ci", sep="")]] = data_mean_upper_ci
   stats_data[[paste(diffusion_types[i], "_var_lower_ci", sep="")]]  = data_var_lower_ci
   stats_data[[paste(diffusion_types[i], "_var_upper_ci", sep="")]]  = data_var_upper_ci
   stats_data[[paste(diffusion_types[i], "_sd_lower_ci", sep="")]]   = data_sd_lower_ci
   stats_data[[paste(diffusion_types[i], "_sd_upper_ci", sep="")]]   = data_sd_upper_ci

   # Conduct the Kolmogorov-Smirnov goodness-of-fit test,
   # comparing the data to a normal distribution using the
   # theoretical mean and variance values, and suppress
   # warnings about ties (a continuous, normal distribution
   # should not produce ties, yet the data is discrete)
   ks_results = suppressWarnings(ks.test(diffusion_data[, diffusion_types[i]],
      "pnorm", mean=expected_mean, sd=expected_sd))

   # Save statistics for exporting
   stats_data[[paste(diffusion_types[i], "_D", sep="")]] = ks_results$statistic
   stats_data[[paste(diffusion_types[i], "_p", sep="")]] = ks_results$p.value
}


# Dump statistics to file
f = file(path_to_stats, "w")
for( i in 1:length(stats_data) )
{
   cat(names(stats_data)[i], as.numeric(stats_data[i]), "\n", file=f)
}
close(f)


# Set completion flag
diffusion_analyzed = TRUE


# End

