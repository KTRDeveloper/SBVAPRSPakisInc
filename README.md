### How to build docker image

```bash
cd docker
./build_SBVAPRSPakisInc_images.sh
```

### How to build

```bash
./build.sh
```

### How to use


```bash
cd bin
./run_SBVAPRSPakisInc <instance> <rundir> 
```



### Run without SBVA
```bash
./PRSPakisInc <instance> [config=config_filename] [--option=param]
```

For example, 

```bash
./PRSPakisInc test.cnf --clause_sharing=1 --DCE=1 --preprocessor=1 --nThreads=32 --cutoff=5000
```

### Parameters and Options

instance: input CNF 

nThreads: the number of workers in PRS

cutoff: the wall time for SAT solving

clause_sharing: whether use clause sharing (1: enable; 0: disable) 

preprocessor: whether use preprocessing (1: enable; 0: disable)
