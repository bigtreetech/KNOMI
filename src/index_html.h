const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html style="font-family: Arial, Helvetica, sans-serif;
display: inline-block;
text-align: center;">
<head>
  <title>BTT KNOMI SETTINGS MANAGER</title>
  <link rel='shortcut icon' type='image/x-icon' href='/favicon.ico' />
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2 {
      font-size: 0.8rem;
      color: white;
    }
    p {
      font-size: 1.4rem;
    }
    .topnav {
      overflow: hidden;
      background-color: #C02E2F;
    }
    body {
      margin: 0;
    }
    .content {
      padding: 5%;
    }
    .card-grid {
      max-width: 800px;
      margin: 0 auto;
      display: flex;
      justify-content: center;
      align-items: center;
      display: block;
    }
    .card {
      background-color: white;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      padding: 15px 15px;
      border: 1px solid lightgrey;
      flex: 1;
      min-height: 100px;
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
    }
    .card-title {
      font-size: 1.2rem;
      font-weight: bold;
      color: #034078
    }
    input[id="sys-btn"] {
      border: none;
      color: #FEFCFB;
      padding: 15px 15px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      width: 230px;
      height: 45;
      margin-right: 0px;
      margin-bottom: 0px;
      border-radius: 5px;
      transition-duration: 0.4s;
    }
    input[id="submit-btn"] {
      border: none;
      color: #FEFCFB;
      background-color: #C02E2F;
      padding: 10px 10px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      width: 100px;
      height: 45;
      margin-left: 20px;
      margin-right: 0px;
      margin-bottom: 0px;
      border-radius: 5px;
      transition-duration: 0.4s;
    }
    input[type=submit]:active {
      opacity: 0.8;
    }
    p {
      display: flex;
      flex-flow: column;
      height: 100%;
      width: 100%;
    }
    input[type=text], input[type=password], select {
      width: 100%;
      padding: 12px 12px;
      border: 1px solid #ccc;
      margin-right: 0px;
      margin-left: 0px;
      border-radius: 5px;
      box-sizing: border-box;
      display: table-cell;
    }
    label {
      font-size: 1.2rem;
      text-align: center;
      padding: 10px;
      display: flex;
    }
    .value{
      font-size: 1.2rem;
      color: #1282A2;
    }
    .refresh-btn{
      max-width: 800px;
      margin: 0 auto;
      position: relative;
      bottom: 0;
      left: 0;
      right: 0;
      width: 100%;
      border: none;
      color: #FEFCFB;
      background-color: #C02E2F;
      text-align: center;
      text-decoration: none;
      font-size: 16px;
      border-radius: 5px;
      transition-duration: 0.4s;
    };
    .state {
      font-size: 1.2rem;
      color: #1282A2;
    }
    .table-container {
        height: 13em;
        box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
        max-width: 800px;
        margin: 0 auto;
        position: relative;
    }
    table {
        display: flex;
        flex-flow: column;
        height: 100%;
        width: 100%;
        padding: 5px;
    }
    table thead {
        /* head takes the height it requires,
        and it's not scaled when table is resized */
        flex: 0 0 auto;
        width: calc(100% - 0.9em);
    }
    table tbody {
        /* body takes all the remaining available space */
        flex: 1 1 auto;
        display: block;
        overflow-y: scroll;
    }
    table tbody tr {
        width: 100%;
    }
    table thead,
    table tbody tr {
        display: table;
        table-layout: fixed;
    }
    table {
        border: 1px solid lightgrey;
    }
    table td, table th {
        padding: 0.3em;
        text-align: center;
    }
    table th {
        background-color: #C02E2F;
        color: white;
    }
    tr:nth-child(even) {background-color: #f2f2f2;}
    .modal {
      display: none;
      position: fixed;
      z-index: 8;
      left: 0;
      top: 0;
      width: 100%;
      height: 100%;
      overflow: auto;
      background-color: rgb(0, 0, 0);
      background-color: rgba(0, 0, 0, 0.4);
    }
    .modal-content {
      margin: 300px auto;
      border: 0px solid #999;
      width: 60%;
    }
    h2,
    p {
      margin: 0 0 20px;
      font-weight: 400;
      color: #999;
    }
    span {
      color: #666;
      display: block;
      padding: 0 0 5px;
      white-space: nowrap;
    }
    .modal_form {
      padding: 25px;
      margin: 25px;
      box-shadow: 0 2px 5px #f5f5f5;
      background: #eee;
    }
    input,
    textarea {
      width: 90%;
      padding: 10px;
      border: 1px solid #1c87c9;
      outline: none;
    }
    .contact-form button {
      width: 100%;
      padding: 10px;
      border: none;
      background: #C02E2F;
      font-size: 16px;
      font-weight: 400;
      color: #fff;
    }
    button:active {
      opacity: 0.8;
    }
    .close {
      color: #aaa;
      float: right;
      font-size: 28px;
      font-weight: bold;
    }
    .close:hover,
    .close:focus {
      color: black;
      text-decoration: none;
      cursor: pointer;
    }
    .ant-form-item-row{
      display: flex;
      justify-content: center;
      align-items: center;
      margin: 20px 0;
    }
    body {
        padding: 0;
        margin: 0;
    }

    .btn_confirm {
        background-color: Blue;
        color: #fff;
        padding: 5px 10px;
        border: 1px solid blue;
        border-radius: 5px;
        margin-right: 10px;
        cursor: pointer;
    }

    .btn_cancel {
        background-color: #fff;
        padding: 5px 10px;
        border: 1px solid #000;
        border-radius: 5px;
        margin-right: 10px;
        cursor: pointer;
    }

    .form_item span {
        display: inline-block;
        width: 100px;
    }

    .form_item input{
        width: 200px;
        height: 25px;
        line-height: 25px;
        outline: none;
        border-radius: 5px;
    }

    .form_item {
        margin-bottom: 10px;
    }

    .form_item:last-child {
        margin-bottom: 0;
    }

    .overlap {
        background: #000;
        opacity: 0.6;
        width: 100%;
        height: 100%;
        display: none;
        position: absolute;
    }

    #popup_id {
        position: fixed;
        left: 0px;
        top: 0px;
        width: 100%;
        height: 100%;
        font-size: 16px;
        background-color: rgba(0, 0, 0, 0.5);
        filter: progid:DXImageTransform.Microsoft.gradient(startColorstr=#7f000000,endColorstr=#7f000000);
        display: none;
    }
    .popup {
        background-color: #ffffff;
        max-width: 400px;
        min-width: 200px;
        height: auto;
        border-radius: 5px;
        margin: 100px auto;
        text-align: center;
    }
    .popup_title {
        height: 60px;
        line-height: 60px;
        border-bottom: solid 1px #cccccc;
    }
    .popup_content {
        height: auto;
        line-height: 25px;
        padding: 15px 10px;
        text-align: left;
        color: #000000;
    }
    .popup_btn {
        padding-bottom: 10px;
    }
    .popup_btn button {
        color: #778899;
        width: 40%;
        height: 40px;
        cursor: pointer;
        border: solid 1px #cccccc;
        border-radius: 5px;
        margin: 5px 10px;
        color: #ffffff;
        background-color: #C02E2F;
    }
</style>
  <script>
        var popup_clicked = false;
        var popup_btn = false;
        async function waitPopupBtn(){
            await new Promise( (resolve) =>{
                let timer = setInterval(() =>{
                    if(popup_clicked){
                        clearInterval(timer)
                        resolve(true)
                    }
                },100)
            })
            var popup_id = document.getElementById("popup_id");
            popup_id.style.display = "none";
            return Promise.resolve(true)
        }
        async function showPopupKlipper(){
            popup_clicked = false;
            popup_btn = false;
            var ip = document.getElementById("ip").value;
            var port = document.getElementById("port").value;
            var tool = document.getElementById("tool").value;
            document.getElementById("popup_title_id").innerHTML="Klipper Config";
            document.getElementById("popup_content_id").innerHTML="<div>" + "Klipper IP: "+ "<font color='#C02E2F'>" + ip + "</font><br>"
                                                                + "Klipper Port: " + "<font color='#C02E2F'>" + port + "</font><br>"
                                                                + "Tool ID: " + "<font color='#C02E2F'>" + tool + "</font><br></div>";

            var popup_id = document.getElementById("popup_id");
            popup_id.style.display = "block";
            await waitPopupBtn();
            if (popup_btn) {
                document.getElementById('klipper-form').submit();
            }
            return popup_btn;
        }
        async function showPopupKnomi(){
            popup_clicked = false;
            popup_btn = false;
            var obj_mode = document.getElementById("mode");
            var mode = obj_mode.options[obj_mode.selectedIndex].text;
            var ssid = document.getElementById("ap-ssid").value;
            var pwd = document.getElementById("ap-pwd").value;
            var name = document.getElementById("hostname").value;
            document.getElementById("popup_title_id").innerHTML="KNOMI Config";
            document.getElementById("popup_content_id").innerHTML="<div>" + "WiFi mode: "+ "<font color='#C02E2F'>" + mode + "</font><br>"
                                                                + "AP SSID: " + "<font color='#C02E2F'>" + ssid + "</font><br>"
                                                                + "AP PWD: " + "<font color='#C02E2F'>" + pwd + "</font><br>"
                                                                + "Hostname: " + "<font color='#C02E2F'>" + name + "</font><br></div>";

            var popup_id = document.getElementById("popup_id");
            popup_id.style.display = "block";
            await waitPopupBtn();
            if (popup_btn) {
                document.getElementById('knomi-form').submit();
            }
            return popup_btn;
        }
        async function showPopupRestart(){
            popup_clicked = false;
            popup_btn = false;
            document.getElementById("popup_title_id").innerHTML="Restart?";
            document.getElementById("popup_content_id").innerHTML="KNOMI will disconnect the network connection and restart<br>Please reconnect after KNOMI restart."

            var popup_id = document.getElementById("popup_id");
            popup_id.style.display = "block";
            await waitPopupBtn();
            if (popup_btn) {
                document.getElementById('restart-form').submit();
            }
            return popup_btn;
        }
        function popupConfirm(){
            popup_clicked = true;
            popup_btn = true;
        }
        function popupCancel(){
            popup_clicked = true;
            popup_btn = false;
        }
  </script>
</head>
<body>
  <div class="topnav">
    <h1>BTT KNOMI SETTINGS MANAGER ;)</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <form id="klipper-form" name="klipper-form" action="/" method="POST">
            <label class="ant-form-item-row">
                <span>Klipper IP:&nbsp</span>
                <input type="text" id ="ip" name="ip" $ip$ maxlength="64" placeholder="1.2.3.4 or printer.local">
            </label>
            <label class="ant-form-item-row">
                <span>Klipper Port:&nbsp</span>
                <input type="text" id ="port" name="port" $port$ maxlength="5">
            </label>
            <label class="ant-form-item-row">
                <span>Tool ID:&nbsp</span>
                <input type="text" id ="tool" name="tool" $tool$ maxlength="6">
            </label>
        </form>
        <input type ="submit" id="submit-btn" value ="Submit" onclick="showPopupKlipper()">
      </div>
    </div>
    <div id="modalOne" class="modal">
      <div class="modal-content">
        <div class="contact-form">
          <a class="close">&times;</a>
          <form class="modal_form" action="/" method="POST">
            <div>
              <input readonly id ="ssid" class="fname" type="text" name="ssid" placeholder="ssid" />
              <span></span>
              <input class="fname" type="text" name="password" placeholder="password" />
              <span></span>
            </div>
            <button type="submit" >Connect</button>
          </form>
        </div>
      </div>
    </div>
    <div class="table-container">
      <table>
        <thead>
          <tr>
            <th>SSID</th>
            <th>RSSI</th>
            <th>STATUS</th>
          </tr>
        </thead>
        <tbody>
          $wifi_list$
        </tbody>
      </table>
    </div>
    <form name="refresh" action="/" method="POST">
      <input type ="submit" class="refresh-btn" name="refresh" value="Refresh">
    </form>

    <div class="card-grid">
      <div class="card">
        <form id="knomi-form" name="knomi-form" action="/" method="POST">
          <select id ="mode" name="mode">
            <option value="ap" $ap$>AP</option>
            <option value="sta" $sta$>STA</option>
            <option value="apsta" $apsta$>AP+STA</option>
          </select>
          <label class="ant-form-item-row">
              <span>AP SSID:&nbsp</span>
              <input type="text" id ="ap-ssid" name="ap_ssid" $ap_ssid$ minlength="1" maxlength="32" required="required">
          </label>
          <label class="ant-form-item-row">
              <span>AP PWD:&nbsp</span>
              <input type="text" id ="ap-pwd" name="ap_password" $ap_password$ minlength="6" maxlength="64">
          </label>
          <label class="ant-form-item-row">
              <span>Hostname:&nbsp</span>
              <input type="text" id ="hostname" name="hostname" $hostname$ maxlength="15">
          </label>
        </form>
        <input type ="submit" id="submit-btn" value ="Submit" onclick="showPopupKnomi()">
      </div>
    </div>
    <div class="card-grid">
      <div class="card">
        <a href="update"><input type="submit" style="background-color: #C02E2F" id="sys-btn" value ="Update Firmware"></a>
        <span style="width: 10px;"></span>
        <form id="restart-form" name="restart-form" action="/" method="POST">
            <input type="hidden" name="restart">
        </form>
        <input type ="submit" style="background-color: #000000" id="sys-btn" value="Restart" onclick="showPopupRestart()">
      </div>
    </div>
    <div id="popup_id">
      <div class="popup">
        <p id="popup_title_id" class="popup_title"></p>
        <p id="popup_content_id" class="popup_content"></p>
        <div class="popup_btn">
          <button style="background-color: #525252" onclick="popupCancel()">Cancel</button>
          <button onclick="popupConfirm()">Confirm</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    let modalBtns = [...document.querySelectorAll(".showpop")];
    modalBtns.forEach(function (btn, index) {
      btn.onclick = function (event) {
        let modal = btn.getAttribute("data-modal");
        document.getElementById(modal).style.display = "block";
        console.log(modalBtns[index].getElementsByClassName("ssid")[0].innerHTML,document.getElementById("ssid"))
        document.getElementById("ssid").value = modalBtns[index].getElementsByClassName("ssid")[0].innerHTML;
      };
    });
    let closeBtns = [...document.querySelectorAll(".close")];
    closeBtns.forEach(function (btn) {
      btn.onclick = function () {
        let modal = btn.closest(".modal");
        modal.style.display = "none";
      };
    });
    window.onclick = function (event) {
      if (event.target.className === "modal") {
        event.target.style.display = "none";
      }
    };
  </script>
</body>
</html>)rawliteral";
