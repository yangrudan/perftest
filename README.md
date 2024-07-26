# METAX

./autogen.sh && ./configure --enable-maca --with-maca=/opt/maca && make -j2

``` bash
./ib_read_lat -d mlx5_3 --use_metax=1 -s 4096 -D 20 --report_gbits
./ib_read_lat -d mlx5_3 --use_metax=1 -s 4096 -D 20 --report_gbits 10.107.204.141
```


# MOORE

./autogen.sh && ./configure --enable-musa --with-musa=/usr/local/musa && make -j8

``` bash
 ./ib_send_bw -d mlx5_2 --use_moore=1 -s 4096 -D 20 --report_gbits
```


# KUNLUN

## Device Ptr to XDR ADDR(/dev/xdrdrv的offset)

```
xpu_current_device # 获取逻辑ID;

xpu_device_get_attr(&dev_id, XPUATTR_DEVID, logic_dev_id) ，从逻辑ID获取物理ID；

offset = dev_id << 60  # dev_id 必须是物理ID，不能是逻辑ID；

xdr_addr = dev_addr | offset

# 对齐到4K
1 page4k = 1 << 12
2 page_mask = ~(page4k-1)
3 aligned_addr = xdr_addr & page_mask  # XDR 驱动内部会将地址mapping到mmio地址空间上
```

查看设备xdr信息
```bash
cat /proc/xdr/map_xpus
```


## mmap参数版
```bash
 ./ib_read_bw  -a  --report_gbits  -d mlx5_1 --mmap=/dev/xdrdrv --mmap-offset=0x0000000090001000
 ./ib_read_bw  -a  --report_gbits  -d mlx5_1 --mmap=/dev/xdrdrv --mmap-offset=0x0000000090001000 10.107.204.3
```