WCH CH582 Arduino Core with USB CDC

主要架構修改自 ElectronicCats/arduino-wch58x : https://github.com/ElectronicCats/arduino-wch58x#arduino-wch58x <br>

編譯工具遷移至 riscv-none-elf-gcc-xpack : https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack <br>

ISP工具使用 wchisp: https://github.com/ch32-rs/wchisp <br>



# CH582Duino-Cpp

修改ld 及Startup 以適應C++ 及IAP 功能開發。<br>

以struct重編CH582的SFR，快速對應暫存器與外設的關係。<br>

實作USB CDC 與USB IAP 方便快速DEBUG 及自動下載。<br>

<br>

## 測試僅在Windows上進行

windows 上開發:<br>

1. 安裝CH375 驅動(CH372DRV)。

2. 自行下載並利用Zadig 將ISP模式下的CH582(顯示為USB Module)驅動綁定為"libusbK"。

3. 本包已將ISP化為Programmer，在ISP模式下燒錄Bootloader後即可以自動下載的模式開發。

<br>

## Board Manager

https://mgndler2.github.io/CH582Duino/package_CH582Duino_index.json<br>

<br>

## 待辦

1. 已實作SPI但沒有條件測試。

2. I2C尚未完成實作，亦無條件完成測試。