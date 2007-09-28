function avg = calc_avg(rlen, mintime)
    avg = read_avg('echos.dat', rlen);
    x = 1:rlen;
    x = (x/64e6+mintime)*3e8/2;
    plot(x, abs(avg), "^;Amplitude;");
    xlabel("Range (meters)");
    axis([max(x) 0])
endfunction
