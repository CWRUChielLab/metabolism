#!/bin/bash
#
# Conducts a series of experiments, stores the
# output in unique locations, and analyses the
# reaction rate data
#
# Usage: ./rate_batch.sh batch experiments iters x y atoms rxnorder prob
#   batch        the name for the batch of experiments
#   experiments  the number of experiments to run in the batch
#   iters        the number of iterations to run each experiment in the batch
#   x            the width of the world in each experiment in the batch
#   y            the height of the world in each experiment in the batch
#   atoms        the number of atoms in each experiment in the batch;
#                  passing -1 will run a batch with increasing densities
#   rxnorder     the order of the reaction; "first" or "second"
#   prob         the probability of the primary reaction occurring

# Check that the proper number of command line parameters were passed
if [ ! $# == 8 ]; then
   echo "BATCH FAILED: Incorrect number of parameters!"
   echo "  Usage: ./rate_batch.sh batch experiments iters x y atoms rxnorder prob"
   exit
fi

# Copy command line parameters
BATCH=$1
EXPERIMENTS=$2
ITERS=$3
X=$4
Y=$5
ATOMS=$6
RXNORDER=$7
PROB=$8

# Create a sequence of numbers from 1 to $EXPERIMENTS with leading zeros
NAME=($(seq -w -s : 1 $EXPERIMENTS | sed 's/:/ /g'))

# Create data directories if necessary
if [ ! -d ../data ]; then
   mkdir ../data
fi
if [ ! -d ../data/rate ]; then
   mkdir ../data/rate
fi

# Create a batch directory if the batch is uniquely named or halt to protect old data
if [ ! -d ../data/rate/$BATCH ]; then
   mkdir ../data/rate/$BATCH
else
   echo "BATCH FAILED: Batch ../data/rate/$BATCH already exists!"
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
#for i in `seq 0 $(($EXPERIMENTS-1))`; do
for i in `seq 30 $(($EXPERIMENTS-1))`; do
   sleep 1
   echo "Beginning experiment $(($i+1)) of $EXPERIMENTS..."
   if [ ! -d ../data/rate/$BATCH/${NAME[i]} ]; then
      mkdir ../data/rate/$BATCH/${NAME[i]}
   fi

   # If the atom count is not fixed, calculate the atom count,
   # incrementing by 0.05% density each experiment
   if [ "$FIXED" = '0' ]; then
      ATOMS=$(((5*($i+1)*$X*$Y*2+10000)/20000))
   fi

   time (                                                             \
      #../src/metabolism -g -i $ITERS -x $X -y $Y -a $ATOMS            \
      ../src/metabolism --shuffle -g -i $ITERS -x $X -y $Y -a $ATOMS  \
         -f ../data/rate/$BATCH/${NAME[i]}/config.${NAME[i]}.out      \
            ../data/rate/$BATCH/${NAME[i]}/census.${NAME[i]}.out      \
            ../data/rate/$BATCH/${NAME[i]}/diffusion.${NAME[i]}.out   \
      &&                                                              \
      ../scripts/rate_analysis.R                                      \
            $RXNORDER                                                 \
            $PROB                                                     \
            ../data/rate/$BATCH/${NAME[i]}/config.${NAME[i]}.out      \
            ../data/rate/$BATCH/${NAME[i]}/census.${NAME[i]}.out      \
            ../data/rate/$BATCH/${NAME[i]}/analysis_$BATCH_${NAME[i]} \
            ../data/rate/$BATCH/${NAME[i]}/stats.${NAME[i]}.out       \
            false                                                     \
        )
   echo
done

# Run batch analysis if atom count was not fixed
#if [ "$FIXED" = '0' ]; then
#   ./rate_batch_analysis.R                \
#      ../data/rate/$BATCH                 \
#      $EXPERIMENTS                        \
#      ../data/rate/$BATCH/analysis_$BATCH \
#      false
#fi

# End

