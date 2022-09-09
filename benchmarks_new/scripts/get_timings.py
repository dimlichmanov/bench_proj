import subprocess
from subprocess import check_output
import sys
import re
import pprint
import copy
import os
from .arch_properties import get_arch
from .files import *
from .roofline import kunpeng_characteristics,intel_xeon_characteristics,amd_epyc_characteristics


def get_timing_from_file_line(line, timings):
    for key, val in timings.items():
        if key in line:
            timings[key] = float(line.split(" ")[1])
    arch_name = get_arch()
    dram_bandwidth = 1
    if arch_name == "kunpeng":
        dram_bandwidth = kunpeng_characteristics["bandwidths"]["DRAM"]
    if arch_name == "intel_xeon":
        dram_bandwidth = intel_xeon_characteristics["bandwidths"]["DRAM"]
    if arch_name == "amd_epyc":
        dram_bandwidth = amd_epyc_characteristics["bandwidths"]["DRAM"]

    timings["mem_efficiency"] = 100.0 * timings["avg_bw"]/dram_bandwidth
    return timings


def run_test_and_parse_timings(prog_name, prog_args, options):  # collect time, perf and BW values
    result = {}

    os.environ['OMP_NUM_THREADS'] = str(options.thread_num)
    os.environ['OMP_PROC_BIND'] = "close"

    prog_name_arg = ["--prog=" + prog_name]
    compiler_arg="--compiler="+options.compiler
    profiling_args = [compiler_arg, "--no_run=false", "--metrics=false", "--output="+tmp_timings_file_name]
    all_args = prog_name_arg + profiling_args + prog_args
    if options.metrics_run :
        all_args = all_args + ["--metrics-run"]

    if options.vtune_enable:
        all_args = all_args + ["--vtune"]

    if options.affinity == "None":
        all_args = all_args + ["--affinity=None"]
    else:
        all_args = all_args + ["--affinity={}".format(options.affinity)]

    subprocess.check_call(["bash"] + ['./make_omp.sh'] + all_args)
    a_file = open("./"+prog_name+"/"+tmp_timings_file_name)

    lines = a_file.readlines()
    timings = {"avg_time": 0, "avg_bw": 0, "avg_flops": 0, "flops_per_byte": 0, "mem_efficiency": 0}
    for line in lines:
        timings = get_timing_from_file_line(line, timings)
    return timings


def init_table(output_file_name, timings):  # add header if file does not exist
    if not os.path.exists(output_file_name):
        with open(output_file_name, 'w') as output_file:
            output_file.write("test_name,")
            for key in timings:
                output_file.write(str(key) + ",")
            output_file.write("\n")


def add_timings_to_file(output_file_name, test_name, timings):
    with open(output_file_name, 'a') as output_file:
        output_file.write(test_name + ",")
        for key in timings:
            output_file.write(str(timings[key]) + ",")
        output_file.write("\n")


def add_roofline_data(roofline_file_name, test_name, timings):
    if not os.path.exists(roofline_file_name):
        write_mode = 'w'
    else:
        write_mode = 'a'

    with open(roofline_file_name, write_mode) as output_file:
        output_file.write(test_name + ",")
        output_file.write(str(timings["avg_flops"]) + ",")
        output_file.write(str(timings["flops_per_byte"]) + ",")
        output_file.write("\n")


def timings_add_separator():
    arch = get_arch()
    file_name = "./output/" + arch + "_timings.csv"
    with open(file_name, 'a') as output_file:
        output_file.write("\n")


def run_timings(prog_name, test_name, prog_args, options):
    arch = get_arch()
    timings_file_name = "./output/" + arch + "_timings.csv"
    roofline_file_name = "./output/" + arch + "_roofline.csv"

    timings = run_test_and_parse_timings(prog_name, prog_args, options)

    init_table(timings_file_name, timings)
    add_timings_to_file(timings_file_name, test_name, timings)

    add_roofline_data(roofline_file_name, test_name, timings)


