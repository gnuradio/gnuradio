mf = read_float_binary('ofdm_sync_pn-mf_f.dat');
theta_pn = read_float_binary('ofdm_sync_pn-theta_f.dat');
peaks_pn = read_char_binary('ofdm_sync_pn-peaks_b.dat');
regen_pn = read_char_binary('ofdm_sync_pn-regen_b.dat');
angle_pn = read_float_binary('ofdm_sync_pn-epsilon_f.dat');

plot(mf, 'k')
hold
plot(theta_pn, 'g');
plot(peaks_pn, 'r');
plot(regen_pn, 'b');
xlim([100, 50000]);
ylim([0, 1])
i = find(peaks_pn);
i(100:200)
hold off

ipeaks = find(peaks_pn);
dpeaks = diff(ipeaks);
hist(dpeaks, 30)
set(gca, 'FontSize', 30, 'FontWeight', 'Bold');
