var Socket;
var data;

function init() {
  //Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
  Socket = new WebSocket("ws://192.168.0.13:81/");

  Socket.onmessage = function(event) {
    data = event.data.split(",");
    //console.log(data);
    document.getElementById("pwindsnelheid").innerHTML = data[0] + " m/s";
    document.getElementById("pwindrichting").innerHTML = data[1] + " &#176";
    document.getElementById("ptemperatuur").innerHTML = data[2] + " &#8451";
    document.getElementById("phumidity").innerHTML = data[3] + " %";
  };

  document.getElementById("divwindsnelheid").onmousedown = function() {
    console.log("0");
  };

  document.getElementById("divwindrichting").onmousedown = function() {
    console.log("1");
  };

  document.getElementById("divtemperatuur").onmousedown = function() {
    console.log("2");
  };

  document.getElementById("divhumidity").onmousedown = function() {
    console.log("3");
  };
}
