# Center and zero pad v to length n
function pad = czpad(v, n)
    c = n/2;
    l = length(v);
    pad(c-l/2+1:c+l/2) = v;
    pad(c+l/2+1:n) = 0;
endfunction
 