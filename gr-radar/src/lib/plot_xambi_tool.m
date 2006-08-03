#!/usr/bin/env octave

xambi_filename = argv{1};
ppm_filename = argv{2};

a = read_xambi(xambi_filename);
plot_xambi_to_file(a, ppm_filename);

