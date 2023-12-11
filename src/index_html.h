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
    }
    .card-title {
      font-size: 1.2rem;
      font-weight: bold;
      color: #034078
    }
    input[id="sys-btn"] {
      border: none;
      color: #FEFCFB;
      background-color: #C02E2F;
      padding: 15px 15px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      width: 230px;
      height: 45;
      margin-right: 0px;
      margin-bottom: 0px;
      border-radius: 45px;
      transition-duration: 0.4s;
    }
    input[id="submit-btn"] {
      border: none;
      color: #FEFCFB;
      background-color: #0085FF;
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
      border-radius: 45px;
      transition-duration: 0.4s;
    }
    input[type=submit]:hover {
      background: #0085FF;
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
      border-radius: 4px;
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
      background-color: #0085FF;
      text-align: center;
      text-decoration: none;
      font-size: 16px;
      border-radius: 0px;
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
      background: #0085FF;
      font-size: 16px;
      font-weight: 400;
      color: #fff;
    }
    button:hover {
      background: #0085FF;
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
  </style>
  <style>
    body {
        padding: 0;
        margin: 0;
    }
    .dialog_outter {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
    }
    .dialog_wrapper {
        width: 400px;
        background: #fff;
        border: 1px solid #f0f0f0;
        border-radius: 10px;
        padding: 10px;
        position: relative;
        box-sizing: border-box;
    }

    .dialog_close_btn {
        position: absolute;
        right: 20px;
        top: 10px;
        cursor: pointer;
    }

    .dialog_title {
        font-size: 18px;
        font-weight: 500;
        text-align: center;
    }

    .dialog_content {
        padding: 20px;
        box-sizing: border-box;
        text-align: center;
    }

    .dialog_btn_group {
        display: flex;
        justify-content: flex-end;
    }

    .btn_confirm {
        background-color: Blue;
        color: #fff;
        padding: 5px 10px;
        border: 1px solid blue;
        border-radius: 4px;
        margin-right: 10px;
        cursor: pointer;
    }

    .btn_cancel {
        background-color: #fff;
        padding: 5px 10px;
        border: 1px solid #000;
        border-radius: 4px;
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
</style>
  <script>
      function validateRestartForm(){
        return confirm("KNOMI will disconnect the network connection and restart\n\nPlease reconnect after KNOMI restart");
      }
      function validateKlipperForm(){
        var ip = document.getElementById("ip").value;
        var port = document.getElementById("port").value;
        var tool = document.getElementById("tool").value;
        return confirm("Klipper IP: " + ip + "\nKlipper Port: " + port + "\nTool ID: " + tool);
      }
      function validateKnomiForm(){
        var obj_mode = document.getElementById("mode");
        var mode = obj_mode.options[obj_mode.selectedIndex].text;
        var ssid = document.getElementById("ap-ssid").value;
        var pwd = document.getElementById("ap-pwd").value;
        var name = document.getElementById("hostname").value;
        return confirm("WiFi mode: " + mode + "\nAP SSID: " + ssid + "\nAP PWD: " + pwd + "\nhostname: " + name);
      }
      function validateAPForm() {
        let ssid = document.getElementById("ap-ssid").value;;
        console.log(document.getElementById("ap-ssid").value);
        if (ssid == "" || ssid == "empty") {
          alert("You must enter a valid ap-ssid.");
          return false;
        }
        return validateKnomiForm();
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
        <form name="wifidata" action="/" method="POST" onsubmit="return validateKlipperForm()">
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
          <input type ="submit" id="submit-btn" value ="Submit">
        </form>
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
      <input type ="submit" class="refresh-btn" name="refresh" value ="Refresh">
    </form>

    <div class="card-grid">
      <div class="card">
        <form name="wifimode" action="/" method="POST" onsubmit="return validateAPForm()">
          <select id ="mode" name="mode">
            <option value="ap" $ap$>AP</option>
            <option value="sta" $sta$>STA</option>
            <option value="apsta" $apsta$>AP+STA</option>          <!--selected默认选择-->
          </select>
          <label class="ant-form-item-row">
              <span>AP SSID:&nbsp</span>
              <input type="text" id ="ap-ssid" name="ap_ssid" $ap_ssid$ minlength="1" maxlength="32">
          </label>
          <label class="ant-form-item-row">
              <span>AP PWD:&nbsp</span>
              <input type="text" id ="ap-pwd" name="ap_password" $ap_password$ minlength="6" maxlength="64">
          </label>
          <label class="ant-form-item-row">
              <span>Hostname:&nbsp</span>
              <input type="text" id ="hostname" name="hostname" $hostname$ maxlength="15">
          </label>
          <input type ="submit" id="submit-btn" value ="Submit">
        </form>
      </div>
    </div>
    <div class="card-grid">
      <div class="card" style="flex-direction: column">
         <!-- <br><label id="FIRMWARE">Firmware</label><br><br><br> -->
        <a href="update"><input type="submit" id="sys-btn" value ="Update Firmware"></a>
        <span style="width: 10px;"></span>
        <form name="Restart" action="/" method="POST" onsubmit="return validateRestartForm()">
          <input type ="submit" style="background-color: #000000" id="sys-btn" name="restart" value="Restart">
        </form>
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
        document.getElementById("ssid").value  = modalBtns[index].getElementsByClassName("ssid")[0].innerHTML;
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
