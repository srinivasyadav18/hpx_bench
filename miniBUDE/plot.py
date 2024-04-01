
import pandas as pd
import matplotlib.pyplot as plt
import sys
# seq_time = None

args = sys.argv
if len(args) != 2:
  print("ERROR : pass MACHINE name")
  sys.exit()

MACHINE = args[1]

def extract_file(fpath):
  f = open(fpath)
  print('--------------')
  print(f"extracting {fpath}")
  lines = f.readlines()
  # print(len(lines))
  i = 0

  data_dict = {}

  while i < len(lines):
    backend = lines[i].split(' ')[-1]
    # print(f"backend : {backend}")
    threads = lines[i+1].split(' ')[-1]
    time = lines[i+2].split(' ')[-2]

    # print(backend, threads, time)
    data_dict[float(threads)] = float(time)
    i+= 3

  # print(data_dict)
  # if seq_time is None:
  seq_time = data_dict[1]

  df = pd.DataFrame(list(zip(data_dict.keys(), data_dict.values())), columns=["threads", "time"])
  df["speed_up"] = seq_time/df["time"]
  df["par_eff"] = df["speed_up"]/df["threads"]
  print(df)
  return df


files = ['seq', 'simd', 'hpx', 'hpx_simd', 'hpx_static_cs', 'omp', 'omp_simd']
all_dfs = {}

for f in files:
  all_dfs[f] = extract_file(f'{f}_clean.out')

print(all_dfs)

plt.figure(figsize=(8, 6))

markers = {}
markers['hpx'] = 'o'
markers['hpx_simd'] = '*'
markers['hpx_static_cs'] = 's'
markers['omp'] = 'o'
markers['omp_simd'] = '*'

ls = {}
ls['hpx'] = 'dotted'
ls['hpx_simd'] = 'solid'
ls['hpx_static_cs'] = 'dashed'
ls['omp'] = 'dotted'
ls['omp_simd'] = 'solid'

for df_name, df in all_dfs.items():
    if df_name == 'seq' or df_name == 'simd' or df_name == 'hpx_static_cs':
        continue
    plt.plot(df["threads"], df["speed_up"], label=df_name, marker=markers[df_name], linestyle=ls[df_name])

plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("MiniBUDE benchmark\nSpeedup against Sequenced execution\nAMD_7763 2x 64 Cores")
plt.grid(True)
print(df["threads"])
plt.xticks(df["threads"])
plt.yticks(df["threads"])
plt.legend()
plt.tight_layout()
plt.savefig(f"plot.png", dpi=300)
# plt.savefig


