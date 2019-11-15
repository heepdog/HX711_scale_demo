const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<link rel="stylesheet" type="text/css" href="/bootstrap.css">
<head>
<body>
 
<div id="demo">
<h1>Roof Snow Weight</h1>
 
<div>
  <h2>Weight : <span id="scaleValue">0</span> grams</h2><br>
</div>

<div>
<button type="button" onclick="TareScale()">Tare Scale</button>
<span id="tareReply"></span>
</div>
<script>
 
var myvar = setInterval(getData, 2000); //2000mSeconds update rate
 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("scaleValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "/getWeight", true);
  xhttp.send();
}

function TareScale(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      //document.getElementById("tareReply").innerHTML = this.responseText;
      alert(this.responseText);
    }
  };
  xhttp.open("get", "/tare", true);
  xhttp.send();
}
</script>
<br><br><a href="http://warroadlegoleague.weebly.com/">Go Warroad FLL</a>
</body>
</html>
)=====";
