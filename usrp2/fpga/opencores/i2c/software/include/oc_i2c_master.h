/*
/////////////////////////////////////////////////////////////////////
////                                                             ////
////  Include file for OpenCores I2C Master core                 ////
////                                                             ////
////  File    : oc_i2c_master.h                                  ////
////  Function: c-include file                                   ////
////                                                             ////
////  Authors: Richard Herveille (richard@asics.ws)              ////
////           Filip Miletic                                     ////
////                                                             ////
////           www.opencores.org                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2001 Richard Herveille                        ////
////                    Filip Miletic                            ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
*/

/*
 * Definitions for the Opencores i2c master core
 */

/* --- Definitions for i2c master's registers --- */
	
/* ----- Read-write access                                            */

#define OC_I2C_PRER_LO 0x00     /* Low byte clock prescaler register  */	
#define OC_I2C_PRER_HI 0x01     /* High byte clock prescaler register */	
#define OC_I2C_CTR     0x02     /* Control register                   */	
										
/* ----- Write-only registers                                         */
										
#define OC_I2C_TXR     0x03     /* Transmit byte register             */	
#define OC_I2C_CR      0x04     /* Command register                   */	
	
/* ----- Read-only registers                                          */
										
#define OC_I2C_RXR     0x03     /* Receive byte register              */
#define OC_I2C_SR      0x04     /* Status register                    */
	
/* ----- Bits definition                                              */
	
/* ----- Control register                                             */
	
#define OC_I2C_EN (1<<7)        /* Core enable bit:                   */
                                /*      1 - core is enabled           */
                                /*      0 - core is disabled          */
#define OC_I2C_IEN (1<<6)       /* Interrupt enable bit               */
                                /*      1 - Interrupt enabled         */
                                /*      0 - Interrupt disabled        */
                                /* Other bits in CR are reserved      */

/* ----- Command register bits                                        */
 
#define OC_I2C_STA (1<<7)       /* Generate (repeated) start condition*/
#define OC_I2C_STO (1<<6)       /* Generate stop condition            */
#define OC_I2C_RD  (1<<5)       /* Read from slave                    */
#define OC_I2C_WR  (1<<4)       /* Write to slave                     */
#define OC_I2C_ACK (1<<3)       /* Acknowledge from slave             */
                                /*      1 - ACK                       */
                                /*      0 - NACK                      */
#define OC_I2C_IACK (1<<0)      /* Interrupt acknowledge              */

/* ----- Status register bits                                         */

#define OC_I2C_RXACK (1<<7)     /* ACK received from slave            */
                                /*      1 - ACK                       */
                                /*      0 - NACK                      */
#define OC_I2C_BUSY  (1<<6)     /* Busy bit                           */
#define OC_I2C_TIP   (1<<1)     /* Transfer in progress               */
#define OC_I2C_IF    (1<<0)     /* Interrupt flag                     */

/* bit testing and setting macros                                     */

#define OC_ISSET(reg,bitmask)       ((reg)&(bitmask))
#define OC_ISCLEAR(reg,bitmask)     (!(OC_ISSET(reg,bitmask)))
#define OC_BITSET(reg,bitmask)      ((reg)|(bitmask))
#define OC_BITCLEAR(reg,bitmask)    ((reg)|(~(bitmask)))
#define OC_BITTOGGLE(reg,bitmask)   ((reg)^(bitmask))
#define OC_REGMOVE(reg,value)       ((reg)=(value))