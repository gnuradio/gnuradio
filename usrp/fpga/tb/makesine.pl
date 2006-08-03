#!/usr/bin/perl

$angle = 0;
$angle_inc = 2*3.14159/87.2;
$amp = 1;
$amp_rate = 1.0035;
for($i=0;$i<3500;$i++)
  {
    printf("@(posedge clk);xi<= #1 16'h%x;yi<= #1 16'h%x;\n",65535&int($amp*cos($angle)),65535&int($amp*sin($angle)));
    $angle += $angle_inc;
    $amp *= $amp_rate;
  }

printf("\$finish;\n");
