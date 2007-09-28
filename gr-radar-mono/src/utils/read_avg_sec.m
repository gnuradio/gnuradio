function avg = read_avg_sec(name, vlen)

    f = fopen(name, "rb");
    s = zeros(1, vlen);
    n = 0;
    m = 0;
        
    while (!feof(f))
       t = fread(f, [2, vlen], "float");
       if (size(t) == [2, vlen])
          n = n+1;
          c = t(1,:)+t(2,:)*j;
          s = s+c;
	  m = m+1;	  
	  if (m == 1000)
	     avg(n/1000,:) = s/1000;
	     s = zeros(1, vlen);
	     m = 0;
	  endif
       endif
    endwhile
    
    fclose(f);

endfunction
