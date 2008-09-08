if test -d worknc
then
echo worknc is ready
else
mkdir worknc
echo worknc is created
fi

ncvlog -f vlog.list -logfile ../log/ncvlog.log

if test $? -ne 0
then
echo compiling err occured...
exit 1
fi

ncelab work.tb_top -NEVERWARN -loadvpi ip_32W_gen:PLI_register -loadvpi ip_32W_check:PLI_register -snapshot work:snap -timescale 1ns/1ps -message -access +rw -logfile ../log/ncelab.log

if test $? -ne 0
then
echo ncelab err occured...
exit 1
fi

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
