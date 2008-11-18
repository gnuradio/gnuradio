This is an example of using gr-trellis in grc.

Two users are transmitting simultaneously using convolutionally encoded QPSK, each with power P1=alpha*P and P2=(1-alpha)*P.
The combined signal is observed in noise and four different receivers are considered:
1) A viterbi decoder decoding user 1 assuming user 2 is noise
2) A viterbi decoder decoding user 2 assuming user 1 is noise
3) A viterbi decoder decoding user 1 first 
   and then reencoding this signal, subtracting it from the observation 
   and then running a Viterbi decoder decoding user 2
4) A viterbi decoder decoding user 2 first 
   and then reencoding this signal, subtracting it from the observation 
   and then running a Viterbi decoder decoding user 1

You can change the signal to noise ratio P/sigma^2 and the allocation of power to the two users, alpha.

Enjoy.
