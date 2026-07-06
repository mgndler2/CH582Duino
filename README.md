# Arduino Core for WCH CH582 (CH58x RISC-V) with USB CDC & IAP

主要架構修改自 ElectronicCats/arduino-wch58x : https://github.com/ElectronicCats/arduino-wch58x#arduino-wch58x <br>

編譯工具遷移至 riscv-none-elf-gcc-xpack : https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack <br>

ISP工具使用 wchisp: https://github.com/ch32-rs/wchisp <br>



# CH582Duino

修改ld 及Startup 以適應C++ 及IAP 功能開發。<br>

以union-struct重編CH582的SFR，快速對應暫存器與外設的關係。<br>

實作USB CDC 與USB IAP 方便快速DEBUG 及自動下載。<br>

<br>

## 測試僅在Windows上進行

Board Manager 已包含win-x64、linux-x64、osx-x64、osx-arm64，但實測只在windows上進行，使用其餘平台時可能會需要自行偵錯。<br>

windows 上開發:<br>

1. 安裝CH375 驅動(CH372DRV)。

2. 自行下載並利用Zadig 將ISP模式下的CH582(顯示為USB Module)驅動綁定為"libusbK"。

3. 本包已將ISP化為Programmer，在ISP模式下燒錄Bootloader後即可以自動下載的模式開發。

4. 若發生Application()卡死導致CDC 無法自動進入Bootloader，按住BOOT鍵(PB22)後點按RST鍵即可進入Bootloader。

<br>

## Board Manager

https://mgndler2.github.io/CH582Duino/package_CH582Duino_index.json<br>

<br>

## 待辦

1. 嘗試實作TMR，以支援WS2812的控制。

2. 已實作I2C，功能可能不完整。

3. 已實作SPI但沒有條件測試。