from gnuradio import gr
try: import pmt
except: from gruel import pmt

def make_lengthtags(lengths, offsets, tagname='length', vlen=1):
    tags = []
    assert(len(offsets) == len(lengths))
    for offset, length in zip(offsets, lengths):
        tag = gr.gr_tag_t()
        tag.offset = offset/vlen
        tag.key = pmt.pmt_string_to_symbol(tagname)
        tag.value = pmt.pmt_from_long(length/vlen)
        tags.append(tag)
    return tags

def string_to_vector(string):
    v = []
    for s in string:
        v.append(ord(s))
    return v

def strings_to_vectors(strings, lengthtagname):
    vs = [string_to_vector(string) for string in strings]
    return packets_to_vectors(vs, lengthtagname)

def vector_to_string(v):
    s = []
    for d in v:
        s.append(chr(d))
    return ''.join(s)

def vectors_to_strings(data, tags, lengthtagname):
    packets = vectors_to_packets(data, tags, lengthtagname)
    return [vector_to_string(packet) for packet in packets]

def count_bursts(data, tags, lengthtagname, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.pmt_symbol_to_string(t.key) == lengthtagname]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(lengthtagname, tag.offset))
        lengths[tag.offset] = pmt.pmt_to_long(tag.value)*vlen
    in_burst = False
    in_packet = False
    packet_length = None
    packet_pos = None
    burst_count = 0
    for pos in range(len(data)):
        if pos in lengths:
            if in_packet:
                print("Got tag at pos {0} current packet_pos is {1}".format(pos, packet_pos))
                raise StandardError("Received packet tag while in packet.")
            packet_pos = -1
            packet_length = lengths[pos]
            in_packet = True
            if not in_burst:
                burst_count += 1
            in_burst = True
        elif not in_packet:
            in_burst = False
        if in_packet:
            packet_pos += 1
            if packet_pos == packet_length-1:
                in_packet = False
                packet_pos = None
    return burst_count

def vectors_to_packets(data, tags, lengthtagname, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.pmt_symbol_to_string(t.key) == lengthtagname]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(lengthtagname, tag.offset))
        lengths[tag.offset] = pmt.pmt_to_long(tag.value)*vlen
    if 0 not in lengths:
        raise ValueError("There is no tag with key {0} and an offset of 0"
                         .format(lengthtagname))
    pos = 0
    packets = []
    while pos < len(data):
        if pos not in lengths:
            raise ValueError("There is no tag with key {0} and an offset of {1}."
                             "We were expecting one."
                             .format(lengthtagname, pos))
        length = lengths[pos]
        if length == 0:
            raise ValueError("Packets cannot have zero length.")
        if pos+length > len(data):
            raise ValueError("The final packet is incomplete.")
        packets.append(data[pos: pos+length])
        pos += length
    return packets

def packets_to_vectors(packets, lengthtagname, vlen=1):
    tags = []
    data = []
    offset = 0
    for packet in packets:
        data.extend(packet)
        tag = gr.gr_tag_t()
        tag.offset = offset/vlen
        tag.key = pmt.pmt_string_to_symbol(lengthtagname)
        tag.value = pmt.pmt_from_long(len(packet)/vlen)
        tags.append(tag)
        offset = offset + len(packet)
    return data, tags
