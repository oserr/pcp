#!/usr/bin/python
# benchmark script 

import subprocess
import sys
import os
import os.path
import getopt
import socket

numbers = 50000
mapLoad = 0.7

# format: [ benchmarkname n inserts removes lookoups struct outputfile  ]
benchmarkList = {
    "insert_heavy": (numbers, 0.8, 0.1, 0.1, 0.2, mapLoad),
    "insert_medium": (numbers, 0.5, 0.25, 0.25, 0.5, mapLoad),
    "lookup_heavy": (numbers, 0.1, 0.1, 0.8, 0.9, mapLoad),
    "lookup_medium": (numbers, 0.25, 0.25, 0.5, 0.75, mapLoad),
    "removal_heavy": (numbers, 0.1, 0.8, 0.1, 0.9, mapLoad),
    "removal_medium": (numbers, 0.25, 0.5, 0.25, 0.75, mapLoad), 
}


def usage(fname):
    print "Usage: %s [-h] [-d <l|m|b>] " % fname
    print "    -h            Print this message"
    print "    -d <l|m|b>    Struct to run in benchmark"
    print "                  l: list"
    print "                  m: map"
    print "                  b: both"
    print "    -b benchmarkname    Name of specific benchmark to run"
    sys.exit(0)


def cmd(numbers, inserts, removes, lookups, preload, mapload, outputDir, datastruct):
    cmd = ["../build/src/benchmark"] + ["-n", str(numbers), "--inserts", str(inserts), "--removals", str(removes), "--lookups", str(lookups), "--preload", str(preload), "-u", str(mapload), "-o", outputDir, "-d", datastruct ]
    gcmdLine = " ".join(cmd)
    print gcmdLine
    try:
        # File number of standard output
        stdoutFileNumber = 1
        simProcess = subprocess.Popen(gcmdLine, stderr = stdoutFileNumber, shell=True)
        simProcess.wait()
        retcode = simProcess.returncode
    except Exception as e:
        print "Execution of command '%s' failed. %s" % (gcmdLine, e)
        return False


def run(name, args):
    optlist, args = getopt.getopt(args, "hd:b:")
    benchmarks = []
    datastruct = ""

    # identify lateday or ghc
    machine = socket.gethostname()
    if "ghc" in machine:
      outputDir = "../data/ghc/"
    else:
      outputDir = "../data/latedays/"

    # read parameters
    for (opt, val) in optlist:
        if opt == '-h':
            usage(name)
        elif opt == '-d':
            if val in ["l", "m", "b"]:
              datastruct = val
            else:
              print "Invalid update mode '%s'" % c
              usage(name)
        elif opt == '-b':
          if val in benchmarkList:
            benchmarks.append(benchmarkList[val])
          else:
            print "Benchmark does '%s' not exists" % val
            usage(name)

    # if no benchmark specified run all 
    if not benchmarks:
      for k in benchmarkList:
        benchmarks.append(benchmarkList[k])

    # if not specified run for both lists and map
    if not datastruct:
      datastruct = "b"

    # run benchmarks
    for bparams in benchmarks:
      (numbers, inserts, removes, lookups, preload, mapload) = bparams
      cmd(numbers, inserts, removes, lookups, preload, mapload, outputDir, datastruct)


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])

