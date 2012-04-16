#
# Copyright 2012 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import optparse
import volk_arch_defs
import volk_machine_defs

def format_flag(flag, compiler):
    if compiler == 'msvc' and flag in ('mmmx', 'msse'):
        return '/arch:SSE'
    if compiler == 'msvc' and flag in ('msse2',):
        return '/arch:SSE2'
    if compiler == 'msvc' and flag in ('msse3', 'mssse3', 'msse4.1', 'msse4.2', 'mpopcnt', 'mavx'):
        return '/arch:AVX'
    #otherwise its a gcc compiler
    return '-' + flag

def do_arch_flags_list(compiler):
    output = list()
    for arch in volk_arch_defs.archs:
        fields = [arch.name]
        fields += list(map(lambda f: format_flag(f, compiler), arch.flags))
        output.append(','.join(fields))
    print ';'.join(output)

def do_machines_list(arch_names):
    output = list()
    for machine in volk_machine_defs.machines:
        machine_arch_set = set(machine.arch_names)
        if set(arch_names).intersection(machine_arch_set) == machine_arch_set:
            output.append(machine.name)
    print ';'.join(output)

def do_machine_flags_list(compiler, machine_name):
    output = list()
    machine = volk_machine_defs.machine_dict[machine_name]
    for arch in machine.archs:
        for flag in arch.flags:
            output.append(format_flag(flag, compiler))
    print ' '.join(output)

def main():
    parser = optparse.OptionParser()
    parser.add_option('--mode', type='string', default='')
    parser.add_option('--compiler', type='string', default='')
    parser.add_option('--archs', type='string', default='')
    parser.add_option('--machine', type='string', default='')
    (opts, args) = parser.parse_args()

    if opts.mode == 'arch_flags': return do_arch_flags_list(opts.compiler.lower())
    if opts.mode == 'machines': return do_machines_list(opts.archs.split(';'))
    if opts.mode == 'machine_flags': return do_machine_flags_list(opts.compiler.lower(), opts.machine)

if __name__ == '__main__': main()
