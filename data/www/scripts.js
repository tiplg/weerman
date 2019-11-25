var Socket;
var data;

const Windrichtingen = ["N", "NE", "E", "SO", "S", "SW", "W", "NW"];
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
  //Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
  Socket = new WebSocket("ws://192.168.0.13:81/");

  Socket.onmessage = function(event) {
    data = event.data.split(",");
    //console.log(data);
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
}

function printWindrichting() {
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
      document.getElementById("ptemperatuur").innerHTML = data[2] + " &#8451"; //TODO if NaN
      break;
    case TemperatuurModes.FAHRENHEIT:
      var temp = (parseFloat(data[2]) * 9) / 5 + 32;
      document.getElementById("ptemperatuur").innerHTML =
        temp.toFixed(1) + " &#8457"; //TODO if NaN
      break;
    case TemperatuurModes.KELVIN:
      var temp = parseFloat(data[2]) + 273.15;
      document.getElementById("ptemperatuur").innerHTML =
        temp.toFixed(1) + " K"; //TODO if NaN
      break;
  }
}

function printSnelheid() {
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
      document.getElementById("phumidity").innerHTML = data[3] + " &#37"; //TODO if NaN
      break;
    case HumidityModes.PROMILLE:
      var temp = parseFloat(data[3]) * 10;
      document.getElementById("phumidity").innerHTML =
        temp.toFixed(1) + " &#8240"; //TODO if NaN
      break;
  }
}
