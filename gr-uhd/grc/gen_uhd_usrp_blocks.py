"""
Copyright 2010-2011,2014 Free Software Foundation, Inc.

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
	<name>UHD: USRP $sourk.title()</name>
	<key>uhd_usrp_$(sourk)</key>
	<flags>throttle</flags>
	<import>from gnuradio import uhd</import>
	<import>import time</import>
	<make>uhd.usrp_$(sourk)(
	",".join((\$dev_addr, \$dev_args)),
	uhd.stream_args(
		cpu_format="\$type",
		\#if \$otw()
		otw_format=\$otw,
		\#end if
		\#if \$stream_args()
		args=\$stream_args,
		\#end if
		\#if \$stream_chans()
		channels=\$stream_chans,
		\#else
		channels=range(\$nchan),
		\#end if
	),$lentag_arg
)
\#if \$clock_rate()
self.\$(id).set_clock_rate(\$clock_rate, uhd.ALL_MBOARDS)
\#end if
#for $m in range($max_mboards)
########################################################################
\#if \$num_mboards() > $m and \$clock_source$(m)()
self.\$(id).set_clock_source(\$clock_source$(m), $m)
\#end if
########################################################################
\#if \$num_mboards() > $m and \$time_source$(m)()
self.\$(id).set_time_source(\$time_source$(m), $m)
\#end if
########################################################################
\#if \$num_mboards() > $m and \$sd_spec$(m)()
self.\$(id).set_subdev_spec(\$sd_spec$(m), $m)
\#end if
########################################################################
#end for
self.\$(id).set_samp_rate(\$samp_rate)
\#if \$sync() == 'sync'
self.\$(id).set_time_unknown_pps(uhd.time_spec())
\#elif \$sync() == 'pc_clock'
self.\$(id).set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)
\#end if
#for $n in range($max_nchan)
\#if \$nchan() > $n
self.\$(id).set_center_freq(\$center_freq$(n), $n)
\#if \$norm_gain${n}()
self.\$(id).set_normalized_gain(\$gain$(n), $n)
\#else
self.\$(id).set_gain(\$gain$(n), $n)
\#end if
	\#if \$ant$(n)()
self.\$(id).set_antenna(\$ant$(n), $n)
	\#end if
	\#if \$bw$(n)()
self.\$(id).set_bandwidth(\$bw$(n), $n)
	\#end if
#if $sourk == 'source'
	\#if \$lo_export$(n)() and not \$hide_lo_controls()
self.\$(id).set_lo_export_enabled(\$lo_export$(n), uhd.ALL_LOS, $n)
	\#end if
    \#if \$lo_source$(n)() and not \$hide_lo_controls()
self.\$(id).set_lo_source(\$lo_source$(n), uhd.ALL_LOS, $n)
	\#end if
	\#if \$dc_offs_enb$(n)()
self.\$(id).set_auto_dc_offset(\$dc_offs_enb$(n), $n)
	\#end if
	\#if \$iq_imbal_enb$(n)()
self.\$(id).set_auto_iq_balance(\$iq_imbal_enb$(n), $n)
	\#end if
#end if
\#end if
#end for
</make>
	<callback>set_samp_rate(\$samp_rate)</callback>
	#for $n in range($max_nchan)
	<callback>set_center_freq(\$center_freq$(n), $n)</callback>
	<callback>\#if \$norm_gain${n}()
self.\$(id).set_normalized_gain(\$gain$(n), $n)
\#else
self.\$(id).set_gain(\$gain$(n), $n)
\#end if
	</callback>
	<callback>\#if not \$hide_lo_controls()
set_lo_source(\$lo_source$(n), uhd.ALL_LOS, $n)
\#end if
	</callback>
    <callback>\#if not \$hide_lo_controls()
set_lo_export_enabled(\$lo_export$(n), uhd.ALL_LOS, $n)
\#end if
	</callback>
	<callback>set_antenna(\$ant$(n), $n)</callback>
	<callback>set_bandwidth(\$bw$(n), $n)</callback>
	#end for
	<param>
		<name>$(direction.title())put Type</name>
		<key>type</key>
		<type>enum</type>
		<option>
			<name>Complex float32</name>
			<key>fc32</key>
			<opt>type:fc32</opt>
		</option>
		<option>
			<name>Complex int16</name>
			<key>sc16</key>
			<opt>type:sc16</opt>
		</option>
		<option>
			<name>VITA word32</name>
			<key>item32</key>
			<opt>type:s32</opt>
		</option>
	</param>
	<param>
		<name>Wire Format</name>
		<key>otw</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if \$otw()
				none
			\#else
				part
			\#end if
		</hide>
		<option>
			<name>Automatic</name>
			<key></key>
		</option>
		<option>
			<name>Complex int16</name>
			<key>sc16</key>
		</option>
		<option>
			<name>Complex int12</name>
			<key>sc12</key>
		</option>
		<option>
			<name>Complex int8</name>
			<key>sc8</key>
		</option>
	</param>
	<param>
		<name>Stream args</name>
		<key>stream_args</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if \$stream_args()
				none
			\#else
				part
			\#end if
		</hide>
		<option>
			<name>peak=0.003906</name>
			<key>peak=0.003906</key>
		</option>
	</param>
	<param>
		<name>Stream channels</name>
		<key>stream_chans</key>
		<value>[]</value>
		<type>int_vector</type>
		<hide>
			\#if \$stream_chans()
				none
			\#else
				part
			\#end if
		</hide>
	</param>
	<param>
		<name>Device Address</name>
		<key>dev_addr</key>
		<value>""</value>
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
		<name>Device Arguments</name>
		<key>dev_args</key>
		<value>""</value>
		<type>string</type>
		<hide>
			\#if \$dev_args()
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
			<name>PC Clock</name>
			<key>pc_clock</key>
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
		<option>
			<name>200 MHz</name>
			<key>200e6</key>
		</option>
		<option>
			<name>184.32 MHz</name>
			<key>184.32e6</key>
		</option>
		<option>
			<name>120 MHz</name>
			<key>120e6</key>
		</option>
		<option>
			<name>30.72 MHz</name>
			<key>30.72e6</key>
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
		<name>Mb$(m): Clock Source</name>
		<key>clock_source$(m)</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if not \$num_mboards() > $m
				all
			\#elif \$clock_source$(m)()
				none
			\#else
				part
			\#end if
		</hide>
		<option><name>Default</name><key></key></option>
		<option><name>Internal</name><key>internal</key></option>
		<option><name>External</name><key>external</key></option>
		<option><name>MIMO Cable</name><key>mimo</key></option>
		<option><name>O/B GPSDO</name><key>gpsdo</key></option>
	</param>
	<param>
		<name>Mb$(m): Time Source</name>
		<key>time_source$(m)</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if not \$num_mboards() > $m
				all
			\#elif \$time_source$(m)()
				none
			\#else
				part
			\#end if
		</hide>
		<option><name>Default</name><key></key></option>
		<option><name>External</name><key>external</key></option>
		<option><name>MIMO Cable</name><key>mimo</key></option>
		<option><name>O/B GPSDO</name><key>gpsdo</key></option>
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
	<check>(not \$stream_chans()) or (\$nchan == len(\$stream_chans))</check>
	#for $n in range($max_nchan)
	<check>(\$norm_gain${n} and \$gain${n} &gt;= 0 and \$gain${n} &lt;= 1) or not \$norm_gain${n}</check>
	#end for
	<sink>
		<name>command</name>
		<type>message</type>
		<optional>1</optional>
		<hide>\$hide_cmd_port</hide>
	</sink>
	<$sourk>
		<name>$direction</name>
		<type>\$type.type</type>
		<nports>\$nchan</nports>
	</$sourk>
	<doc>
The UHD USRP $sourk.title() Block:

Device Address:
The device address is a delimited string used to locate UHD devices on your system. \\
If left blank, the first UHD device found will be used. \\
Use the device address to specify a specific device or list of devices.
USRP1 Example: serial=12345678
USRP2 Example: addr=192.168.10.2
USRP2 Example: addr0=192.168.10.2, addr1=192.168.10.3

$(direction.title()) Type:
This parameter controls the data type of the stream in gnuradio.

Wire Format:
This parameter controls the form of the data over the bus/network. \
Complex bytes may be used to trade off precision for bandwidth. \
Not all formats are supported on all devices.

Stream Args:
Optional arguments to be passed in the UHD streamer object. \
Streamer args is a list of key/value pairs; usage is determined by the implementation.
Ex: the scalar key affects the scaling between 16 and 8 bit integers in sc8 wire format.

Num Motherboards:
Selects the number of USRP motherboards in this device configuration.

Reference Source:
Where the motherboard should sync its time and clock references.
If source and sink blocks reference the same device,
it is only necessary to set the reference source on one of the blocks.

Subdevice specification:
Each motherboard should have its own subdevice specification \\
and all subdevice specifications should be the same length. \\
Select the subdevice or subdevices for each channel using a markup string. \\
The markup string consists of a list of dboard_slot:subdev_name pairs (one pair per channel). \\
If left blank, the UHD will try to select the first subdevice on your system. \\
See the application notes for further details.
Single channel example: :AB
Dual channel example: :A :B

Num Channels:
Selects the total number of channels in this multi-USRP configuration.
Ex: 4 motherboards with 2 channels per board = 8 channels total

Sample rate:
The sample rate is the number of samples per second input by this block. \\
The UHD device driver will try its best to match the requested sample rate. \\
If the requested rate is not possible, the UHD block will print an error at runtime.

Center frequency:
The center frequency is the overall frequency of the RF chain. \\
For greater control of how the UHD tunes elements in the RF chain, \\
pass a tune_request object rather than a simple target frequency.
Tuning with an LO offset example: uhd.tune_request(freq, lo_off)
Tuning without DSP: uhd.tune_request(target_freq, dsp_freq=0, \\
dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)

Antenna:
For subdevices with only one antenna, this may be left blank. \\
Otherwise, the user should specify one of the possible antenna choices. \\
See the daughterboard application notes for the possible antenna choices.

Bandwidth:
To use the default bandwidth filter setting, this should be zero. \\
Only certain subdevices have configurable bandwidth filters. \\
See the daughterboard application notes for possible configurations.

Length tag key (Sink only):
When a nonempty string is given, the USRP sink will look for length tags \\
to determine transmit burst lengths.

See the UHD manual for more detailed documentation:
http://uhd.ettus.com
	</doc>
</block>
"""

PARAMS_TMPL = """	<param>
		<name>Ch$(n): Center Freq (Hz)</name>
		<key>center_freq$(n)</key>
		<value>0</value>
		<type>real</type>
		<hide>\#if \$nchan() > $n then 'none' else 'all'#</hide>
		<tab>RF Options</tab>
	</param>
	<param>
		<name>Ch$(n): Gain Value</name>
		<key>gain$(n)</key>
		<value>0</value>
		<type>float</type>
		<hide>\#if \$nchan() > $n then 'none' else 'all'#</hide>
		<tab>RF Options</tab>
	</param>
	<param>
		<name>Ch$(n): Gain Type</name>
		<key>norm_gain$(n)</key>
		<value>False</value>
		<type>bool</type>
                <hide>\#if \$nchan() &lt;= $n
                all
                \#elif bool(\$norm_gain${n}())
                none
                \#else
                part
                \#end if</hide>
		<option>
			<name>Absolute (dB)</name>
			<key>False</key>
		</option>
		<option>
			<name>Normalized</name>
			<key>True</key>
		</option>
		<tab>RF Options</tab>
	</param>
	<param>
		<name>Ch$(n): Antenna</name>
		<key>ant$(n)</key>
		<value></value>
		<type>string</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#elif \$ant$(n)()
				none
			\#else
				part
			\#end if
		</hide>
		<option>
			<name>TX/RX</name>
			<key>TX/RX</key>
		</option>
#if $sourk == 'source'
		<option>
			<name>RX2</name>
			<key>RX2</key>
		</option>
		<option>
			<name>RX1</name>
			<key>RX1</key>
		</option>
#end if
		<tab>RF Options</tab>
	</param>
	<param>
		<name>Ch$(n): Bandwidth (Hz)</name>
		<key>bw$(n)</key>
		<value>0</value>
		<type>real</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#elif \$bw$(n)()
				none
			\#else
				part
			\#end if
		</hide>
		<tab>RF Options</tab>
	</param>
#if $sourk == 'source'
	<param>
		<name>Ch$(n): LO Source</name>
		<key>lo_source$(n)</key>
		<value>internal</value>
		<type>string</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#elif \$hide_lo_controls()
				all
			\#else
				none
			\#end if
		</hide>
		<option>
			<name>Internal</name>
			<key>internal</key>
		</option>
		<option>
			<name>External</name>
			<key>external</key>
		</option>
		<option>
			<name>Companion</name>
			<key>companion</key>
		</option>
		<tab>RF Options</tab>
	</param>
#end if
#if $sourk == 'source'
	<param>
		<name>Ch$(n): LO Export</name>
		<key>lo_export$(n)</key>
		<value>False</value>
		<type>bool</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#elif \$hide_lo_controls()
				all
			\#else
				none
			\#end if
		</hide>
		<option>
			<name>True</name>
			<key>True</key>
		</option>
		<option>
			<name>False</name>
			<key>False</key>
		</option>
		<tab>RF Options</tab>
	</param>
#end if
#if $sourk == 'source'
	<param>
		<name>Ch$(n): Enable DC Offset Correction</name>
		<key>dc_offs_enb$(n)</key>
		<value>""</value>
		<type>raw</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#else
				part
			\#end if
		</hide>
		<tab>FE Corrections</tab>
	</param>
	<param>
		<name>Ch$(n): Enable IQ Imbalance Correction</name>
		<key>iq_imbal_enb$(n)</key>
		<value>""</value>
		<type>raw</type>
		<hide>
			\#if not \$nchan() > $n
				all
			\#else
				part
			\#end if
		</hide>
		<tab>FE Corrections</tab>
	</param>
#end if
"""

SHOW_CMD_PORT_PARAM = """
	<param>
		<name>Show Command Port</name>
		<key>hide_cmd_port</key>
		<value>False</value>
		<type>enum</type>
		<hide>part</hide>
		<option>
			<name>Yes</name>
			<key>False</key>
		</option>
		<option>
			<name>No</name>
			<key>True</key>
		</option>
		<tab>Advanced</tab>
	</param>
"""

SHOW_LO_CONTROLS_PARAM = """
	<param>
		<name>Show LO Controls</name>
		<key>hide_lo_controls</key>
		<value>True</value>
		<type>bool</type>
		<hide>part</hide>
		<option>
			<name>Yes</name>
			<key>False</key>
		</option>
		<option>
			<name>No</name>
			<key>True</key>
		</option>
		<tab>Advanced</tab>
	</param>
"""

TSBTAG_PARAM = """	<param>
		<name>TSB tag name</name>
		<key>len_tag_name</key>
		<value></value>
		<type>string</type>
		<hide>\#if len(str(\$len_tag_name())) then 'none' else 'part'#</hide>
	</param>"""

TSBTAG_ARG = """
	#if $len_tag_name()
	$len_tag_name,
	#end if"""

def parse_tmpl(_tmpl, **kwargs):
	from Cheetah import Template
	return str(Template.Template(_tmpl, kwargs))

max_num_mboards = 8
max_num_channels = max_num_mboards*4

if __name__ == '__main__':
	import sys
	for file in sys.argv[1:]:
		if file.endswith ('source.xml'):
			sourk = 'source'
			direction = 'out'
		elif file.endswith ('sink.xml'):
			sourk = 'sink'
			direction = 'in'
		else: raise Exception, 'is %s a source or sink?'%file

		params = ''.join([parse_tmpl(PARAMS_TMPL, n=n, sourk=sourk) for n in range(max_num_channels)])
		params += SHOW_CMD_PORT_PARAM
		params += SHOW_LO_CONTROLS_PARAM
		if sourk == 'sink':
			params += TSBTAG_PARAM
			lentag_arg = TSBTAG_ARG
		else: lentag_arg = ''
		open(file, 'w').write(parse_tmpl(MAIN_TMPL,
			lentag_arg=lentag_arg,
			max_nchan=max_num_channels,
			max_mboards=max_num_mboards,
			params=params,
			sourk=sourk,
			direction=direction,
		))
