#!/bin/bash
#
# Conducts a series of experiments and stores the
# output in unique locations
#
# Usage: ./batch.sh batch experiments iters x y atoms
#   batch        the name for the batch of experiments
#   experiments  the number of experiments to run in the batch
#   iters        the number of iterations to run each experiment in the batch
#   x            the width of the world in each experiment in the batch
#   y            the height of the world in each experiment in the batch
#   atoms        the number of atoms in each experiment in the batch

# Check that the proper number of command line parameters were passed
if [ ! $# == 6 ]; then
   echo "BATCH FAILED: Improper parameter count!"
   echo "  Usage: ./batch.sh batch experiments iters x y atoms"
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
if [ ! -d data ]; then
   mkdir data
fi
if [ ! -d data/batch ]; then
   mkdir data/batch
fi

# Create a batch directory if the batch is uniquely named or halt to protect old data
if [ ! -d data/batch/$BATCH ]; then
   mkdir data/batch/$BATCH
else
   echo "BATCH FAILED: Batch data/batch/$BATCH already exists!"
   echo "  Rename this batch or move/delete the old data before continuting."
   exit
fi

# Run the experiments
for i in `seq 0 $(($EXPERIMENTS-1))`; do
   sleep 1
   echo "Beginning experiment $(($i+1)) of $EXPERIMENTS..."
   if [ ! -d data/batch/$BATCH/${NAME[i]} ]; then
      mkdir data/batch/$BATCH/${NAME[i]}
   fi
   time ./metabolism -g -i $ITERS -x $X -y $Y -a $ATOMS \
             -f data/batch/$BATCH/${NAME[i]}/config.out.${NAME[i]} \
                data/batch/$BATCH/${NAME[i]}/census.out.${NAME[i]} \
                data/batch/$BATCH/${NAME[i]}/diffusion.out.${NAME[i]}
   echo
done

