#!/bin/bash
#
# Conducts nine experiments and stores the
# output in unique locations without analyzing the
# reaction rate data
#
# Usage: ./rate_batch.sh batch iters x1 x2 x3 y1 y2 y3 atoms1 atoms2 atoms3
#   batch                   the name for the batch of experiments
#   iters                   the number of experiments to run each experiment
#                             in the batch
#   x1, x2, x3              the range of widths of the world in the nine
#                             experiments
#   y1, y2, y3              the range of heights of the world in the nine
#                             experiments
#   atoms1, atoms2, atoms3  the range of atom counts in the nine experiments

# Check that the proper number of command line parameters were passed
if [ ! $# == 11 ]; then
   echo "BATCH FAILED: Incorrect number of parameters!"
   echo "  Usage: ./rate_batch.sh batch iters x1 x2 x3 y1 y2 y3 atoms1 atoms2 atoms3"
   exit
fi

# Copy command line parameters
BATCH=$1
ITERS=$2
X[1]=$3
X[2]=$4
X[3]=$5
Y[1]=$6
Y[2]=$7
Y[3]=$8
ATOMS[1]=$9
ATOMS[2]=${10}
ATOMS[3]=${11}

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

# Run the experiments
for i in 1 2 3; do
   if [ ! -d ../data/rate/$BATCH/${ATOMS[i]} ]; then
      mkdir ../data/rate/$BATCH/${ATOMS[i]}
   fi
   for j in 1 2 3; do
      if [ ! -d ../data/rate/$BATCH/${ATOMS[i]}/${X[j]}x${Y[j]} ]; then
         mkdir ../data/rate/$BATCH/${ATOMS[i]}/${X[j]}x${Y[j]}
      fi

      sleep 1
      time (                                                                 \
         ../src/metabolism -g -i $ITERS -x ${X[j]} -y ${Y[j]} -a ${ATOMS[i]} \
            -f ../data/rate/$BATCH/${ATOMS[i]}/${X[j]}x${Y[j]}/config.out    \
               ../data/rate/$BATCH/${ATOMS[i]}/${X[j]}x${Y[j]}/census.out    \
               ../data/rate/$BATCH/${ATOMS[i]}/${X[j]}x${Y[j]}/diffusion.out \
           )
      echo
   done
done

# End

