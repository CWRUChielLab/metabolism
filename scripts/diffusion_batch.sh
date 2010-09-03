#!/bin/bash
#
# Conducts a series of simulations, stores the output in unique locations,
# and statistically analyses and plots the diffusion data
#
# Usage: ./diffusion_batch.sh batch experiments iters x y
#   batch        the name for the batch of experiments
#   experiments  the number of experiments to run in the batch
#   iters        the number of iterations to run each experiment in the batch
#   x            the width of the world in each experiment in the batch
#   y            the height of the world in each experiment in the batch


# Check that the proper number of command line arguments
# were passed
if [ ! $# == 5 ]; then
   echo "DIFFUSION BATCH FAILED: Incorrect number of parameters!"
   echo "  Usage: ./diffusion_batch.sh batch experiments iters x y"
   exit
fi


# Copy command line arguments
BATCH=$1
EXPERIMENTS=$2
ITERS=$3
X=$4
Y=$5


# Create a sequence of numbers from 1 to $EXPERIMENTS with
# leading zeros
NAME=($(seq -w -s " " 1 $EXPERIMENTS))


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


# Run the experiments and conduct individual analyses
for i in `seq 0 $(($EXPERIMENTS-1))`; do
   sleep 1
   echo "Beginning experiment $(($i+1)) of $EXPERIMENTS..."
   if [ ! -d ../data/diffusion/$BATCH/${NAME[i]} ]; then
      mkdir ../data/diffusion/$BATCH/${NAME[i]}
   fi


   # Calculate the particle concentration
   CONC=$(echo "0.01*($i+1)" | bc)


   # Create a temporary config file for loading
   CONFIG="temp.config.out"
   echo "x $X"                 >  $CONFIG
   echo "y $Y"                 >> $CONFIG
   echo "iters $ITERS"         >> $CONFIG
   echo "reactions off"        >> $CONFIG
   echo ""                     >> $CONFIG
   echo "ele A A teal $CONC 0" >> $CONFIG
   echo ""                     >> $CONFIG


   time (                                                                         \
      ../src/metabolism --gui-off --load $CONFIG                                  \
         -f ../data/diffusion/$BATCH/${NAME[i]}/config.$BATCH.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/census.$BATCH.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/diffusion.$BATCH.${NAME[i]}.out   \
            ../data/diffusion/$BATCH/${NAME[i]}/rand.$BATCH.${NAME[i]}.out        \
      &&                                                                          \
      ../scripts/diffusion.R                                                      \
            pdf                                                                   \
            ../data/diffusion/$BATCH/${NAME[i]}/config.$BATCH.${NAME[i]}.out      \
            ../data/diffusion/$BATCH/${NAME[i]}/diffusion.$BATCH.${NAME[i]}.out   \
            ../data/diffusion/$BATCH/${NAME[i]}/plots-$BATCH-${NAME[i]}           \
            ../data/diffusion/$BATCH/${NAME[i]}/stats.$BATCH.${NAME[i]}.out       \
        )

   echo
done


# End

