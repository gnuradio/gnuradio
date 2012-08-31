#!python

import sys
import os
import glob

this_dir = os.path.dirname(os.path.abspath(__file__))
volk_path = os.path.join(this_dir, 'include', 'volk')
kern_path = os.path.join(this_dir, 'kernels', 'volk')
print this_dir

kernels = glob.glob(os.path.join(volk_path, 'volk*.h'))
kernels = filter(lambda k: k.count('_') > 1, kernels)
kernels = map(os.path.basename, kernels)
print kernels

kern_to_aligned = set()
kern_to_unaligned = set()

for kern in kernels:
    if kern.endswith('_u.h'): kern_to_unaligned.add(kern)
    elif kern.endswith('_a.h'): kern_to_aligned.add(kern)
    else: raise Exception, 'what is %s'%kern

new_kerns = set()
for kern in kernels:
    new_kerns.add(kern.replace('_u.h', '.h').replace('_a.h', '.h'))

print '\n\n\n'
print new_kerns

try: os.makedirs(kern_path)
except: pass

for kern in new_kerns:
    code = ''
    kern_u = kern.replace('.h', '_u.h')
    kern_a = kern.replace('.h', '_a.h')
    if kern_u in kernels: code += open(os.path.join(volk_path, kern_u)).read()
    if kern_a in kernels: code += open(os.path.join(volk_path, kern_a)).read()
    if kern in kernels: code += open(os.path.join(volk_path, kern)).read()

    #replace u_generic....
    generic_find = kern.replace('.h', '_u_generic')
    generic_repl = kern.replace('.h', '_generic')
    if generic_find not in code:
        generic_find = kern.replace('.h', '_a_generic')
    if generic_find not in code:
        print 'no generic in %s!!'%kern

    #replace a_orc with u_orc....
    u_orc_find = kern.replace('.h', '_u_orc(')
    a_orc_find = kern.replace('.h', '_a_orc(')
    if u_orc_find not in code:
        code = code.replace(a_orc_find, u_orc_find)

    code = code.replace(generic_find, generic_repl)
    code = code.replace('volk_32fc_s32fc_x2_rotator_32fc_a', 'volk_32fc_s32fc_x2_rotator_32fc')
    #print 'write: ', kern
    open(os.path.join(kern_path, kern), 'w').write(code)

VOLK_PROFILE_SRS = (
    os.path.join(this_dir, 'lib', 'testqa.cc'),
    os.path.join(this_dir, 'apps', 'volk_profile.cc'),
)
for src in VOLK_PROFILE_SRS:
    code = open(src).read()
    new_code = list()
    for line in code.splitlines():
        line = line.replace('volk_32fc_s32fc_rotatorpuppet_32fc_a', 'volk_32fc_s32fc_rotatorpuppet_32fc')
        line = line.replace('volk_32fc_s32fc_x2_rotator_32fc_a', 'volk_32fc_s32fc_x2_rotator_32fc')
        special = 'VOLK_PROFILE' in line or 'VOLK_RUN_TESTS' in line
        if special:
            for kern in new_kerns:
                kern_x = kern.replace('.h', '')
                kern_u = kern.replace('.h', '_u')
                kern_a = kern.replace('.h', '_a')
                line = line.replace(kern_u, kern_x)
                line = line.replace(kern_a, kern_x)
        if special and line in new_code:
            pass
        else: new_code.append(line)
    open(src, 'w').write('\n'.join(new_code)+'\n')
