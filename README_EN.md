# 2.95" 480×854 TFT MIPI module (ST7701) — documentation & samples

**简体中文：** [`README.md`](README.md)

---

> This repository provides **sample projects** for this module, together with datasheets, specifications, and interface / bring-up documentation for selection reference and integration.

## Product overview

| Item | Description |
|:--|:--|
| Module | 2.95-inch **TFT** panel, **480×854** resolution |
| Interface | **MIPI** |
| Driver IC | **ST7701** |
| Spec ID | **`2.95-tft-480x854-mipi-st7701`** is the common product designation in documentation |

---

## Repository layout

### Top-level

| Path | Contents |
|:--|:--|
| `docs/` | Datasheets, specifications, initialization documentation |
| `examples/` | **Sample projects** |

### `examples/` layout

| Location | Description (internal package folder) |
|:--|:--|
| `examples/` root | **IDF代码** |

### Sample project paths

| Description | Path |
|:--|:--|
| esp-lvgl-port + LVGL9 | `examples/P4-IDF_ST7701-MIPI_ESP-LVGL-PORT_V9/` |
| MIPI DSI + LVGL | `examples/st7701s_mipi_dsi/` |
