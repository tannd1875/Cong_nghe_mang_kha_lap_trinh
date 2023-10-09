#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>


struct Config {
  String scanSSID; //ssid sau khi scan
  String scanPASS; // password sau khi scan
};

// giờ thay vì số đèn với năng lượng thì lấy ssid với password => ok

// Replace with your network credentials
const char* ssid = "UiTiOt-Staff";
const char* password = "UiTiOtAP111";

const char *configFilename   = "/config.json"; // ????
const char * defaultSSID        = "UiTiOt-E3.1"; // default
const char * defaultPASS        = "UiTiOtAP"; // default
unsigned long previousMillis = 0; // thời gian

Config config; // tạo 1 struct
String json;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// code của cái web
const char index_html[] PROGMEM = R"rawliteral(  
<!DOCTYPE html>
<html lang="en">

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no" />
    <title>Config ESP</title>
    <script>

    function c(l,e) {
    	console.log(l);
    	// preventDefault();
        document.getElementById('s').value = l.innerText || l.textContent;
        // document.location.href = document.location +"#wmform";
        p = l.nextElementSibling.classList.contains("l");
        document.getElementById('p').disabled = !p;
        if(p)document.getElementById('p').focus();
        return false;
    }

    </script>

<style>
.c,
body {
    text-align: center
}

div,
input {
    padding: 5px;
    font-size: 1em
}

input {
    width: 95%
}

body {
    font-family: verdana
}

button {
    cursor: pointer;
    border: 0;
    border-radius: .3rem;
    background-color: #35c9a4;
    color: #fff;
    line-height: 2.4rem;
    font-size: 1.2rem;
    width: 100%
}

button:hover {
    opacity: 0.6;
}

a {
    color: #000;
    font-weight: 700;
    text-decoration: none
}

a:hover {
    color: #1fa3ec;
    text-decoration: underline
}

.q {
    height: 16px;
    margin: 0;
    padding: 0 5px;
    text-align: right;
    min-width: 38px
}

.q.q-0:after {
    background-position-x: 0
}

.q.q-1:after {
    background-position-x: -16px
}

.q.q-2:after {
    background-position-x: -32px
}

.q.q-3:after {
    background-position-x: -48px
}

.q.q-4:after {
    background-position-x: -64px
}

.q.l:before {
    background-position-x: -80px;
    padding-right: 5px
}

.ql .q {
    float: left
}

.qr .q {
    float: right
}

.qinv .q {
    -webkit-filter: invert(1);
    filter: invert(1)
}

.q:after,
.q:before {
    content: '';
    width: 16px;
    height: 16px;
    display: inline-block;
    background-repeat: no-repeat;
    background-position: 16px 0;
    background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGAAAAAQCAMAAADeZIrLAAAAJFBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADHJj5lAAAAC3RSTlMAIjN3iJmqu8zd7vF8pzcAAABsSURBVHja7Y1BCsAwCASNSVo3/v+/BUEiXnIoXkoX5jAQMxTHzK9cVSnvDxwD8bFx8PhZ9q8FmghXBhqA1faxk92PsxvRc2CCCFdhQCbRkLoAQ3q/wWUBqG35ZxtVzW4Ed6LngPyBU2CobdIDQ5oPWI5nCUwAAAAASUVORK5CYII=');
}

@media (-webkit-min-device-pixel-ratio: 2),
(min-resolution: 192dpi) {
    .q:before,
    .q:after {
        background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAgCAMAAACfM+KhAAAALVBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAOrOgAAAADnRSTlMAESIzRGZ3iJmqu8zd7gKjCLQAAACmSURBVHgB7dDBCoMwEEXRmKlVY3L//3NLhyzqIqSUggy8uxnhCR5Mo8xLt+14aZ7wwgsvvPA/ofv9+44334UXXngvb6XsFhO/VoC2RsSv9J7x8BnYLW+AjT56ud/uePMdb7IP8Bsc/e7h8Cfk912ghsNXWPpDC4hvN+D1560A1QPORyh84VKLjjdvfPFm++i9EWq0348XXnjhhT+4dIbCW+WjZim9AKk4UZMnnCEuAAAAAElFTkSuQmCC');
        background-size: 95px 16px;
    }
}

input:disabled {
    opacity: 0.5;
}

</style>

</head>

<body>
    <div class="qr" style='text-align:left;width:260px; margin: auto;' id="wifiList">
    </div>
    <div class="qr" style='text-align:left;width:260px; margin: auto;'>
        <form id="wmform" method='get' action="submit">
            <input id='s' name='s' length=32 placeholder='SSID'>
            <br/>
            <input id='p' name='p' length=64 type='password' placeholder='password'>
            <br/>
            <br/>
        </form>
        <button style="margin-bottom:10px;" onclick="submit();">Save</button>
        <br/>
        <button class="scan_button c" onclick="scan();">Scan</button></div>
    </div>
</body>

<script>
    function preProcess({ssid, encryptionType, rssi}) {
        let wifiInfo = {
            ssid: "",
            encryptionType: "",
            rssi: 0
        }
        console.log(ssid);
        if (rssi >= 70)
            wifiInfo.rssi = "q-4";
        else 
            if (rssi >= 50)
                wifiInfo.rssi = "q-3";
            else
                    if (rssi >= 20)
                        wifiInfo.rssi = "q-2";
                    else
                        if (rssi >= 1)
                            wifiInfo.rssi = "q-1";
            if (encryptionType != 7)
                wifiInfo.encryptionType = 'l';
            if (ssid)
                wifiInfo.ssid = ssid;
        return wifiInfo;
    }
        
    function showWiFiScan({ssid, encryptionType, rssi}){
        const wifiList = document.getElementById('wifiList');
        const wifi = document.createElement("div");
        wifi.innerHTML = `
            <div>
                <a href='#p' onclick='c(this)'>${ssid}</a>
                <div role='img' class='q ${rssi} ${encryptionType}'></div>
            </div>
        `
        wifiList.appendChild(wifi);
    }

    function scan() {
        const wifiList = document.getElementById('wifiList');
        const wifiScanList = getData();
        if (wifiScanList)
            console.log("Get Data Success");
        wifiList.innerHTML = "";
        for (const wifiObj of wifiScanList) {
            showWiFiScan(preProcess(wifiObj))
        }
    }

    function submit() {
        const ssidValue = document.getElementById('s');
        const passwordValue = document.getElementById('p');
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/update?ssid_scan="+ssidValue+"&password_scan="+passwordValue, true);
        xhr.send();
        console.log(xhr);
    }

    function getData(){
        const xhr = new XMLHttpRequest();

        xhr.open("GET", "/scan");

        xhr.setRequestHeader("Accept", "application/json");

        xhr.send();

        xhr.onload = function() {
            if (xhr.status === 200) {
              const json = JSON.parse(xhr.responseText);
              console.log(json);
              return json;
            }
        }
    }
</script>
</html>
)rawliteral";

// lưu config
void saveConfiguration(const char *filename, const Config &config){
  // Mở file để ghi, tạo file nếu không tồn tại (w+)
  File file = LittleFS.open(filename, "w+");
  if (!file) {
    Serial.println("Failed to create file");
    return;
  }

  // Tạo JsonOjbect
  StaticJsonDocument<50> doc;
  doc["scanSSID"] = config.scanSSID;
  doc["scanPASS"] = config.scanPASS;

  // Chuyển JsonObject sang String và lưu vào file  // {"pinNumber": 2, "period": 1000}
  if (serializeJson(doc, file) == 0){
    Serial.println("Failed to write to file");
  }

  file.close(); // đóng file
}

void loadConfiguration(const char *filename, Config &config){
  // Mở file để đọc
  File file = LittleFS.open(filename, "r");

  // Tạo JsonObject
  StaticJsonDocument<50> doc;

  // Chuyển String sang JsonObject
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println("Failed to read file, using default configuration");
  }
  file.close();
}

void scan() {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;
  json = "";
  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0) {
    Serial.println(F("No networks found"));
  }
  else if (scanResult > 0) {
      // Print unsorted scan results
      // compute the required size
      // 1 array with "scanResult element", each element is object with 3 member
      const size_t CAPACITY = JSON_ARRAY_SIZE(scanResult) + scanResult*JSON_OBJECT_SIZE(3);

      // allocate the memory for the document
      DynamicJsonDocument doc(CAPACITY);

      // create an empty array
      JsonArray array = doc.to<JsonArray>();
      for (int8_t i = 0; i < scanResult; i++) {
        WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
        doc[i]["ssid"] = ssid;
        doc[i]["encryptionType"] = encryptionType;
        doc[i]["rssi"] = map(rssi, -100, 0, 0, 255);
      }
      serializeJson(doc, json);
  }
}


void setup(){
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("Unable to begin(), aborting");
    return;
  }

  loadConfiguration(configFilename, config);
  Serial.println();

/* Quy trình
  1. kết nối WiFi, Ok thì không sao, không OK thì thành cái AP => OK rồi
  2. khi thành AP thì dùng điện thoại dô cái webserver của arduino bằng IP => OK rồi
  3. Cái Web có tính năng Scan Wifi, hiện tên WiFi, bảo mật, mức sóng => làm local được rồi, merge cái Scan WiFi dô
  4. Lấy SSID, nhập pass, chuyển sang chế dộ Station, con arduino kết nối dô cái WiFi mới là xong
*/

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // nếu mà nó không dô được wifi thì chuyển nó thành AP
    delay(1000);
    const char *ESPssid = "ESP8266-Tan";
    const char *ESPpassword = "123456789";

  // Tạo điểm truy cập Wi-Fi
    WiFi.softAP(ESPssid, ESPpassword);

  // In địa chỉ IP của điểm truy cập Wi-Fi
    Serial.println(WiFi.softAPSSID());
    Serial.println(WiFi.softAPIP());
    scan();
    delay(5000);
    // Route for root / web page -> con server sẽ chạy cái code index_html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html);
    });

    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {  
      if (json)
        Serial.println(json);
      request->send(200, "application/json", json);
    });

    // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
    server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String inputMessage;
      // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
      // cơ bản là lấy dữ liệu từ web về rồi cập nhật dô cái struct config
      if (request->hasParam("ssid_scan")) {
        inputMessage = request->getParam("ssid_scan")->value();
        config.scanSSID = inputMessage;
      }
      if (request->hasParam("password_scan")) {
        inputMessage = request->getParam("password_scan")->value();
        config.scanPASS = inputMessage;
      }
      saveConfiguration(configFilename, config);
        request->send(200, "text/plain", "OK");
    });
    // Start server
    server.begin();    
  } 
}
  
void loop() {
}
