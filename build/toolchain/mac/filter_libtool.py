#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import re
import subprocess
import sys

# This script executes libtool and filters out logspam lines like:
#    '/path/to/libtool: file: foo.o has no symbols'
BLOCKLIST_PATTERNS = map(re.compile, [
    r'^.*libtool: (?:for architecture: \S* )?file: .* has no symbols$',
    r'^.*libtool: warning for library: .* the table of contents is empty '
    r'\(no object file members in the library define global symbols\)$',
    r'^.*libtool: warning same member name \(\S*\) in output file used for '
    r'input files: \S* and: \S* \(due to use of basename, truncation, '
    r'blank padding or duplicate input files\)$',
])


def is_blocklisted_line(line):
    """Returns whether the line should be filtered out."""
    for pattern in BLOCKLIST_PATTERNS:
        if isinstance(line, bytes):
            line = line.decode()
        if pattern.match(line):
            return True
    return False


def main(cmd_list):
    env = os.environ.copy()
    # The problem with this flag is that it resets the file mtime on the file
    # to epoch=0, e.g. 1970-1-1 or 1969-12-31 depending on timezone.
    env['ZERO_AR_DATE'] = '1'
    libtoolout = subprocess.Popen(cmd_list, stderr=subprocess.PIPE, env=env)
    _, err = libtoolout.communicate()
    for line in err.splitlines():
        if not is_blocklisted_line(line):
            print(line, file=sys.stderr)
    # Unconditionally touch the output .a file on the command line if present
    # and the command succeeded. A bit hacky.
    if not libtoolout.returncode:
        for i in range(len(cmd_list) - 1):
            if cmd_list[i] == '-o' and cmd_list[i + 1].endswith('.a'):
                os.utime(cmd_list[i + 1], None)
                break
    return libtoolout.returncode


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
