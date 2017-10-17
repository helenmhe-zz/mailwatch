import requests
import pprint
import json


def web_handler(request):
    if request['GET'].get('stock',None) != None:
        stock = request['GET']['stock']
    else:
        return "Missing ticker."


    to_send = "http://dev.markitondemand.com/MODApis/Api/v2/Quote/json?&symbol={}".format(stock)
    r = requests.get(to_send)
    data = r.json()
    if data.get("Message","INVALID") != "INVALID":
        return "No data found."
    else:
        last_price = data["LastPrice"]
        change_percent = str(data["ChangePercent"])[:5]


    return stock+"$"+str(last_price)+"#"+ change_percent
