for currentCount in {1..3}
do
    ./mylang2ir canCases/inputs/$currentCount.my
    rm -f canCases/outputs/$currentCount.txt
    lli canCases/inputs/$currentCount.ll >> canCases/outputs/$currentCount.txt    
done