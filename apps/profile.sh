TIMEFORMAT=%R
total=0.0
rm net_out net_time.txt
make run_simplenet_client
for i in {1..100}
do
    (time make run_simplenet_client > /dev/null) 2> out
    runtime=$(cat out)
    rm out
    runtime=$(expr $runtime)
    res=$(bc <<< "scale=5; $total + $runtime")
    total=$(expr $res)
    echo $runtime >> net_time.txt
    echo "Iteration $i"
done

echo "The total time a simplenet client connection takes is: $total" >> net_out
res=$(bc <<< "scale=5; $total / 100")
total=$(expr $res)
echo "The average time a simplenet client connection takes is: $total" >> net_out
