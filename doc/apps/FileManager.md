# File Manager

File Manager for view images and set selected image for current watch face backgound.
The selected background is valid until selecting another watch face.
The `PineTimeStyle` watch face is currently not transparent and the background image is not visible.
You can review all files in 4MB external (SPI) Flash and delete them by long pressing the delete button.
File deletion does not work until the filesystem driver is not fixed.
The directories, `*.bin` and `*.txt` files can be opened with one click.
Long press on the files to open the menu.


### How to copy images to 4MB external flash

The image files must be of a suitable size (max 240x240), and converted by online [lvgl image converter](https://lvgl.io/tools/imageconverter) or `lv_img_conv` python script.
Use color format CF_TRUE_COLOR or CF_TRUE_COLOR_ALPHA and output format Binary RGB565 Swap.

```sh
$ lv_img_conv imagefile.png --output-file imagefile.bin --color-format CF_TRUE_COLOR -output-format bin -binary-format ARGB8565_RBSWAP   
```

Then send the image over to myimages/filename.bin on the watch's file system using a companion app like ITD , Amazfish or, using [File Explorer for PineTime](https://infinitimeexplorer.netlify.app) in chromium browser.

```sh
$ $ itctl fs write imagefile.bin /myimages/imagename.bin        # upload image   
```