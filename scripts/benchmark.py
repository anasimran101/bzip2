import os
import csv
import time
import subprocess
import resource

BINARY = "./bzip2_impl"
CONFIG = "config.ini"

DATASETS = [
    "benchmarks/canterbury",
    "benchmarks/calgary",
    "benchmarks/silesia",
    "benchmarks/large_text",
    "benchmarks/binary"
]

CSV_FILE = "results.csv"


def get_memory_kb():
    usage = resource.getrusage(resource.RUSAGE_CHILDREN)
    return usage.ru_maxrss


def write_config(input_path, output_path, block_size=900000):
    with open(CONFIG, "w") as f:
        f.write(f"""input = {input_path}
output = {output_path}
block_size = {block_size}
rle1_enabled = true
bwt_type = suffix_array
mtf_enabled = true
rle2_enabled = true
huffman_enabled = true
""")


def run_benchmark(file_path):
    output_path = file_path + ".out"

    file_size = os.path.getsize(file_path)

    #write_config(file_path, output_path)

    start_mem = get_memory_kb()
    start = time.perf_counter()

    subprocess.run(
        [BINARY, CONFIG],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=True
    )

    end = time.perf_counter()
    end_mem = get_memory_kb()

    duration = end - start

    output_size = os.path.getsize(output_path)
    ratio = file_size / output_size if output_size else 0
    memory = end_mem - start_mem

    return os.path.basename(file_path), file_size, ratio, duration, memory


def collect_files():
    files = []
    for d in DATASETS:
        for root, _, fs in os.walk(d):
            for f in fs:
                files.append(os.path.join(root, f))
    return files


def init_csv():
    if not os.path.exists(CSV_FILE):
        with open(CSV_FILE, "w", newline="") as f:
            csv.writer(f).writerow([
                "File",
                "Size",
                "BlockSize",
                "CompressionRatio",
                "Time",
                "Memory"
            ])


def append(row):
    with open(CSV_FILE, "a", newline="") as f:
        csv.writer(f).writerow(row)


def main():
    init_csv()

    for file_path in collect_files():
        print("Running:", file_path)

        try:
            name, size, ratio, t, mem = run_benchmark(file_path)

            append([
                name,
                size,
                900000,
                f"{ratio:.6f}",
                f"{t:.6f}",
                mem
            ])

        except subprocess.CalledProcessError:
            print("Failed:", file_path)


if __name__ == "__main__":
    main()