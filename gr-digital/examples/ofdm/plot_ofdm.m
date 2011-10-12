function plot_ofdm(fft_size, occ_tones)

ofdm = read_complex_binary('ofdm_frame_acq_c.dat');
ofdm_split = split_vect(ofdm, occ_tones);

ofdm_derot = read_complex_binary('ofdm_frame_sink_c.dat');
ofdm_derot_split = split_vect(ofdm_derot, occ_tones);

fftc = read_complex_binary('fft_out_c.dat');
fftc_split = split_vect(fftc, fft_size);

size(ofdm_split)
size(ofdm_derot_split)
disp "DEROTATED SPLIT"
ofdm_derot(1:100)


figure(1)
#set(gcf, 'Position', [50 50 1000 600]);

a = size(ofdm_split);
if nargin == 3
    maxcount = count;
    if maxcount > a(1)
        error('plot_ofdm:tolong', 'Requested count size exceeds size of vectors');
    end
else
    maxcount = a(1);
end

for i = 1:size(ofdm_split)[0]
    x = ofdm_split(i,:);
    y = fftc_split(i+1,:);
    
    subplot(2,2,1);
    plot(real(x), imag(x), 'bo');
    #set(gca, 'FontSize', 30, 'FontWeight', 'Bold');
    axis([-1.5, 1.5, -1.5, 1.5])
    #title('I&Q Constellation', 'FontSize', 36);
    #xlabel('Inphase', 'FontSize', 32);
    #ylabel('Quadrature', 'FontSize', 32);
    
    subplot(2,2,3);
    plot(angle(x*j), 'bo');
    #set(gca, 'FontSize', 30, 'FontWeight', 'Bold');
    axis([0, occ_tones, -3.5, 3.5])
    #title('Equalized Symbol Angle', 'FontSize', 36);
    #xlabel('Bin Number (Occ. Tones)', 'FontSize', 32);
    #ylabel('Symbol Angle', 'FontSize', 32);
    
    subplot(2,2,2);
    plot(angle(y*j), 'bo');
    #set(gca, 'FontSize', 30, 'FontWeight', 'Bold');
    axis([0, fft_size, -3.5, 3.5]) 
    #title('Unequalized Symbol Angle', 'FontSize', 36);
    #xlabel('Bin Number (FFT Size)', 'FontSize', 32);
    #ylabel('Symbol Angle', 'FontSize', 32);
    
    Y = 20*log10(abs(y) ./ max(abs(y)));
    subplot(2,2,4);
    plot(Y, 'b-');
    #set(gca, 'FontSize', 30, 'FontWeight', 'Bold');
    axis([0, fft_size, -50, 1]);
    #title('Frequency Domain of Unequalized Rx', 'FontSize', 36);
    #xlabel('Bin Number (FFT Size)', 'FontSize', 32);
    #ylabel('Power (dB)', 'FontSize', 32);
    
    #N = 20*log10(var(abs(x)-1));
    
    disp(sprintf('Symbol Number: %d\n', i))
    #disp(sprintf('\tFreq Error: %f\n', anglesh_pn(1+(i-1)*fft_size)))
    pause

end
