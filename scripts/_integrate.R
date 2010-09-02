#!/usr/bin/Rscript
#
# Subroutine for defining the classical rate laws for the
# reaction system and performing a numerical integration on
# them; depends on _parse_config.R and the existence of
# should_integrate; if should_integrate is "true", it is
# treated as TRUE, otherwise it is treated as FALSE


# Check for config_parsed
if (!exists("config_parsed"))
{
   sink(stderr())
   print("NUMERICAL INTEGRATION FAILED: _parse_config.R must be executed first!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}

# Check for should_integrate
if (!exists("should_integrate"))
{
   sink(stderr())
   print("NUMERICAL INTEGRATION FAILED: should_integrate is not defined!")
   sink()
   q(save="no", status=1, runLast=FALSE)
}


if (should_integrate == "true")
{
   library(deSolve)


   # Define a function that returns the value of the rate
   # laws when passed a state vector
   rates = function(t, state, params)
   {
      S = t(N$Post-N$Pre)
      derivatives = as.vector(S %*% N$h(state))
      return(list(derivatives))
   }


   # Perform a numerical integration on each rate law to
   # find the expected trajectories of each Element
   expected_data = ode(N$M, seq(0, iters, length.out=10000), rates, constants)


   # Set completion flag
   integration_done = TRUE
} else {
   integration_done = FALSE
}


# End

