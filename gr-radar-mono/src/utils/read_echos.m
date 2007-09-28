# Read echos from file into array
function echos = read_echos(filename, len, drop)
    e = read_complex_binary(filename);
    n = length(e)/len-drop;
    start = drop*len+1;
    echos = reshape(e(start:end), len, n).';
endfunction
