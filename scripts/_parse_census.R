#!/usr/bin/Rscript
#
# Subroutine for importing the contents of a census file;
# depends on _parse_config.R and the existence of
# path_to_census


# Check for config_parsed
if (!exists("config_parsed"))
{
   sink(stderr())
   print("PARSE CENSUS FAILED: _parse_config.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for path_to_census
if (!exists("path_to_census"))
{
   sink(stderr())
   print("PARSE CENSUS FAILED: path_to_census is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


# Import simulated kinetics data
census_data = read.table(path_to_census, header=TRUE, check.names=FALSE)
iter_data = census_data[["iter"]]
ele_data = census_data[names(census_data) != "iter" & names(census_data) != "total"]
ele_data = ele_data[, unlist(ele_names)]
total_data = census_data[["total"]]


# Set completion flag
census_parsed = TRUE


# End

