#!/usr/bin/node
Tail = require('tail').Tail;

console.log("Reading "+process.argv[2]);
tail = new Tail(process.argv[2]);
buffer = new Array(60);

function AQI(I_high, I_low, C_high, C_low, C) {
  return (I_high - I_low) * (C - C_low) / (C_high - C_low) + I_low;
}
function dustDensityToAQI(density) {
  d10 = (density * 10);
 
  if (d10 <= 0) {
    return 0;
  }
  else if(d10 <= 120) {
    return AQI(50, 0, 120, 0, d10);
  }
  else if (d10 <= 354) {
    return AQI(100, 51, 354, 121, d10);
  }
  else if (d10 <= 554) {
    return AQI(150, 101, 554, 355, d10);
  }
  else if (d10 <= 1504) {
    return AQI(200, 151, 1504, 555, d10);
  }
  else if (d10 <= 2504) {
    return AQI(300, 201, 2504, 1505, d10);
  }
  else if (d10 <= 3504) {
    return AQI(400, 301, 3504, 2505, d10);
  }
  else if (d10 <= 5004) {
    return AQI(500, 401, 5004, 3505, d10);
  }
  else if (d10 <= 10000) {
    return AQI(1000, 501, 10000, 5005, d10);
  }
  else {
    return 1001;
  }
} 

tail.on("line", function(data) {
  var raw = data.split(' ')[0];
  var voltage = raw * 0.0049;
  var dustDensity = Math.max(0, voltage - 0.54) * 166.6667;
  buffer.unshift(dustDensity);
  buffer.length = 60;
  var sum = 0, validEntries = 0;
  var avg = buffer.reduce(function(prev, curr, index, array) {
    if (typeof curr === 'number') {
      validEntries++;
      return prev + curr;
     }
    else return prev;
  }, 0) / validEntries;

  // console.log(data);
  console.log(voltage+';'+dustDensity+';'+dustDensityToAQI(dustDensity));
  console.error('[' + new Date() + '] ' + raw + " => " + voltage.toFixed(2) + " V => " + dustDensity.toFixed(3) + " ug/m^3 => avg " + avg.toFixed(3) + " ug/m^3 => AQI: " + dustDensityToAQI(avg).toFixed(0));
});