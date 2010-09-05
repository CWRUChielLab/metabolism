#!/bin/bash
#
# Conducts a series of simulations with incrementally increasing particle
# densities, stores the output in unique locations, statistically analyses
# diffusion data, and plots the diffusion data
#
# Usage: ./diffusion_batch.sh batch iters x y min max steps/stepsize
#   batch           the name for the batch of simulations
#   iters           the number of iterations to run each simulation in the batch
#   x               the width of the world in each simulation in the batch
#   y               the height of the world in each simulation in the batch
#   min             the minimum particle density used in the first simulation;
#                      should be a real-valued number between 0 and 1
#   max             the maximum particle density used in the final simulation;
#                      should be a real-valued number between 0 and 1
#   steps/stepsize  either an integer (steps > 1) indicating the number of
#                      simulations to be run, or a real-valued number
#                      (0 < stepsize <= max-min) indicating the density
#                      increment between simulations


# Check that the proper number of command line arguments
# were passed
if [ ! $# == 7 ]; then
   echo "DIFFUSION BATCH FAILED: Incorrect number of parameters!"
   echo "  Usage: ./diffusion_batch.sh batch iters x y min max steps/stepsize"
   exit
fi


# Copy command line arguments
BATCH=$1
ITERS=$2
X=$3
Y=$4
MIN=$5
MAX=$6


# Verify that min and max are safe
if [ $(echo "(0 <= $MIN) && ($MIN < $MAX) && ($MAX <= 1)" | bc) == 0 ]; then
   echo "DIFFUSION BATCH FAILED: Invalid min and/or max!"
   exit
fi


# Determine steps and stepsize
if [ $(echo "$7 > 1" | bc) == 1 ]; then
   STEPS=$(echo "$7/1" | bc)                      # Round down
   STEPSIZE=$(echo "($MAX-$MIN)/$STEPS" | bc -l)  # Floating point division
elif [ $(echo "(0 < $7) && ($7 <= $MAX-$MIN)" | bc) == 1 ]; then
   STEPSIZE=$7
   STEPS=$(echo "($MAX-$MIN)/$STEPSIZE" | bc)     # Integer division
else
   echo "DIFFUSION BATCH FAILED: Invalid steps/stepsize!"
   exit
fi


# Create a sequence of numbers from 1 to $STEPS+1 with
# leading zeros
NAME=($(seq -w -s " " 1 $STEPS))


# Create data directories if necessary
if [ ! -d ../data ]; then
   mkdir ../data
fi
if [ ! -d ../data/diffusion ]; then
   mkdir ../data/diffusion
fi


# Create a batch directory if the batch is uniquely named,
# or halt to protect old data
if [ ! -d ../data/diffusion/$BATCH ]; then
   mkdir ../data/diffusion/$BATCH
else
   echo "DIFFUSION BATCH FAILED: Batch ../data/diffusion/$BATCH already exists!"
   echo "  Rename this batch or move/delete the old data before continuing."
   exit
fi


# Constant used for determining densities (should match
# the value defined in src/sim.h)
MAX_ELES_NOT_INCLUDING_SOLVENT=8


# Calculate maximum density available to a single Element
DENSITYSCALE=$(echo "1/$MAX_ELES_NOT_INCLUDING_SOLVENT" | bc -l)


# Run the simulations and conduct individual analyses
for i in `seq 0 $(($STEPS-1))`; do
   # Pause to guarentee that each seed taken from the
   # system time is unique
#   sleep 1
   echo "Beginning simulation $(($i+1)) of $STEPS..."

   # Create a data directory for the simulation
   mkdir ../data/diffusion/$BATCH/${NAME[i]}

   # Calculate the particle density needed for this simulation
   DENSITY=$(echo "$MIN + $(($i+1))*$STEPSIZE" | bc)

   # Create a temporary config file for loading
   LOAD="temp.config.out"
   rm $LOAD
   for j in `seq 0 $(($MAX_ELES_NOT_INCLUDING_SOLVENT-1))`; do
      if [ $(echo "$DENSITY > $DENSITYSCALE" | bc) == 1 ]; then
         echo "ele $j ? teal 1 $j" >> $LOAD
         DENSITY=$(echo "$DENSITY-$DENSITYSCALE" | bc)
      else
         echo "ele $j ? teal $(echo "$DENSITY/$DENSITYSCALE" | bc -l) $j" >> $LOAD
         break
      fi
   done

   time (                                                                            \
      ../src/metabolism --gui-off -x $X -y $Y --iters $ITERS --rxns-off --load $LOAD \
         --files ../data/diffusion/$BATCH/${NAME[i]}/config.$BATCH.${NAME[i]}.out    \
                 ../data/diffusion/$BATCH/${NAME[i]}/census.$BATCH.${NAME[i]}.out    \
                 ../data/diffusion/$BATCH/${NAME[i]}/diffusion.$BATCH.${NAME[i]}.out \
                 ../data/diffusion/$BATCH/${NAME[i]}/rand.$BATCH.${NAME[i]}.out      \
      &&                                                                             \
      ../scripts/diffusion.R                                                         \
         png                                                                         \
         ../data/diffusion/$BATCH/${NAME[i]}/config.$BATCH.${NAME[i]}.out            \
         ../data/diffusion/$BATCH/${NAME[i]}/diffusion.$BATCH.${NAME[i]}.out         \
         ../data/diffusion/$BATCH/${NAME[i]}/plots-$BATCH-${NAME[i]}                 \
         ../data/diffusion/$BATCH/${NAME[i]}/stats.$BATCH.${NAME[i]}.out             \
        )

   echo
done


# Run batch analysis
../scripts/diffusion_batch_analysis.R \
   ../data/diffusion/$BATCH           \
   $STEPS                             \
   png                                \
   ../data/diffusion/$BATCH/plots


# End

