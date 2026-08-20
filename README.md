# FilterC

FilterC is a C program for applying image filters to **24-bit BMP images**.

## Filters

* `Blur` — Applies a box blur.
* `Grayscale` — Converts the image to grayscale.
* `Sepia` — Applies a sepia-tone effect.
* `FlipH` — Flips the image horizontally.
* `FlipV` — Flips the image vertically.

## Usage

```bash
./filter [Filter] ./inputPath.bmp ./outputPath.bmp
```

Example:

```bash
./filter grayscale ./input.bmp ./output.bmp
```

Filter names are **case-insensitive**.

## IMPORTANT

The input image must:

* Be a BMP file.
* Use the standard 40-byte DIB header.
* Be **24-bit**.
* Be **uncompressed**.

Most BMP images works, but other BMP format may not be supported.


**Created by:** Arjay Cedigo