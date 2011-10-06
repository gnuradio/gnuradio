"""
Copyright 2011 Free Software Foundation, Inc.

This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

MAIN_TMPL = """\
<?xml version="1.0"?>
<block>
	<name>SHD: SMINI $sourk.title()</name>
	<key>shd_smini_$(sourk)</key>
	<import>from gnuradio import shd</import>
	<make>shd.smini_$(sourk)(
	device_addr=\$dev_addr,
	io_type=shd.io_type.\$type.type,
	num_channels=\$nchan,
)
\#if \$clock_rate()
self.\$(id).set_clock_rate(\$clock_rate, shd.ALL_MBOARDS)
\#end if
#for $m in range($max_mboards)
########################################################################
\#if \$num_mboards() > $m and \$ref_source$(m)() == 'external'
self.\$(id).set_clock_config(shd.clock_config.external(), $m)
\#end if
########################################################################
\#if \$num_mboards() > $m and \$ref_source$(m)() == 'internal'
self.\$(id).set_clock_config(shd.clock_config.internal(), $m)
\#end if
########################################################################
\#if \$num_mboards() > $m and \$ref_source$(m)() == 'mimo'
_config = shd.clock_config()
_config.ref_source = shd.clock_config.REF_MIMO
_config.pps_source = shd.clock_config.PPS_MIMO
self.\$(id).set_clock_config(_config, $m)
\#end if
########################################################################
\#if \$num_mboards() > $m and \$sd_spec$(m)()
self.\$(id).set_subdev_spec(\$sd_spec$(m), $m)
\#end if
########################################################################
#end for
\#if \$sync()
self.\$(id).set_time_unknown_pps(shd.time_spec())
\#end if
self.\$(id).set_samp_rate(\$samp_rate)
#for $n in range($max_nchan)
\#if \$nchan() > $n
self.\$(id).set_center_freq(\$center_freq$(n), $n)
self.\$(id).set_gain(\$gain$(n), $n)
\#end if
#end for
</make>
	<callback>set_samp_rate(\$samp_rate)</callback>
	#for $n in range($max_nchan)
	<callback>set_center_freq(\$center_freq$(n), $n)</callback>
	<callback>set_gain(\$gain$(n), $n)</callback>
	#end for
	<param>
		<name>$(direction.title())put Type</name>
		<key>type</key>
		<type>enum</type>
		<option>
			<name>Complex</name>
			<key>complex</key>
			<opt>type:COMPLEX_FLOAT32</opt>
			<opt>vlen:1</opt>
		</option>
		<option>
			<name>Short</name>
			<key>short</key>
			<opt>type:COMPLEX_INT16</opt>
			<opt>vlen:2</opt>
		</option>
	</param>
	<param>
		<name>Device Addr</name>
		<key>dev_addr</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if \$dev_addr()
				none
			\#else
				part
			\#end if
		</hide>
	</param>
	<param>
		<name>Sync</name>
		<key>sync</key>
		<value></value>
		<type>enum</type>
		<hide>\#if \$sync() then 'none' else 'part'#</hide>
		<option>
			<name>unknown PPS</name>
			<key>sync</key>
		</option>
		<option>
			<name>don't sync</name>
			<key></key>
		</option>
	</param>
	<param>
		<name>Clock Rate (Hz)</name>
		<key>clock_rate</key>
		<value>0.0</value>
		<type>real</type>
		<hide>\#if \$clock_rate() then 'none' else 'part'#</hide>
		<option>
			<name>Default</name>
			<key>0.0</key>
		</option>
	</param>
	<param>
		<name>Num Mboards</name>
		<key>num_mboards</key>
		<value>1</value>
		<type>int</type>
		<hide>part</hide>
		#for $m in range(1, $max_mboards+1)
		<option>
			<name>$(m)</name>
			<key>$m</key>
		</option>
		#end for
	</param>
	#for $m in range($max_mboards)
	<param>
		<name>Mb$(m): Ref Source</name>
		<key>ref_source$(m)</key>
		<value></value>
		<type>enum</type>
		<hide>
			\#if not \$num_mboards() > $m
				all
			\#elif \$ref_source$(m)()
				none
			\#else
				part
			\#end if
		</hide>
		<option><name>Default</name><key></key></option>
		<option><name>Internal</name><key>internal</key></option>
		<option><name>External</name><key>external</key></option>
		<option><name>MIMO Cable</name><key>mimo</key></option>
	</param>
	<param>
		<name>Mb$(m): Subdev Spec</name>
		<key>sd_spec$(m)</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if not \$num_mboards() > $m
				all
			\#elif \$sd_spec$(m)()
				none
			\#else
				part
			\#end if
		</hide>
	</param>
	#end for
	<param>
		<name>Num Channels</name>
		<key>nchan</key>
		<value>1</value>
		<type>int</type>
		#for $n in range(1, $max_nchan+1)
		<option>
			<name>$(n)</name>
			<key>$n</key>
		</option>
		#end for
	</param>
	<param>
		<name>Samp Rate (Sps)</name>
		<key>samp_rate</key>
		<value>samp_rate</value>
		<type>real</type>
	</param>
	$params
	<check>$max_nchan >= \$nchan</check>
	<check>\$nchan > 0</check>
	<check>$max_mboards >= \$num_mboards</check>
	<check>\$num_mboards > 0</check>
	<check>\$nchan >= \$num_mboards</check>
	<$sourk>
		<name>$direction</name>
		<type>\$type</type>
		<vlen>\$type.vlen</vlen>
		<nports>\$nchan</nports>
	</$sourk>
	<doc>
The SHD SMINI $sourk.title() Block:

Device Address:
The device address is a delimited string used to locate SHD devices on your system. \\
If left blank, the first SHD device found will be used. \\
Use the device address to specify a specific device or list of devices.
SMINI1 Example: serial=12345678
SMINI2 Example: type=xmini

Num Motherboards:
Selects the number of SMINI motherboards in this device configuration.

Reference Source:
Where the motherboard should sync its time and clock references.
If source and sink blocks reference the same device,
it is only necessary to set the reference source on one of the blocks.

Subdevice specification:
Each motherboard should have its own subdevice specification \\
and all subdevice specifications should be the same length. \\
Select the subdevice or subdevices for each channel using a markup string. \\
The markup string consists of a list of dboard_slot:subdev_name pairs (one pair per channel). \\
If left blank, the SHD will try to select the first subdevice on your system. \\
See the application notes for further details.
Single channel example: :AB
Dual channel example: :A :B

Num Channels:
Selects the total number of channels in this multi-SMINI configuration.
Ex: 4 motherboards with 2 channels per board = 8 channels total

Sample rate:
The sample rate is the number of samples per second input by this block. \\
The SHD device driver will try its best to match the requested sample rate. \\
If the requested rate is not possible, the SHD block will print an error at runtime.

Center frequency:
The center frequency is the overall frequency of the RF chain. \\
For greater control of how the SHD tunes elements in the RF chain, \\
pass a tune_request object rather than a simple target frequency.
Tuning with an LO offset example: shd.tune_request(freq, lo_off)

	</doc>
</block>
"""

PARAMS_TMPL = """
	<param>
		<name>Ch$(n): Center Freq (Hz)</name>
		<key>center_freq$(n)</key>
		<value>0</value>
		<type>real</type>
		<hide>\#if \$nchan() > $n then 'none' else 'all'#</hide>
	</param>
	<param>
		<name>Ch$(n): Gain (dB)</name>
		<key>gain$(n)</key>
		<value>0</value>
		<type>real</type>
		<hide>\#if \$nchan() > $n then 'none' else 'all'#</hide>
	</param>
"""

def parse_tmpl(_tmpl, **kwargs):
	from Cheetah import Template
	return str(Template.Template(_tmpl, kwargs))

max_num_mboards = 8
max_num_channels = max_num_mboards*4

if __name__ == '__main__':
	import sys
	for file in sys.argv[1:]:
		if 'source' in file:
			sourk = 'source'
			direction = 'out'
		elif 'sink' in file:
			sourk = 'sink'
			direction = 'in'
		else: raise Exception, 'is %s a source or sink?'%file

		params = ''.join([parse_tmpl(PARAMS_TMPL, n=n) for n in range(max_num_channels)])
		open(file, 'w').write(parse_tmpl(MAIN_TMPL,
			max_nchan=max_num_channels,
			max_mboards=max_num_mboards,
			params=params,
			sourk=sourk,
			direction=direction,
		))
