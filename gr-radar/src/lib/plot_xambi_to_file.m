function plot_xambi_to_file(amb, filename)
  [nr, nc] = size(amb);
  ndoppler_bins = nr
  nranges = nc

  %colormap(cool(256));
  %colormap(rainbow(256));
  colormap(gray(1024));

  %d = 10*log10(amb);
  d = amb;
  min(min(d))
  max(max(d))
  b = imagesc(d, 4.0);
  saveimage(filename, b, "ppm")

endfunction;
