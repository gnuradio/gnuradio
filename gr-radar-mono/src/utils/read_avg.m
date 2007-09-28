function avg = read_avg(name, vlen)

    f = fopen(name, "rb");
    s = zeros(1, vlen);
    n = 0;
        
    while (!feof(f))
       t = fread(f, [2, vlen], "float");
       if (size(t) == [2, vlen])
          n = n+1;
          c = t(1,:)+t(2,:)*j;
	  if (n > 10)
             s = s+c;
	  endif
       endif
    endwhile

    avg = s/(n-1);
    
    fclose(f);
    
endfunction
