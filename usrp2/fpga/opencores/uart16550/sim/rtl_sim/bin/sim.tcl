database -open waves -into ../out/uart -default 
probe -create -shm uart_test -all -depth all
stop -create -time 1000000000ns -relative
run
quit
