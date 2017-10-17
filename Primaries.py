import requests
import pprint
import json
import time

ts = time.time() 

api = "AIzaSyBzyHs1n4f7DwaUvV9xdtmSbTqBSGnQw64"

def web_handler(request):
    if request['GET'].get('location',None) != None:
        location = request['GET']['location']
    else:
        return "Missing Location"


    #use the string below for properly formatted wikipedia api access (https://www.mediawiki.org/wiki/API:Main_page)
    to_send = "https://api.darksky.net/forecast/283753fa9cbe676d89a9c99c1d46646f/{}".format(location)
    r = requests.get(to_send)
    data = r.json()
    temp = data["currently"]["temperature"]
    skytext = data["hourly"]["summary"]

    to_send = "https://maps.googleapis.com/maps/api/timezone/json?location={}&timestamp=".format(location) + str(float(ts)) + " &key=" + api
    r = requests.get(to_send)
    data = r.json()
    offset = data["dstOffset"]
    offset2 = data["rawOffset"]
    epoch = ts + offset + offset2 + 14400

    timed = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(epoch))

    return "@" + str(timed) + "%"+  str(skytext) + "#" + str(temp) + "$" 
