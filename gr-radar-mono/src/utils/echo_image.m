function echo_image(filename, data)
    d = abs(data);
    d = d-min(min(d));
    d = d/(max(max(d)))*255;
    pngwrite(filename, d, d, d, ones(size(d)));
endfunction