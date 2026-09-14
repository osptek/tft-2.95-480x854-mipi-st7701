<p align="left"><img alt="OSPTEK" src="./images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 2.95″ TFT 480×854 (ST7701 · MIPI)</h1>

<p align="center"><b>TFT / IPS module · MIPI · ST7701</b></p>

<p align="center"><a href="./README.md">简体中文</a> | English · <a href="../../README_EN.md">Family index</a></p>

<p align="center">
  <img alt="Size: 2.95 inch" src="https://img.shields.io/badge/Size-2.95%22-3498DB?style=flat-square" />
  <img alt="Resolution: 480x854" src="https://img.shields.io/badge/Resolution-480%C3%97854-8E44AD?style=flat-square" />
  <img alt="Interface: MIPI" src="https://img.shields.io/badge/Interface-MIPI-27AE60?style=flat-square" />
  <img alt="Driver: ST7701" src="https://img.shields.io/badge/Driver-ST7701-E7352C?style=flat-square" />
</p>

<p align="center"><img alt="OSPTEK 2.95″ 480×854 TFT MIPI module (ST7701) product image" src="./images/product.png" width="640" /></p>

## Contents

- [Overview](#overview)
- [Specifications](#specifications)
- [Sample projects](#sample-projects)
- [Repository layout](#repository-layout)
- [Resources](#resources)
- [Buy](#buy)
- [Support](#support)

---

## Overview

OSPTEK **2.95″ 480×854 TFT (IPS)** is a **MIPI** color display module driven by **ST7701**. Suited to handheld devices, portrait instruments, and compact HMI.

Spec ID (repository name): `tft-2.95-480x854-mipi-st7701`

Current module version: **YDP295B001-V1**. Electrical and mechanical details follow [`docs/YDP_295_B001_V1_f812644e0f.pdf`](./docs/YDP_295_B001_V1_f812644e0f.pdf).

## Specifications

| Item | Spec |
| ---- | ---- |
| Size | 2.95 inch |
| Type | TFT / IPS (color) |
| Resolution | 480×854 |
| Interface | MIPI |
| Driver IC | ST7701 |

> Full outline, FPC definition, power, and timing follow the product datasheet / driver IC datasheet.

## Sample projects

| Description | Path |
| ---- | ---- |
| ESP32-P4 · ST7701 MIPI + esp-lvgl-port / LVGL9 | [`examples/P4-IDF_ST7701-MIPI_ESP-LVGL-PORT_V9/`](./examples/P4-IDF_ST7701-MIPI_ESP-LVGL-PORT_V9/) |
| ESP32-P4 · ST7701 MIPI DSI + LVGL | [`examples/st7701s_mipi_dsi/`](./examples/st7701s_mipi_dsi/) |

## Repository layout

```text
tft-2.95-480x854-mipi-st7701/                                # repo root (nav: ../../README_EN.md)
└── versions/
    └── YDP295B001-V1/                                # full materials for this part number
        ├── README.md
        ├── README_EN.md
        ├── images/
        ├── docs/
        └── examples/
```

## Resources

### Product files

| Resource | Link |
| ---- | ---- |
| Product datasheet (YDP295B001-V1) | [`docs/YDP_295_B001_V1_f812644e0f.pdf`](./docs/YDP_295_B001_V1_f812644e0f.pdf) |
| Driver IC datasheet (ST7701S) | [`docs/ST_7701_S_SPEC_V1_3_f82b940377.pdf`](./docs/ST_7701_S_SPEC_V1_3_f82b940377.pdf) |
| Init sequence (C source) | [`docs/ST7701S+BOE2.95-2L.c`](./docs/ST7701S%2BBOE2.95-2L.c) |
| Init command table (`st7701s.h`) | [`docs/st7701s.h`](./docs/st7701s.h) |
| RGB timing parameters | [`docs/RGB时序参数.png`](./docs/RGB%E6%97%B6%E5%BA%8F%E5%8F%82%E6%95%B0.png) |

### Samples

- [ESP32-P4 ST7701 MIPI + LVGL9](./examples/P4-IDF_ST7701-MIPI_ESP-LVGL-PORT_V9/)
- [ESP32-P4 ST7701 MIPI DSI + LVGL](./examples/st7701s_mipi_dsi/)

## Buy

<p align="center">
  <a href="https://www.aliexpress.com/store/1105701619"><img alt="AliExpress store" src="https://img.shields.io/badge/AliExpress-Official_Store-FF6A00?style=for-the-badge" /></a>
  &nbsp;&nbsp;
  <a href="https://shop110742373.taobao.com/"><img alt="Taobao store" src="https://img.shields.io/badge/Taobao-Official_Store-FF6A00?style=for-the-badge" /></a>
</p>

**Overseas (AliExpress)**

- Store: [OSPTEK Official Store](https://www.aliexpress.com/store/1105701619)

**China (Taobao)**

- Store: [鱼鹰光电工厂店](https://shop110742373.taobao.com/)

## Support

- Technical support / product inquiry: <luyu@osptek.com>
- QQ group: **985881096**
- Website: <https://osptek.com/>
- Feel free to open an Issue in this repository with any questions

---

<p align="center"><sub>© 2026 OSPTEK · Materials in this repository are licensed under CC BY 4.0</sub></p>
