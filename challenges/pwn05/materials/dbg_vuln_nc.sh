#!/bin/bash
gdb -ex "set follow-fork-mode child" -ex "catch exec" -ex "r" -ex "b main" --args ncat -vvlp $1 -e ./vuln


