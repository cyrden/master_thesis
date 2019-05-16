#!/usr/bin/env python3

import json
import os
import signal
import subprocess
import sys
from argparse import ArgumentParser as ap
from time import clock_gettime_ns, CLOCK_MONOTONIC, sleep


import psutil

__STOP__ = False


def sigint_handler(sig, frame):
    global __STOP__
    __STOP__ = True
    print("Your request has successfully been taken into account. Please wait")


def tick():
    return clock_gettime_ns(CLOCK_MONOTONIC)


def perf(json_path, pid=None, args=None, interval=1, ):
    if pid is not None and args is not None:
        print("Either pid or args must be chosen at the same time")
        sys.exit(1)

    if interval <= 0:
        print('Interval must be strictly greater than 0')
        sys.exit(1)

    if pid is not None:
        p = psutil.Process(pid)
    elif args is not None:
        p = psutil.Popen(args, stdout=sys.stdout, stderr=sys.stderr)
    else:
        print("args and pid are empty")
        sys.exit(1)

    attrs = ["cpu_percent", "cpu_num", "num_threads", "threads", "cpu_times",
             "nice", "num_ctx_switches", "memory_full_info", "memory_percent",
             "connections", "io_counters"]

    with open(json_path, 'w') as f:
        while not __STOP__:
            f.write(json.dumps({
                'time': tick(),
                'info': p.as_dict(attrs)
            }))
            f.write('\n')
            sleep(interval)

    return p


def custom_perf(args, other):
    pre_processes = []

    with open(args.path[0], 'r') as j:
        data = json.load(j)

    proc_monit_args = data['monit'].strip().split()

    for command in data['pre_start']:
        p = subprocess.Popen(command.strip().split())
        pre_processes.append(p)

    monitor_process = perf(args.file, args=proc_monit_args, interval=args.interval)

    monitor_process.send_signal(signal.SIGINT)
    for proc in pre_processes:
        proc.send_signal(signal.SIGINT)


def from_pid_main(args, other):
    perf(args.file, pid=args.pid[0], interval=args.interval)


def from_args_main(args, other):
    command = args.args + other
    perf(args.file, args=command, interval=args.interval)


if __name__ == '__main__':
    def error(args, other):
        print("Positional argument is missing")
        parser.print_usage()


    if os.getuid() != 0:
        print('This script must be run as root')
        sys.exit(1)

    signal.signal(signal.SIGINT, sigint_handler)
    parser = ap(description='Collect CPU, Memory and networking info from given process')

    parser.add_argument('-f', '--file', required=True,
                        help="Filepath where process stats will be written to")
    parser.add_argument('-i', '--interval', default=1, type=float,
                        help='TODO')

    parser.set_defaults(func=error)

    s = parser.add_subparsers()

    # Attach psutils to PID
    from_pid = s.add_parser('pid', help='Attach psutils to already running process')
    from_pid.add_argument('pid', metavar='pid', type=int, nargs=1, help='pid of the process to monitor')
    from_pid.set_defaults(func=from_pid_main)

    # Run psutils from commandline args given at arguments
    from_args = s.add_parser('args', help='Gives to psutils the command to run the process')
    from_args.add_argument('args', metavar='args', type=str, nargs='+',
                           help="full command line to run the process to monitor")
    from_args.set_defaults(func=from_args_main)

    # More advanced monitoring with custom JSON file
    custom = s.add_parser('custom', help='Define a JSON telling which processes to run')
    custom.add_argument('path', nargs=1, help='Path to JSON containing directives')
    custom.set_defaults(func=custom_perf)

    arguments, other_args = parser.parse_known_args()
    arguments.func(arguments, other_args)
