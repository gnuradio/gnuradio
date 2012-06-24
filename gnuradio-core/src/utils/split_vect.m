% split vector into packets

function y = split_vect(vect,N)
  Z = floor(max(size(vect))/N);
  y = [];
  if(size(vect)(1)>size(vect)(2))
    v = vect';
  else
    v = vect;
  end
  for i=1:Z
    y(i,1:N) = v((i-1)*N+1:i*N);
  end
end

