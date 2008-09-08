if test -f ../log/.sim_failed
then
	echo "../log/.sim_failed log file has been removed!"
	rm ../log/.sim_failed
fi

if test -f ../log/sim_succeed
then
	echo "../log/sim_succeed log file has been removed!"
	rm ../log/sim_succeed
fi

ncsim work:snap $1 -UNBUFFERED -logfile ../log/ncsim.log -NOKEY 
