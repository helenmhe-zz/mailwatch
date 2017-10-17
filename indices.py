import requests

def web_handler(request):
	DJI = requests.get('http://www.google.com/finance?q=INDEXDJX%3A.DJI&ei=9rP_WMC6K8uxmAGIjK2gAg')
	DJI = DJI.text[70000:]

	a1 = DJI.find("class=\"pr\"")
	b1 = DJI.find("id-price-change nwp")

	DJIquote = DJI[a1 + 22: b1 - 28]
	c = DJIquote.find(">")
	DJIquote = DJIquote[c+1:]

	a2 = DJI.find("cp\">(")
	b2 = DJI.find("<span class=nwp>")
	DJIchg = DJI[a2 + 5: b2 - 37]

	###############

	IXIC = requests.get('http://www.google.com/finance?q=ixic&ei=-bT_WLHbBoO7mAGEyqbICQ')
	IXIC = IXIC.text[70001:-11000]

	a3 = IXIC.find("class=\"pr\"")
	b3 = IXIC.find("id-price-change nwp")

	IXICquote = IXIC[a3 + 22: b3 - 28]
	c = IXICquote.find(">")
	IXICquote = IXICquote[c+1:]

	a4 = IXIC.find("cp\">(")
	b4 = IXIC.find("<span class=nwp>")
	IXICchg = IXIC[a4 + 5: b4 - 37]

	################

	INX = requests.get('http://www.google.com/finance?q=INDEXSP%3A.INX&ei=JLf_WPjAB825mQHKmZ_QBg')
	INX = INX.text[67000:-11000]

	a5 = INX.find("class=\"pr\"")
	b5 = INX.find("id-price-change nwp")

	INXquote = INX[a5 + 22: b5 - 28]
	c = INXquote.find(">")
	INXquote = INXquote[c+1:]

	a6 = INX.find("cp\">(")
	b6 = INX.find("<span class=nwp>")
	INXchg = INX[a6 + 5: b6 - 37]

	return ("DJI"+DJIquote+"@" + DJIchg + "IXIC" + 
	IXICquote + "_" + IXICchg + "INX" + INXquote + "(" + INXchg)
