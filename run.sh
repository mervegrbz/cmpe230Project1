make
for currentCount in {0..18}
do
    if [[ $currentCount -eq 16 ]]; then
      continue
    fi
    ./mylang2ir deliverables/inputs/testcase$currentCount.my
    diff -s  deliverables/outputs/testcase$currentCount.txt <(lli deliverables/inputs/testcase$currentCount.ll)
done