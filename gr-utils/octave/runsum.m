function r = runsum(x)
  len = length(x);
  r = zeros (1, len);
  r(1) = x(1);
  for i = 2:len;
    r(i) = r(i-1) + x(i);
  endfor;
endfunction;

