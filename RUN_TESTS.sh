#!/bin/bash
#----------------------------------------------------------------------------------------
# Ryan Schachte
# Batch Bash Test Runner
# USEAGE:
# Run inside project root, make sure CHMOD 777 to execute and run inside directory
#----------------------------------------------------------------------------------------

correct=0
incorrect=0
files_not_found=0
total_ran=0

for f in tests/*.txt
do
  total_ran=$((total_ran+1))

 x=$(./a.out < $f)

 if [ ! -f $f.expected ]; then
     incorrect=$((incorrect+1))
     files_not_found=$((files_not_found+1))
   else
     y=$(cat $f.expected)

    if [[ ($x != $y) ]]; then
        echo "
          + FAILURE ON ($f)
        "
        incorrect=$((incorrect+1))
      else
        correct=$((correct+1))

    fi
 fi
done

echo "---------------------------------"
echo "TESTING OUTPUT SUMMARY FOR 340"
echo "---------------------------------"
echo "$correct total tests passing"
echo "$incorrect failed tests"
echo "$files_not_found files were missing expected outputs"
echo "---------------------------------"
echo "$total_ran TOTAL TESTS RAN"
echo "---------------------------------"

echo "YOUR SCORE IS v"
echo $(($total_ran-$incorrect))/$(($total_ran))*100 | bc -l
