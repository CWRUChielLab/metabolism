#!/bin/bash
#
# Conducts a series of experiments, stores the
# output in unique locations, and analyses the
# diffusion data
#
# Usage: ./diffusion_batch.sh batch experiments iters x y atoms
#   batch        the name for the batch of experiments
#   experiments  the number of experiments to run in the batch
#   iters        the number of iterations to run each experiment in the batch
#   x            the width of the world in each experiment in the batch
#   y            the height of the world in each experiment in the batch
#   atoms        the number of atoms in each experiment in the batch;
#                  passing -1 will run a batch with increasing densities

# Check that the proper number of command line parameters were passed
if [ ! $# == 6 ]; then
   echo "BATCH FAILED: Incorrect number of parameters!"
   echo "  Usage: ./diffusion_batch.sh batch experiments iters x y atoms"
   exit
fi

# Copy command line parameters
BATCH=$1
EXPERIMENTS=$2
ITERS=$3
X=$4
Y=$5
ATOMS=$6

# Create a sequence of numbers from 1 to $EXPERIMENTS with leading zeros
NAME=($(seq -w -s : 1 $EXPERIMENTS | sed 's/:/ /g'))

# Create data directories if necessary
if [ ! -d ../data ]; then
   mkdir ../data
fi
if [ ! -d ../data/diffusion ]; then
   mkdir ../data/diffusion
fi

# Create a batch directory if the batch is uniquely named or halt to protect old data
if [ ! -d ../data/diffusion/$BATCH ]; then
   mkdir ../data/diffusion/$BATCH
else
   echo "BATCH FAILED: Batch ../data/diffusion/$BATCH already exists!"
   echo "  Rename this batch or move/delete the old data before continuing."
   exit
fi

# Determine whether atom count is fixed or increasing
if [ "$ATOMS" = '-1' ]; then
   FIXED=0
else
   FIXED=1
fi

# Run the experiments and conduct individual analyses
for i in `seq 0 $(($EXPERIMENTS-1))`; do
   sleep 1
   echo "Beginning experiment $(($i+1)) of $EXPERIMENTS..."
   if [ ! -d ../data/diffusion/$BATCH/${NAME[i]} ]; then
      mkdir ../data/diffusion/$BATCH/${NAME[i]}
   fi

   # If the atom count is not fixed, calculate the atom count,
   # incrementing by 0.05% density each experiment
   if [ "$FIXED" = '0' ]; then
      ATOMS=$(((5*($i+1)*$X*$Y*2+10000)/20000))
   fi

   time (                                                                  \
      ../src/metabolism -g -i $ITERS -x $X -y $Y -a $ATOMS                 \
         -f ../data/diffusion/$BATCH/${NAME[i]}/config.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/census.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/diffusion.${NAME[i]}.out   \
      &&                                                                   \
      ./diffusion_analysis.R                                               \
            ../data/diffusion/$BATCH/${NAME[i]}/config.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/diffusion.${NAME[i]}.out   \
            ../data/diffusion/$BATCH/${NAME[i]}/analysis_$BATCH_${NAME[i]} \
            ../data/diffusion/$BATCH/${NAME[i]}/stats.${NAME[i]}.out       \
            false                                                          \
        )
   echo
done

# Run batch analysis if atom count was not fixed
if [ "$FIXED" = '0' ]; then
   ./diffusion_batch_analysis.R                \
      ../data/diffusion/$BATCH                 \
      $EXPERIMENTS                             \
      ../data/diffusion/$BATCH/analysis_$BATCH \
      false
fi

# End

