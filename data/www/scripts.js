var Socket;
var data = [%DATA_TEMPLATE%]; //[%DATA_TEMPLATE%];
var tornado = false;

const Windrichtingen = ["N", "NO", "O", "ZO", "Z", "ZW", "W", "NW"];
var WindDeler = 360 / Windrichtingen.length;

const RichtingModes = {
  DEGREE: 0,
  COMPASS: 1
};

const TemperatuurModes = {
  CELSIUS: 0,
  FAHRENHEIT: 1,
  KELVIN: 2
};

const SnelheidModes = {
  METERPERSEC: 0,
  KILOMETERPERHOUR: 1,
  MILESPERHOUR: 2,
  KNOTS: 3
};

const HumidityModes = {
  PERCENT: 0,
  PROMILLE: 1
};

var richtingMode = RichtingModes.DEGREE;
var temperatuurMode = TemperatuurModes.CELSIUS;
var snelheidMode = SnelheidModes.METERPERSEC;
var humidityMode = HumidityModes.PERCENT;

function init() {
  Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
  //Socket = new WebSocket("ws://192.168.0.19:81/");

  Socket.onmessage = function(event) {
    data = event.data.split(",");

    printSnelheid();
    printWindrichting();
    printTemperatuur();
    printHumidity();
  };

  document.getElementById("divwindsnelheid").onmousedown = function() {
    switch (snelheidMode) {
      case SnelheidModes.METERPERSEC:
        snelheidMode = SnelheidModes.KILOMETERPERHOUR;
        break;
      case SnelheidModes.KILOMETERPERHOUR:
        snelheidMode = SnelheidModes.MILESPERHOUR;
        break;
      case SnelheidModes.MILESPERHOUR:
        snelheidMode = SnelheidModes.KNOTS;
        break;
      case SnelheidModes.KNOTS:
        snelheidMode = SnelheidModes.METERPERSEC;
        break;
    }

    printSnelheid();
  };

  document.getElementById("divwindrichting").onmousedown = function() {
    switch (richtingMode) {
      case RichtingModes.DEGREE:
        richtingMode = RichtingModes.COMPASS;
        break;
      case RichtingModes.COMPASS:
        richtingMode = RichtingModes.DEGREE;
        break;
    }

    printWindrichting();
  };

  document.getElementById("divtemperatuur").onmousedown = function() {
    switch (temperatuurMode) {
      case TemperatuurModes.CELSIUS:
        temperatuurMode = TemperatuurModes.FAHRENHEIT;
        break;
      case TemperatuurModes.FAHRENHEIT:
        temperatuurMode = TemperatuurModes.KELVIN;
        break;
      case TemperatuurModes.KELVIN:
        temperatuurMode = TemperatuurModes.CELSIUS;
        break;
    }

    printTemperatuur();
  };

  document.getElementById("divhumidity").onmousedown = function() {
    switch (humidityMode) {
      case HumidityModes.PERCENT:
        humidityMode = HumidityModes.PROMILLE;
        break;
      case HumidityModes.PROMILLE:
        humidityMode = HumidityModes.PERCENT;
        break;
    }

    printHumidity();
  };

  printSnelheid();
  printWindrichting();
  printTemperatuur();
  printHumidity();
}

function printWindrichting() {
  document.getElementById("compass").rotate(data[1] - 45);

  switch (richtingMode) {
    case RichtingModes.DEGREE:
      document.getElementById("pwindrichting").innerHTML = data[1] + " &#176";
      break;
    case RichtingModes.COMPASS:
      var temp = parseInt(data[1]) + WindDeler / 2;
      if (temp > 360) temp -= 360;
      document.getElementById("pwindrichting").innerHTML =
        Windrichtingen[parseInt(temp / WindDeler)];
      break;
  }
}

function printTemperatuur() {
  switch (temperatuurMode) {
    case TemperatuurModes.CELSIUS:
      document.getElementById("ptemperatuur").innerHTML = data[2] + " &#8451";
      break;
    case TemperatuurModes.FAHRENHEIT:
      var temp = (parseFloat(data[2]) * 9) / 5 + 32;
      document.getElementById("ptemperatuur").innerHTML =
        temp.toFixed(1) + " &#8457";
      break;
    case TemperatuurModes.KELVIN:
      var temp = parseFloat(data[2]) + 273.15;
      document.getElementById("ptemperatuur").innerHTML =
        temp.toFixed(1) + " K";
      break;
  }
}

function printSnelheid() {
  if (data[0] > 33 && !tornado) {
    document.getElementById("windfaan").classList.remove("icon-air-sock1");
    document.getElementById("windfaan").classList.add("icon-tornado");
    tornado = true;
  } else if (data[0] < 33 && tornado) {
    document.getElementById("windfaan").classList.remove("icon-tornado");
    document.getElementById("windfaan").classList.add("icon-air-sock1");
    tornado = false;
  }

  switch (snelheidMode) {
    case SnelheidModes.METERPERSEC:
      document.getElementById("pwindsnelheid").innerHTML = data[0] + " m/s"; //TODO if INF
      break;
    case SnelheidModes.KILOMETERPERHOUR:
      var temp = parseFloat(data[0]) * 3.6;
      document.getElementById("pwindsnelheid").innerHTML =
        temp.toFixed(1) + " km/h"; //TODO if INF
      break;
    case SnelheidModes.MILESPERHOUR:
      var temp = parseFloat(data[0]) * 2.236936;
      document.getElementById("pwindsnelheid").innerHTML =
        temp.toFixed(1) + " mph"; //TODO if INF
      break;
    case SnelheidModes.KNOTS:
      var temp = parseFloat(data[0]) * 1.943844;
      document.getElementById("pwindsnelheid").innerHTML =
        temp.toFixed(1) + " kn"; //TODO if INF
      break;
  }
}

function printHumidity() {
  switch (humidityMode) {
    case HumidityModes.PERCENT:
      document.getElementById("phumidity").innerHTML = data[3] + " &#37";
      break;
    case HumidityModes.PROMILLE:
      var temp = parseFloat(data[3]) * 10;
      document.getElementById("phumidity").innerHTML =
        temp.toFixed(1) + " &#8240";
      break;
  }
}

Object.prototype.rotate = function(d) {
  var s = "rotate(" + d + "deg)";
  if (this.style) {
    // regular DOM Object
    this.style.MozTransform = s;
    this.style.WebkitTransform = s;
    this.style.OTransform = s;
    this.style.transform = s;
  } else if (this.css) {
    // JQuery Object
    this.css("-moz-transform", s);
    this.css("-webkit-transform", s);
    this.css("-0-transform", s);
    this.css("transform", s);
  }
  this.setAttribute("rotation", d);
};
