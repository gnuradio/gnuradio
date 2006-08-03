filename="xambi.out"

ambdata=read_float_binary(filename);

fftsize=512
nranges = prod(size(ambdata))/fftsize

ambdata=reshape(ambdata,fftsize,nranges);
% ambdata=fftshift(ambdata,1);

% colormap(cool(256));
colormap(rainbow(256));

d = 10*log10(ambdata);
min(min(d))
max(max(d))
imagesc(d, 2.0);





















