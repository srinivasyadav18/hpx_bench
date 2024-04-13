# import optparse module
import optparse
import subprocess
import os
import math
import pandas as pd
import matplotlib.pyplot as plt

def generate_default_threads():
    i = 2
    default_threads = "2,"
    N = math.pow(2, i)
    while N < os.cpu_count():
        default_threads += f'{int(N)},'
        i += 1
        N = math.pow(2, i)
    default_threads += str(os.cpu_count())
    return default_threads

default_threads = generate_default_threads()

def parse_threads(threads_str):
    threads_str_list = threads_str.split(',')
    threads_int_list = [int(item) for item in threads_str_list if item.isdigit()]

    return threads_int_list

def parse_benchmarks(exe_dir, benchmarks_str):
    benchmarks_list = benchmarks_str.split(',')
    benchmarks = {}
    for benchmark in benchmarks_list:
        benchmark_path = os.path.join(exe_dir, benchmark)
        if os.path.isfile(benchmark_path):
            benchmark_path, os.path.isfile(benchmark_path)
            benchmarks[benchmark] = benchmark_path
        else:
            print(f'Error: Benchmark "{benchmark}" does not exist at {exe_dir}')
            exit()
            
    return benchmarks

def parse_deck(data_dir, deck):
    deck_path = os.path.join(data_dir, deck)
    if os.path.exists(deck_path):
        return deck_path
    else:
        print(f'deck path does not exist {deck_path}')
        exit()

parser = optparse.OptionParser()
parser.add_option('-e', '--exe-dir', dest = 'exe_dir',
                type = 'str', 
                default = "build",
                help = 'executables directory')
parser.add_option('-b', '--bench', dest = 'bench',
                type = 'string', 
                help = 'list of benchmarks to run (comma seperated)')
parser.add_option('-t', '--threads', dest = 'threads',
                type = 'string', 
                default = default_threads,
                help = 'list of threads to run (comma seperated)')
parser.add_option('-i', '--iters', dest = 'iters',
                type = 'int', 
                default = 5,
                help = 'number of iterations to run')
parser.add_option('--data-dir', dest = 'data_dir',
                type = 'str',
                default = 'data',
                help = 'path to data dir')
parser.add_option('--deck', dest = 'deck',
                type = 'str', 
                default = 'bm1',
                help = 'deck or data to be used.')
parser.add_option('-m', '--mode', dest = 'mode',
                type = 'str', 
                default = 'both',
                help = 'mode can 1. run only 2. plot only 3. both')
parser.add_option('-o', '--output-dir', dest = 'output',
                type = 'str',
                default = 'results',
                help = 'output directory path for benchmarks results')
parser.add_option('-v', '--variant', dest = 'variant',
                type = 'str',
                help = 'unique name for this benchmark for ex. x84_64_intel8358_64Cores')

(options, args) = parser.parse_args()


if options.bench is None:
    parser.error("Needed list of benchmarks to run")
if options.variant is None: 
    parser.error("Required a unique name for this run")

print(options)

def parse_output_dir(variant_dir, deck):
    print(variant_dir)
    if os.path.exists(variant_dir) == False:
        os.mkdir(variant_dir)
    output_path = os.path.abspath(os.path.join(variant_dir, deck))
    if os.path.exists(output_path) == False:
        os.mkdir(output_path)
    return output_path                                  

config = {}
config['benchmarks'] = parse_benchmarks(os.path.abspath(options.exe_dir), options.bench)
config['threads'] = parse_threads(options.threads)
config['iters'] = options.iters
config['exe_dir'] = options.exe_dir
config['output'] = parse_output_dir(os.path.join(os.path.abspath(options.output), options.variant), options.deck)
config['data_dir'] = options.data_dir
config['deck'] = parse_deck(os.path.abspath(options.data_dir), options.deck)
config['variant'] = options.variant
config['n'] = 1048576 if options.deck == 'bm2_long' else 65536
mode = options.mode

print('####\n')
for k, v in config.items():
    print(f'{k}: {v}')
print('####\n')
print()

def run_bench(config):
    
    extra_args=f' -i {config["iters"]} --deck {config["deck"]} -n {config["n"]} '
    all_dfs = {}
    seq_time = None
    for bench_name, bench_path in config['benchmarks'].items():
        times = []
        for thread in config['threads']:
            print('-----------------------------------------')
            cmd = ''
            if bench_name == 'seq':
                cmd = f'{bench_path} -i 1 --deck {config["deck"]} -n {config["n"]}'
            elif bench_name == 'hpxmp' or bench_name[:3] == 'omp':
                cmd = f'OMP_NUM_THREADS={thread} {bench_path} {extra_args}'
            elif bench_name[:3] == 'hpx':
                cmd = f'{bench_path} --hpx:threads={thread} {extra_args}'
            else:
                print(f'No idea how to run {bench_name}')
                exit()
            print(bench_name, bench_name[:3])
            cmd += ' | grep "Average time"'
            print('executing cmd: ', cmd)
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
            print(result)
            if result.stderr != '':
                print(f'Error in executing cmd {cmd} --> {result.stderr}')
                exit()
            print(result.stdout.split(' '))
            avg_time = float(result.stdout.split(' ')[-2])
            print(avg_time)
            print('\n\n')

            times.append(avg_time)
            if bench_name == 'seq':
                break
        if bench_name == 'seq':
            # df = pd.DataFrame(list(zip([1], times)), columns=["threads", "time"])
            seq_time = avg_time
        else: 
            df = pd.DataFrame(list(zip(config['threads'], times)), columns=["threads", "time"])
            all_dfs[bench_name] = df
    
    return seq_time, all_dfs

def run_and_write_bench(config):
    seq_time, all_dfs = run_bench(config)
    print(seq_time)
    for bench_name, df in all_dfs.items():
        print(bench_name)
        print(df)
        df.to_csv(os.path.join(config['output'], f'{bench_name}.csv'), sep=',', index=False, encoding='utf-8')

    if seq_time is not None:
        seq_file = open(os.path.join(config['output'], 'seq.csv'), 'w')
        seq_file.write(str(seq_time))
        seq_file.close()
    # return seq_time, all_dfs

def read_bench(config):
    seq_file_path = os.path.join(config['output'], 'seq.csv')
    if os.path.exists(seq_file_path):
        print(f"Reading sequenced execution time from : {seq_file_path}")
        seq_file = open(seq_file_path, 'r')
        seq_time = float(seq_file.readline())
        print(f"Seq Time : ", seq_time)
        
        all_dfs = {}
        for bench_name in config['benchmarks'].keys():
            if bench_name != 'seq':
                print(f"Reading {bench_name}")
                df = pd.read_csv(os.path.join(config['output'], f'{bench_name}.csv'))
                print(df)
                all_dfs[bench_name] = df
        return seq_time, all_dfs
    else:
        print(f"Error. cannot find sequenced execution result file : {seq_file_path}")
        exit()

def plot_bench(config, seq_time, all_dfs):
    new_all_dfs = {}
    for bench_name, df in all_dfs.items():
        df["speed_up"] = seq_time/df["time"]
        new_all_dfs[bench_name] = df
    for k, v in new_all_dfs.items():
        print(k)
        print(v)

    for df_name, df in all_dfs.items():
        plt.plot(df["threads"], df["speed_up"], label=df_name)

    plt.xlabel("Threads")
    plt.ylabel("Speedup")
    plt.title(f"MiniBUDE benchmark\nSpeedup against Sequenced execution\n{config['variant']}")
    plt.grid(True)

    plt.xticks(config["threads"])
    # plt.yticks(config["threads"])
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(config['output'], f'plot_{config["variant"]}.png'), dpi=300)

# seq_time, all_dfs = read_bench(config)
# print(seq_time, all_dfs)
# plot_bench(config, seq_time, all_dfs)

if mode == 'both':
    run_and_write_bench(config)
    seq_time, all_dfs = read_bench(config)
    plot_bench(config, seq_time, all_dfs)
elif mode == 'run':
    run_and_write_bench(config)
elif mode == 'plot':
    seq_time, all_dfs = read_bench(config)
    plot_bench(config, seq_time, all_dfs)
else:
    print("Error in selecting mode of run script. Please choose run or plot or both!!")
    exit()
    