# Must Read before Use

As Baidu Cloud has limited the max size of a single file, we have to cut the whole dataset into 40 .zip files with each about 2.0 GB. 
You should download all the .zip files in the 'Zipdiles' folder, then edit the script 'file_unzip.py', you will obtain the whole dataset. 

Please check the final dataset directory according to the following structure.

```
${DATASET_ROOT}
|-- E01
|   |-- S01
|   |   |-- A01
|   |   |   |-- rgb
|   |   |   |-- mmwave
|   |   |   |-- wifi-csi
|   |   |   |-- ...
|   |   |-- A02
|   |   |-- ...
|   |   |-- A27
|   |-- S02
|   |-- ...
|   |-- S10
|-- E02
|......
|-- E03
|......
|-- E04
|......
```

