loadProjectFile -file "C:\cygwin\home\matt\usrp2\fpga\boot_cpld/boot_cpld.ipf"
setMode -ss
setMode -sm
setMode -hw140
setMode -spi
setMode -acecf
setMode -acempm
setMode -pff
setMode -bs
setMode -bs
setMode -bs
setMode -bs
setCable -port auto
Identify 
identifyMPM 
assignFile -p 1 -file "C:/cygwin/home/matt/usrp2/fpga/boot_cpld/boot_cpld.jed"
Program -p 1 -e -v -defaultVersion 0 
Program -p 1 -e -v -defaultVersion 0 
Program -p 1 -e -v -defaultVersion 0 
Program -p 1 -e -v -defaultVersion 0 
Program -p 1 -e -v -defaultVersion 0 
Program -p 1 -e -v -defaultVersion 0 
Identify 
identifyMPM 
Identify 
identifyMPM 
Identify 
identifyMPM 
Identify 
identifyMPM 
Identify 
identifyMPM 
setMode -bs
deleteDevice -position 1
