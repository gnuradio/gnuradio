These are examples of using gr-trellis in grc.

INTERFERENCE CANCELLATION
-------------------------
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


Serially Concatenated Convolutional Codes
-----------------------------------------
An SCCC can be defined by an outer and an inner FSM together with an interleaver
and a modulation type. You can change the SNR and observe the estimated BER.
In sccc.grc the decoding and metric calculation are combined; in sccc1.grc they are separate.

Parallel Concatenated Convolutional Codes
-----------------------------------------
A PCCC can be defined by two FSMs together with an interleaver
and a modulation type. You can change the SNR and observe the estimated BER.
In pccc1.grc the decoding and metric calculation are separate.


Enjoy.
