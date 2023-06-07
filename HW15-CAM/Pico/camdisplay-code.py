# requires adafruit_ov7670.mpy and adafruit_st7735r.mpy in the lib folder
import time
from displayio import (
    Bitmap,
    Group,
    TileGrid,
    FourWire,
    release_displays,
    ColorConverter,
    Colorspace,
)
from adafruit_st7735r import ST7735R
import board
import busio
import digitalio
from adafruit_ov7670 import (
    OV7670,
    OV7670_SIZE_DIV1,
    OV7670_SIZE_DIV8,
    OV7670_SIZE_DIV16,
)

release_displays()
spi = busio.SPI(clock=board.GP2, MOSI=board.GP3)
display_bus = FourWire(spi, command=board.GP0, chip_select=board.GP1, reset=None)
display = ST7735R(display_bus, width=160, height=128, rotation=90)


# Ensure the camera is shut down, so that it releases the SDA/SCL lines,
# then create the configuration I2C bus

with digitalio.DigitalInOut(board.GP10) as reset:
    reset.switch_to_output(False)
    time.sleep(0.001)
    bus = busio.I2C(board.GP9, board.GP8) #GP9 is SCL, GP8 is SDA

# Set up the camera (you must customize this for your board!)
cam = OV7670(
    bus,
    data_pins=[
        board.GP12,
        board.GP13,
        board.GP14,
        board.GP15,
        board.GP16,
        board.GP17,
        board.GP18,
        board.GP19,
    ],  # [16]     [org] etc
    clock=board.GP11,  # [15]     [blk]
    vsync=board.GP7,  # [10]     [brn]
    href=board.GP21,  # [27/o14] [red]
    mclk=board.GP20,  # [16/o15]
    shutdown=None,
    reset=board.GP10,
)  # [14]

width = display.width
height = display.height

bitmap = None
# Select the biggest size for which we can allocate a bitmap successfully, and
# which is not bigger than the display
for size in range(OV7670_SIZE_DIV1, OV7670_SIZE_DIV16 + 1):
    #cam.size = size # for 4Hz
    #cam.size = OV7670_SIZE_DIV16 # for 30x40, 9Hz
    cam.size = OV7670_SIZE_DIV8 # for 60x80, 9Hz
    if cam.width > width:
        continue
    if cam.height > height:
        continue
    try:
        bitmap = Bitmap(cam.width, cam.height, 65534)
        break
    except MemoryError:
        continue

print(width, height, cam.width, cam.height)
if bitmap is None:
    raise SystemExit("Could not allocate a bitmap")
time.sleep(4)
g = Group(scale=1, x=(width - cam.width) // 2, y=(height - cam.height) // 2)
tg = TileGrid(
    bitmap, pixel_shader=ColorConverter(input_colorspace=Colorspace.BGR565_SWAPPED)
)
g.append(tg)
display.show(g)

t0 = time.monotonic_ns()
display.auto_refresh = False
while True:
    cam.capture(bitmap)
    bitmap.dirty()
    display.refresh(minimum_frames_per_second=0)
    t1 = time.monotonic_ns()
    print("fps", 1e9 / (t1 - t0))
    t0 = t1
