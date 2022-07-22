TIMEFORMAT=%R
total=0.0
rm sandboxed_net_out sandboxed_net_time.txt
make run_sandboxed_client
for i in {1..100}
do
    (time make run_sandboxed_client > /dev/null) 2> out
    runtime=$(cat out)
    runtime=$(expr $runtime)
    res=$(bc <<< "scale=5; $total + $runtime")
    total=$(expr $res)
    echo $runtime >> sandboxed_net_time.txt
    echo "Iteration $i"
done

echo "The total time a sandboxed simplenet client connection takes is: $total" >> sandboxed_net_out
res=$(bc <<< "scale=5; $total / 100")
total=$(expr $res)
echo "The average time a sandboxed simplenet client connection takes is: $total" >> sandboxed_net_out
rm out