import os
import time
import ipaddress
import wifi
import socketpool
import board
import microcontroller
import terminalio
from digitalio import DigitalInOut, Direction
from adafruit_httpserver.server import Server
from adafruit_httpserver.request import Request
from adafruit_httpserver.response import Response
# from adafruit_httpserver.methods import Method #obsolete. method=Method.POST has been replaced with method="POST"
from adafruit_httpserver.mime_types import MIMETypes

#  onboard LED setup
led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT
led.value = False

#  function to convert celcius to fahrenheit
def c_to_f(temp):
    temp_f = (temp * 9/5) + 32
    return temp_f

#  connect to network
print()
print("Connecting to WiFi")

#  set static IP address
ipv4 =  ipaddress.IPv4Address("192.168.1.42")
netmask =  ipaddress.IPv4Address("255.255.255.0")
gateway =  ipaddress.IPv4Address("192.168.1.1")
wifi.radio.set_ipv4_address(ipv4=ipv4,netmask=netmask,gateway=gateway)
#  connect to your SSID
wifi.radio.connect(os.getenv('CIRCUITPY_WIFI_SSID'), os.getenv('CIRCUITPY_WIFI_PASSWORD'))

print("Connected to WiFi")
pool = socketpool.SocketPool(wifi.radio)
server = Server(pool, "/static")

#  variables for HTML
temp_test = str(c_to_f(microcontroller.cpu.temperature))
unit = "F"
i = 0
istr = str(i)
#  font for HTML
font_family = "monospace"

#  the HTML script
#  setup as an f string
#  this way, can insert string variables from code.py directly
#  of note, use {{ and }} if something from html *actually* needs to be in brackets
#  i.e. CSS style formatting
def webpage():
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
    <meta http-equiv="Content-type" content="text/html;charset=utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
    html{{font-family: {font_family}; background-color: lightgrey;
    display:inline-block; margin: 0px auto; text-align: center;}}
      h1{{color: deeppink; width: 200; word-wrap: break-word; padding: 2vh; font-size: 20px;}}
      p{{font-size: 15px; width: 200; word-wrap: break-word;}}
      .button{{font-family: {font_family};display: inline-block;
      background-color: black; border: none;
      border-radius: 4px; color: white; padding: 5px 5px;
      text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}}
      p.dotted {{margin: auto;
      width: 75%; font-size: 15px; text-align: center;}}
    </style>
    </head>
    <body>
    <title>Pico W HTTP Server</title>
    <h1>Pico W HTTP Server</h1>
    <br>
    <p class="dotted">This is a Pico W running an HTTP server with CircuitPython.</p>
    <br>
    <p class="dotted">The current ambient temperature near the Pico W is
    <span style="color: deeppink;">{temp_test}°{unit}</span></p><br>
    <span style="color: black;">i={istr}</span></p><br>
    <h1>Control the LED on the Pico W with these buttons:</h1><br>
    <form accept-charset="utf-8" method="POST">
    <button class="button" name="LED ON" value="ON" type="submit">LED ON</button></a></p></form>
    <p><form accept-charset="utf-8" method="POST">
    <button class="button" name="LED OFF" value="OFF" type="submit">LED OFF</button></a></p></form>
    </body></html>
    """
    return html

MIMETypes.configure(default_to="text/html")
#  route default static IP
@server.route("/")
def base(request: Request):
    #  serve the HTML f string
    #  with content type text/html
    return Response(request,body = f"{webpage()}",content_type="text/html")

#  if a button is pressed on the site
@server.route("/", "POST")
def buttonpress(request: Request):
    #  get the raw text
    raw_text = request.raw_request.decode("utf8")
    print("raw text = ")
    print(raw_text)
    #  if the led on button was pressed
    if "ON" in raw_text:
        #  turn on the onboard LED
        led.value = True
    #  if the led off button was pressed
    if "OFF" in raw_text:
        #  turn the onboard LED off
        led.value = False
    #  reload site
    return Response(request,body = f"{webpage()}", content_type="text/html")

print("starting server..")
# startup the server
try:
    server.start(str(wifi.radio.ipv4_address))
    print("Listening on http://%s:80" % wifi.radio.ipv4_address)
#  if the server fails to begin, restart the pico w
except OSError:
    time.sleep(5)
    print("restarting..")
    microcontroller.reset()

clock = time.monotonic()

while True:
    try:
        #  every 30 seconds, update temp reading
        if (clock + 30) < time.monotonic():
            i = i + 1
            istr = str(i)
            print("istr = " + istr)
            clock = time.monotonic()
            temp_test = str(c_to_f(microcontroller.cpu.temperature))

        #  poll the server for incoming/outgoing requests
        server.poll()
    except Exception as e:
        print(e)
        continue
