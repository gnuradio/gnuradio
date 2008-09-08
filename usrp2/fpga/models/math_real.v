/*
 * This is a general recreation of the VHDL ieee.math_real package.
 */
 
module math_real ;
   // Constants for use below and for general reference
   // TODO: Bring it out to 12 (or more???) places beyond the decimal?
   localparam MATH_E            =  2.7182818284;
   localparam MATH_1_OVER_E     =  0.3678794411;
   localparam MATH_PI           =  3.1415926536;
   localparam MATH_2_PI         =  6.2831853071;
   localparam MATH_1_OVER_PI    =  0.3183098861;
   localparam MATH_PI_OVER_2    =  1.5707963267;
   localparam MATH_PI_OVER_3    =  1.0471975511;
   localparam MATH_PI_OVER_4    =  0.7853981633;
   localparam MATH_3_PI_OVER_2  =  4.7123889803;
   localparam MATH_LOG_OF_2     =  0.6931471805;
   localparam MATH_LOG_OF_10    =  2.3025850929;
   localparam MATH_LOG2_OF_E    =  1.4426950408;
   localparam MATH_LOG10_OF_E   =  0.4342944819;
   localparam MATH_SQRT_2       =  1.4142135623;
   localparam MATH_1_OVER_SQRT_2=  0.7071067811;
   localparam MATH_SQRT_PI      =  1.7724538509;
   localparam MATH_DEG_TO_RAD   =  0.0174532925;
   localparam MATH_RAD_TO_DEG   =  57.2957795130;

   // The number of iterations to do for the Taylor series approximations
   localparam EXPLOG_ITERATIONS =  19;
   localparam COS_ITERATIONS    =  8;
   
   /* Conversion Routines */
    
   // Return the sign of a particular number.
   function real sign ;
      input real x ;
      begin
         sign = x < 0.0 ? 1.0 : 0.0 ;
      end
   endfunction
   
   // Return the trunc function of a number
   function real trunc ;
      input real x ;
      begin
         trunc = x - mod(x,1.0) ;
      end
   endfunction
   
   // Return the ceiling function of a number.
   function real ceil ;
      input real x ;
      real retval ;
      begin
         retval = mod(x,1.0) ;
         if( retval != 0.0 && x > 0.0 )  retval = x+1.0 ;
         else retval = x ;
         ceil = trunc(retval) ;
      end
   endfunction
   
   // Return the floor function of a number
   function real floor ;
      input real x ;
      real retval ;
      begin
         retval = mod(x,1.0) ;
         if( retval != 0.0 && x < 0.0 ) retval = x - 1.0 ;
         else retval = x ;
         floor = trunc(retval) ;
      end
   endfunction
   
   // Return the round function of a number
   function real round ;
      input real x ;
      real retval ;
      begin
         retval = x > 0.0 ? x + 0.5 : x - 0.5 ;
         round = trunc(retval) ;
      end
   endfunction
   
   // Return the fractional remainder of (x mod m)
   function real mod ;
      input real x ;
      input real m ;
      real retval ;
      begin
         retval = x ;
         if( retval > m ) begin
            while( retval > m ) begin
               retval = retval - m ;
            end
         end
         else begin
            while( retval < -m ) begin
               retval = retval + m ;
            end
         end
         mod = retval ;
      end
   endfunction
   
   // Return the max between two real numbers
   function real realmax ;
      input real x ;
      input real y ;
      begin
         realmax = x > y ? x : y ;
      end
   endfunction
   
   // Return the min between two real numbers
   function real realmin ;
      input real x ;
      input real y ;
      begin
         realmin = x > y ? y : x ;
      end
   endfunction
   
   /* Random Numbers */
   
   // Generate Gaussian distributed variables
   function real gaussian ;
      input real mean ;
      input real var ;
      real u1, u2, v1, v2, s ;
      begin
         s = 1.0 ;
         while( s >= 1.0  ) begin
            // Two random numbers between 0 and 1
            u1 = $random/4294967296.0 + 0.5 ;
            u2 = $random/4294967296.0 + 0.5 ;
            // Adjust to be between -1,1
            v1 = 2*u1-1.0 ;
            v2 = 2*u2-1.0 ;
            // Polar mag squared
            s = (v1*v1 + v2*v2) ;
         end
         gaussian = mean + sqrt((-2.0*log(s))/s) * v1 * sqrt(var) ;
         // gaussian2 = mean + sqrt(-2*log(s)/s)*v2 * sqrt(var) ;
      end
   endfunction
   
   /* Roots and Log Functions */
   
   // Return the square root of a number
   function real sqrt ;
      input real x ;
      real retval ;
      begin
         sqrt = (x == 0.0) ? 0.0 : powr(x,0.5) ;
      end
   endfunction
   
   // Return the cube root of a number
   function real cbrt ;
      input real x ;
      real retval ;
      begin
         cbrt = (x == 0.0) ? 0.0 : powr(x,1.0/3.0) ;
      end
   endfunction
   
   // Return the absolute value of a real value
   function real abs ;
      input real x ;
      begin
         abs = (x > 0.0) ? x : -x ;
      end
   endfunction 
   
   // Return a real value raised to an integer power
   function real pow ;
      input real b ;
      input integer x ;
      integer absx ;
      real retval ;
      begin
         retval = 1.0 ;
         absx = abs(x) ;
         repeat(absx) begin
            retval = b*retval ;
         end
         pow = x < 0 ? (1.0/retval) : retval ;
      end
   endfunction
   
   // Return a real value raised to a real power
   function real powr ;
      input real b ;
      input real x ;
      begin
         powr = exp(x*log(b)) ;
      end
   endfunction
   
   // Return the evaluation of e^x where e is the natural logarithm base
   // NOTE: This is the Taylor series expansion of e^x
   function real exp ;
      input real x ;
      real retval ;
      integer i ;
      real nm1_fact ;
      real powm1 ;
      begin
         nm1_fact = 1.0 ;
         powm1 = 1.0 ;
         retval = 1.0 ;
         for( i = 1 ; i < EXPLOG_ITERATIONS ; i = i + 1 ) begin
            powm1 = x*powm1 ;
            nm1_fact = nm1_fact * i ;
            retval = retval + powm1/nm1_fact ;
         end
         exp = retval ;
      end
   endfunction
   
   // Return the evaluation log(x)
   function real log ;
      input real x ;
      integer i ;
      real whole ;
      real xm1oxp1 ;
      real retval ;
      real newx ;
      begin
         retval = 0.0 ;
         whole = 0.0 ;
         newx = x ;
         while( newx > MATH_E ) begin
            whole = whole + 1.0 ;
            newx = newx / MATH_E ;
         end
         xm1oxp1 = (newx-1.0)/(newx+1.0) ;
         for( i = 0 ; i < EXPLOG_ITERATIONS ; i = i + 1 ) begin
            retval = retval + pow(xm1oxp1,2*i+1)/(2.0*i+1.0) ;
         end
         log = whole+2.0*retval ;
      end
   endfunction
   
   // Return the evaluation ln(x) (same as log(x))
   function real ln ;
      input real x ;
      begin
         ln = log(x) ;
      end
   endfunction
   
   // Return the evaluation log_2(x)
   function real log2 ;
      input real x ;
      begin
         log2 = log(x)/MATH_LOG_OF_2 ;
      end
   endfunction
   
   function real log10 ;
      input real x ;
      begin
         log10 = log(x)/MATH_LOG_OF_10 ;
      end
   endfunction
   
   function real log_base ;
      input real x ;
      input real b ;
      begin
         log_base = log(x)/log(b) ;
      end
   endfunction
   
   /* Trigonometric Functions */
   
   // Internal function to reduce a value to be between [-pi:pi]
   function real reduce ;
      input real x ;
      real retval ;
      begin
         retval = x ;
         while( abs(retval) > MATH_PI ) begin
            retval = retval > MATH_PI ? 
                     (retval - MATH_2_PI) : 
                     (retval + MATH_2_PI) ;
         end
         reduce = retval ;
      end
   endfunction
   
   // Return the cos of a number in radians
   function real cos ;
      input real x ;
      integer i ;
      integer sign ;
      real newx ;
      real retval ;
      real xsqnm1 ;
      real twonm1fact ;
      begin
         newx = reduce(x) ;
         xsqnm1 = 1.0 ;
         twonm1fact = 1.0 ;
         retval = 1.0 ;
         for( i = 1 ; i < COS_ITERATIONS ; i = i + 1 ) begin
            sign = -2*(i % 2)+1 ;
            xsqnm1 = xsqnm1*newx*newx ;
            twonm1fact = twonm1fact * (2.0*i) * (2.0*i-1.0) ;
            retval = retval + sign*(xsqnm1/twonm1fact) ; 
         end
         cos = retval ;
      end
   endfunction
   
   // Return the sin of a number in radians
   function real sin ;
      input real x ;
      begin
         sin = cos(x - MATH_PI_OVER_2) ;
      end
   endfunction
   
   // Return the tan of a number in radians
   function real tan ;
      input real x ;
      begin
         tan = sin(x) / cos(x) ;
      end
   endfunction
   
   // Return the arcsin in radians of a number
   function real arcsin ;
      input real x ;
      begin
         arcsin = 2.0*arctan(x/(1.0+sqrt(1.0-x*x))) ;
      end
   endfunction
   
   // Return the arccos in radians of a number
   function real arccos ;
      input real x ;
      begin
         arccos = MATH_PI_OVER_2-arcsin(x) ;
      end
   endfunction
   
   // Return the arctan in radians of a number
   // TODO: Make sure this REALLY does work as it is supposed to!
   function real arctan ;
      input real x ;
      real retval ;
      real y ;
      real newx ;
      real twoiotwoip1 ;
      integer i ;
      integer mult ;
      begin
         retval = 1.0 ;
         twoiotwoip1 = 1.0 ;
         mult = 1 ;
         newx = abs(x) ;
         while( newx > 1.0 ) begin
            mult = mult*2 ;
            newx = newx/(1.0+sqrt(1.0+newx*newx)) ;
         end
         y = 1.0 ;
         for( i = 1 ; i < 2*COS_ITERATIONS ; i = i + 1 ) begin
            y = y*((newx*newx)/(1+newx*newx)) ;
            twoiotwoip1 = twoiotwoip1 * (2.0*i)/(2.0*i+1.0) ;
            retval = retval + twoiotwoip1*y ;
         end
         retval = retval * (newx/(1+newx*newx)) ;
         retval = retval * mult ;
         
         arctan = (x > 0.0) ? retval : -retval ;
      end
   endfunction
   
   // Return the arctan in radians of a ratio x/y
   // TODO: Test to make sure this works as it is supposed to!
   function real arctan_xy ;
      input real x ;
      input real y ;
      real retval ;
      begin
         retval = 0.0 ;
         if( x < 0.0 ) retval = MATH_PI - arctan(-abs(y)/x) ;
         else if( x > 0.0 ) retval = arctan(abs(y)/x) ;
         else if( x == 0.0 ) retval = MATH_PI_OVER_2 ;
         arctan_xy = (y < 0.0) ? -retval : retval ;
      end
   endfunction
   
   /* Hyperbolic Functions */
   
   // Return the sinh of a number
   function real sinh ;
      input real x ;
      begin
         sinh = (exp(x) - exp(-x))/2.0 ;
      end
   endfunction
   
   // Return the cosh of a number
   function real cosh ;
      input real x ;
      begin
         cosh = (exp(x) + exp(-x))/2.0 ;
      end
   endfunction
   
   // Return the tanh of a number
   function real tanh ;
      input real x ;
      real e2x ;
      begin
         e2x = exp(2.0*x) ;
         tanh = (e2x+1.0)/(e2x-1.0) ;
      end
   endfunction
   
   // Return the arcsinh of a number
   function real arcsinh ;
      input real x ;
      begin
         arcsinh = log(x+sqrt(x*x+1.0)) ;
      end
   endfunction
   
   // Return the arccosh of a number
   function real arccosh ;
      input real x ;
      begin
         arccosh = ln(x+sqrt(x*x-1.0)) ;
      end
   endfunction
   
   // Return the arctanh of a number
   function real arctanh ;
      input real x ;
      begin
         arctanh = 0.5*ln((1.0+x)/(1.0-x)) ;
      end
   endfunction
   /*
    initial begin
    $display( "cos(MATH_PI_OVER_3): %f", cos(MATH_PI_OVER_3) ) ;
    $display( "sin(MATH_PI_OVER_3): %f", sin(MATH_PI_OVER_3) ) ;
    $display( "sign(-10): %f", sign(-10) ) ;
    $display( "realmax(MATH_PI,MATH_E): %f", realmax(MATH_PI,MATH_E) ) ;
    $display( "realmin(MATH_PI,MATH_E): %f", realmin(MATH_PI,MATH_E) ) ;
    $display( "mod(MATH_PI,MATH_E): %f", mod(MATH_PI,MATH_E) ) ;
    $display( "ceil(-MATH_PI): %f", ceil(-MATH_PI) ) ;
    $display( "ceil(4.0): %f", ceil(4.0) ) ;
    $display( "ceil(3.99999999999999): %f", ceil(3.99999999999999) ) ;
    $display( "pow(MATH_PI,2): %f", pow(MATH_PI,2) ) ;
    $display( "gaussian(1.0,1.0): %f", gaussian(1.0,1.0) ) ;
    $display( "round(MATH_PI): %f", round(MATH_PI) ) ;
    $display( "trunc(-MATH_PI): %f", trunc(-MATH_PI) ) ;
    $display( "ceil(-MATH_PI): %f", ceil(-MATH_PI) ) ;
    $display( "floor(MATH_PI): %f", floor(MATH_PI) ) ;
    $display( "round(e): %f", round(MATH_E)) ;
    $display( "ceil(-e): %f", ceil(-MATH_E)) ;
    $display( "exp(MATH_PI): %f", exp(MATH_PI) ) ;
    $display( "log2(MATH_PI): %f", log2(MATH_PI) ) ;
    $display( "log_base(pow(2,32),2): %f", log_base(pow(2,32),2) ) ;
    $display( "ln(0.1): %f", log(0.1) ) ;
    $display( "cbrt(7): %f", cbrt(7) ) ;
    $display( "cos(MATH_2_PI): %f", cos(20*MATH_2_PI) ) ;
    $display( "sin(-MATH_2_PI): %f", sin(-50*MATH_2_PI) ) ;
    $display( "sinh(MATH_E): %f", sinh(MATH_E) ) ;
    $display( "cosh(MATH_2_PI): %f", cosh(MATH_2_PI) ) ;
    $display( "arctan_xy(-4,3): %f", arctan_xy(-4,3) ) ;
    $display( "arctan(MATH_PI): %f", arctan(MATH_PI) ) ;
    $display( "arctan(-MATH_E/2): %f", arctan(-MATH_E/2) ) ;
    $display( "arctan(MATH_PI_OVER_2): %f", arctan(MATH_PI_OVER_2) ) ;
    $display( "arctan(1/7) = %f", arctan(1.0/7.0) ) ;
    $display( "arctan(3/79) = %f", arctan(3.0/79.0) ) ;
    $display( "pi/4 ?= %f", 5*arctan(1.0/7.0)+2*arctan(3.0/79.0) ) ;
    $display( "arcsin(1.0): %f", arcsin(1.0) ) ;
    $display( "cos(pi/2): %f", cos(MATH_PI_OVER_2)) ;
    $display( "arccos(cos(pi/2)): %f", arccos(cos(MATH_PI_OVER_2)) ) ;
    $display( "cos(0): %f", cos(0) ) ;
    $display( "cos(MATH_PI_OVER_4): %f", cos(MATH_PI_OVER_4) ) ;
    $display( "cos(MATH_PI_OVER_2): %f", cos(MATH_PI_OVER_2) ) ;
    $display( "cos(3*MATH_PI_OVER_4): %f", cos(3*MATH_PI_OVER_4) ) ;
    $display( "cos(MATH_PI): %f", cos(MATH_PI) ) ;
    $display( "cos(5*MATH_PI_OVER_4): %f", cos(5*MATH_PI_OVER_4) ) ;
    $display( "cos(6*MATH_PI_OVER_4): %f", cos(6*MATH_PI_OVER_4) ) ;
    $display( "cos(7*MATH_PI_OVER_4): %f", cos(7*MATH_PI_OVER_4) ) ;
    $display( "cos(8*MATH_PI_OVER_4): %f", cos(8*MATH_PI_OVER_4) ) ;
    end*/
   
endmodule
