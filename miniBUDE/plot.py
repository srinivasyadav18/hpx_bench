
import pandas as pd
import matplotlib.pyplot as plt

# seq_time = None

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

for df_name, df in all_dfs.items():
    if df_name == 'seq' or df_name == 'simd':
        continue
    plt.plot(df["threads"], df["speed_up"], label=df_name, marker='o')

plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads for Different Datasets")
plt.grid(True)
print(df["threads"])
plt.xticks(df["threads"])
plt.yticks(df["threads"])
plt.legend()
plt.savefig("time_vs_threads.png")
# plt.savefig


