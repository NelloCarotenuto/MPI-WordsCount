#!/bin/bash

# Strong scalability test

# This script automates the execution of MPI-WordsCount on a fixed input with an increasing number of processors from 1
# up to the value passed as --maxnp argument.

EXECUTABLE="MPI-WordsCount"
REPORTS="reports/"

if [[ $# -lt 5 ]]
then
    echo "Usages:"
    echo "\t$0 $EXECUTABLE --maxnp <value> -f <filenames>"
    echo "\t$0 $EXECUTABLE --maxnp <value> -d <dirname>"
    echo "\t$0 $EXECUTABLE --maxnp <value> -mf <masterfile>"
fi

if [[ $2 = "--maxnp" ]]
then
    MAXNP=$3

    if [[ $4 = "-f" ]]
    then
        shift 4

        for (( i=1; i<=$MAXNP; i++ ))
        do
            echo "Testing with $i processes ... "
            mpirun -np "$i" "$EXECUTABLE" -f "${@}" | tail -2
            echo
        done
    elif [[ $4 = "-d" ]] || [[ $4 = "-mf" ]]
    then
        for (( i=1; i<=$MAXNP; i++ ))
        do
            echo "Testing with $i processes ... "
            mpirun -np "$i" "$EXECUTABLE" "$4" "$5" | tail -2
            echo
        done
    else
        echo "Unknown $EXECUTABLE option \""$4"\""
        exit
    fi
else
    echo "Unknown option \""$2"\""
    exit
fi

echo "Tests done! Check reports in $REPORTS folder."
exit