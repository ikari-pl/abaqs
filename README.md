Arduino Bluetooth Air Quality Station (ABAQS)
=============================================

Measures PM2.5 dust density in the air, tries to average the values, and prints debug log over serial port and USB in the following form:
```
145 => 0.71 V => 28.417 ug/m^3 => avg 17.392 ug/m^3 => AQI: 61
```

ⓒ2014 Cezar „ikari” Pokorski

## HARDWARE used:
 * Arduino Leonardo 
 * GP2Y1010AU0F dust sensor https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
 * bluetooth module http://www.lightinthebox.com/new-edition-hc06-bluetooth-wireless-module-2-pcs_p1235994.html


## CREDITS:

* This blog post and it's not so clear picture helped me wire the stuff together:
  http://sensorapp.net/sharp-dust-sensor-and-arduino/

* And then I found this almost identical project:
  https://gist.github.com/nfjinjing/8d63012c18feea3ed04e
  which I would like to credit for the AQI formula. Later I have stolen also the buffer code,
  because what the heck, it was good.
 
* And finally, the bluetooth setup that finally explained me,
  why the heck it doesn't JUST WORK:
  https://learn.sparkfun.com/tutorials/using-the-bluesmirf/example-code-using-command-mode
  
## Further export:

I currently also pipe the data into Logstach + ElasticSearch + Kibana to get some graphs. More info coming soon.