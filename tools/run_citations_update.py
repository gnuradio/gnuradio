#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright 2022 Johannes Demel.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
#

import argparse
from pprint import pprint
import regex
import json
import pathlib


def parse_name(contributor):
    name_pattern = "(.*?) <"
    name = regex.search(name_pattern, contributor).group().replace(" <", "")
    email_pattern = "<(.*?)>"
    email = regex.search(email_pattern, contributor).group().replace(
        "<", "").replace(">", "")
    return name, email


def parse_contributors(contributors):
    result = []
    for c in contributors:
        name, email = parse_name(c)
        result.append({"name": name, "email": email})
    return result


'''
It would be really nice to fix this list automatically. Ideas welcome.
So far, this is the result of careful manual checks, email address comparisons etc.
'''

name_aliases = {
    "0xloem": "Karl Semich",
    "777arc": "Marc Lichtman",
    "aiph": "Philipp Aigner",
    "alekhgupta1441": "Alekh Gupta",
    "anastas": "Achilleas Anastasopoulos",
    "Andrew F. Davis": "Andrew Davis",
    "andriy gelman": "Andriy Gelman",
    "anshulthakur": "Anshul Thakur",
    "aru31": "Arpit Gupta",
    "arualok22": "Aradhana Alok",
    "Bernard Tyers - Sane UX Design": "Bernard Tyers",
    "Chris": "Chris Donohue",
    "Christophe SEGUINOT": "Christophe Seguinot",
    "cmayer": "Christoph Mayer",
    "CMorin": "Cyrille Morin",
    "cottrema": "Mark Cottrell",
    "DaulPavid": "Paul David",
    "devnulling": "Nate Temple",
    "dl1ksv": "Volker Schroer",
    "Doug": "Douglas Geiger",
    "Doug Geiger": "Douglas Geiger",
    "duggabe": "Barry Duggan",
    "ewxl": "Elof Wecksell",
    "fengzhe29888": "Zhe Feng",
    "flarroca": "Federico La Rocca",
    "gateship1": "Matt Griffiths",
    "ghostop14": "Mike Piscopo",
    "gnieboer": "Geof Nieboer",
    "gnychis": "George Nychis",
    "hcab14": "Christoph Mayer",
    "iohannez": "Johannes Schmitz",
    "Jan": "Jan Krämer",
    "Jan Kraemer": "Jan Krämer",
    "jan-safar": "Jan Šafář",
    "Jaroslav Skarvada": "Jaroslav Škarvada",
    "jblum": "Josh Blum",
    "jcorgan": "Johnathan Corgan",
    "jdemel": "Johannes Demel",
    "jfmadeira": "João Madeira",
    "Jiri Pinkava": "Jiří Pinkava",
    "jmadeira": "Joao Madeira",
    "JohnOrlando": "John Orlando",
    "johschmitz": "Johannes Schmitz",
    "jsallay": "John Sallay",
    "jwl": "Jeff Long",
    "kolen": "Konstantin Mochalov",
    "krk": "Kerem Kat",
    "lazydodo": "Ray Molenkamp",
    "luz paz": "luzpaz",
    "luz.paz": "luzpaz",
    "m-ri@users.noreply.github.com": "Marco Ribero",
    "maitbot": "A. Maitland Bottoms",
    "Marc": "Marc Lichtman",
    "Marc L": "Marc Lichtman",
    "Marcus D Leech": "Marcus Leech",
    "Marcus D. Leech": "Marcus Leech",
    "Marcus Mueller": "Marcus Müller",
    "mhostetter": "Matt Hostetter",
    "Michael": "Michael Dickens",
    "Michael L Dickens": "Michael Dickens",
    "michaelld": "Michael Dickens",
    "Micheal Dickens": "Michael Dickens",
    "Mike Jameson M0MIK": "Mike Jameson",
    "mleech": "Marcus Leech",
    "mormj": "Josh Morman",
    "muaddib1984": "Paul Atreides",
    "n4hy": "Bob McGwier",
    "namccart": "Nicholas McCarthy",
    "Nick": "Nick M",
    "Nick McCarthy": "Nicholas McCarthy",
    "nldudok1": "Martin Dudok van Heel",
    "Notou": "Cyrille Morin",
    "qarlosalberto": "Carlos Alberto Ruiz Naranjo",
    "riatsila": "Alistair Bird",
    "root": "Philip Balister",
    "schneider": "schneider42",
    "Sebastian Müller": "Sebastian Müller",
    "Sec": "Stefan Zehl",
    "sidkapoor97": "Siddharth Kapoor",
    "Solomon": "Solomon Tan",
    "spectrejan": "Jan Krämer",
    "Stefan": "Stefan Wunsch",
    "Stefan `Sec` Zehl": "Stefan Zehl",
    "Stefan-Olt": "Stefan Oltmanns",
    "Stephan Ludwig (donludovico)": "Stephan Ludwig",
    "sugandhagupta": "Sugandha Gupta",
    "Tim K": "Tim Kuester",
    "Tim Oshea": "Tim O'Shea",
    "Timo Juhani Lindfors": "Timo Lindfors",
    "Tom": "Tom Rondeau",
    "tracierenea": "Tracie Conn",
    "trondeau": "Tom Rondeau",
    "ttsou": "Thomas Tsou",
    "U-CERVELO\\ttsou": "Thomas Tsou",
    "Yamakaja": "David Winter",
}


drop_list = ['git', 'user', 'git repository hosting', 'U-DON-WORKBENCH\\Don']


def fix_known_names(contributors):
    results = []
    for c in contributors:
        if c['name'] not in drop_list:
            results.append(c)
    contributors = results
    for c in contributors:
        c['name'] = name_aliases.get(c['name'], c['name'])
    return contributors


def merge_names(contributors):
    results = []
    names = sorted(list(set([c['name'] for c in contributors])))
    all_emails = sorted(list(set([c['email'] for c in contributors])))
    for name in names:
        emails = []
        for c in contributors:
            if name == c['name']:
                emails.append(c['email'])
        emails = sorted(list(set(emails)))
        results.append({'name': name, 'email': emails})

    return results


def normalize_names(contributors):
    for c in contributors:
        name = c['name'].split(' ')
        if len(name) > 1:
            name = f'{name[-1]}, {" ".join(name[0:-1])}'
        else:
            name = name[0]
        c['name'] = name

    return contributors


def find_citation_file(filename='.zenodo.json'):
    # careful! This function makes quite specific folder structure assumptions!
    file_loc = pathlib.Path(__file__).absolute()
    file_loc = file_loc.parent
    citation_file = next(file_loc.glob(f'../{filename}'))
    return citation_file.resolve()


def load_citation_file(filename):
    with open(filename, 'r') as file:
        citation_file = json.load(file)
    return citation_file


def update_citation_file(filename, citation_data):
    with open(filename, 'w')as file:
        json.dump(citation_data, file, indent=4)


def main():
    parser = argparse.ArgumentParser(description='Update citation file')
    parser.add_argument('contributors', metavar='N', type=str, nargs='+',
                        help='contributors with emails: Name <email>')
    args = parser.parse_args()

    contributors = args.contributors[0].split("\n")
    contributors = parse_contributors(contributors)
    contributors = fix_known_names(contributors)
    contributors = merge_names(contributors)
    contributors = normalize_names(contributors)

    citation_file_name = find_citation_file()
    citation_file = load_citation_file(citation_file_name)

    creators = citation_file['creators']

    git_names = sorted([c['name'] for c in contributors])
    cite_names = sorted([c['name'] for c in creators])
    git_only_names = []
    for n in git_names:
        if n not in cite_names:
            git_only_names.append(n)

    for name in git_only_names:
        creators.append({'name': name})

    # make sure all contributors are sorted alphabetically by their family name.
    # Also discussed in https://github.com/gnuradio/gnuradio/issues/3155
    creators = sorted(creators, key=lambda x: x['name'])
    maintainers = ["Long, Jeff", "Morman, Josh"]
    maintainer_list = list(filter(lambda x: x['name'] in maintainers, creators))
    creators = list(filter(lambda x: x['name'] not in maintainers, creators))
    nick_list = list(filter(lambda x: ', ' not in x['name'], creators))
    fullname_list = list(filter(lambda x: ', ' in x['name'], creators))

    creators = maintainer_list + fullname_list + nick_list

    citation_file['creators'] = creators
    update_citation_file(citation_file_name, citation_file)


if __name__ == "__main__":
    main()
