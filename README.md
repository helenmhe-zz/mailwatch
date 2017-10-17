# 6.s08TeensyWatch

Current Contents:

(i) Primaries class (.cpp). The Primaries class communicates with server-side code to pull time, date, temperature, and a sentence about the weather depending on GPS location. For example, Primaries Boston(42.364,-71.103) would instantiate a Primaries object for Boston. Then, Boston.initPrims(); would fetch the previously mentioned primaries. The primaries can then be accessed via Boston.getDate(); Boston.getTemp(); etc. We then locally increment time (minutes by 1) using Boston.incrTime(); 

(ii) Working demonstration of the Primaries class with Spoofed GPS data. Right now, can scroll between four major cities, selecting using button, and display the primaries for that city. (Primaries.ino)


"# mailwatch" 
