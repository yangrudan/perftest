# METAX

./autogen.sh && ./configure --enable-maca --with-maca=/opt/maca && make -j2

``` bash
./ib_read_lat -d mlx5_3 --use_metax=1 -s 4096 -D 20 --report_gbits
./ib_read_lat -d mlx5_3 --use_metax=1 -s 4096 -D 20 --report_gbits 10.107.204.141
```


# MOORE
./autogen.sh && ./configure --enable-musa --with-musa=/usr/local/musa && make -j8

``` bash
```