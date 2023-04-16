# Benchmark
The benchmark script runs the program with different algorithm versions and number of bodies, records performance metrics and stores the results in a Pandas DataFrame. It uses Seaborn plots to visualize performance of each algorithm version.

## Previous requirements

In order to run `benchmark.py` you will have to install the requirements:
```bash
pip install -r requirements.txt
```

After that, you will need to compile the C++ program. Please, read the main `README.md` of the project if you do not know how to compile it.

## Run the benchmark
You can run the benchmark with the next command:
```bash
python3 benchmark.py /path/to/compiled/program 
```
> Do not touch anything until the script shows you the first plot.  

> Please note that there are multiple plots generated by the script, but only one plot will appear at a time. You will need to close the current plot to see the next one.

## Optional arguments
You can also use some optional arguments to customize the benchmark:
```bash
python3 benchmark.py /path/to/compiled/program --algorithm 1
```

This will only measure the performance of algorithm/version 1.

Another example:
```bash
python3 benchmark.py /path/to/compiled/program --algorithm 1 3
```
This will measure the performance of algorithm/version 1 and 3.