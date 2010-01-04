#!/bin/bash

# Conducts a series of experiments and stores the
# output in unique locations

BATCH="alpha"
EXPERIMENTS=100
NAME=($(seq -w -s : 1 $EXPERIMENTS | sed 's/:/ /g'))
ITERS=25000
X=512
Y=512
ATOMS=1311  # 0.5% density
#ATOMS=1049  # 0.4% density
ALLUNIQUE=1

# Create batch data directories if necessary
if [ ! -d data ]; then
   mkdir data
fi
if [ ! -d data/batch ]; then
   mkdir data/batch
fi
if [ ! -d data/batch/$BATCH ]; then
   mkdir data/batch/$BATCH
fi

# Check that each experiment is new to avoid
# overriding data
for i in `seq 0 $(($EXPERIMENTS-1))`; do
   if [ -d data/batch/$BATCH/${NAME[i]} ]; then
      echo "Experiment data/batch/$BATCH/${NAME[i]} already exists!"
      echo "  Delete the data or rename this batch before continuing."
      ALLUNIQUE=0
   fi
done

# Run the experiments if experiment name is unique
if [ $ALLUNIQUE == 1 ]; then
   for i in `seq 0 $(($EXPERIMENTS-1))`; do
      sleep 1
      echo Beginning experiment $(($i+1)).
      if [ ! -d data/batch/$BATCH/${NAME[i]} ]; then
         mkdir data/batch/$BATCH/${NAME[i]}
      fi
      time ./metabolism -g -i $ITERS -x $X -y $Y -a $ATOMS \
                -f data/batch/$BATCH/${NAME[i]}/config.out.${NAME[i]} \
                   data/batch/$BATCH/${NAME[i]}/census.out.${NAME[i]} \
                   data/batch/$BATCH/${NAME[i]}/diffusion.out.${NAME[i]}
   done
fi

