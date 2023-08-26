<style
    type="text/css">.input {
    display: block;
    margin-top: 10px;
}

.input span {
    width: 100px;
    float: left;
    height: 36px;
    line-height: 36px;
}

.input input {
    height: 30px;
    width: 200px;
}

.btn {
    width: 110px;
    height: 50px;
    background-color: #438EF0;
    border-radius: 8px;
    font-size: 20px;
    color: #ffffff;
    border-color: #438EF0;
    margin-top: 16px;
    margin-left: 104px;
}
</style>
<form method="POST" action="configwifi">
    <label class="input">
        <span>WiFi SSID</span>
        <input type="text" name="ssid" value="">
    </label>
    <label class="input">
        <span>WiFi PASS</span>
        <input type="text" name="pass">
    </label>
    <label class="input">
        <span>Klipper IP</span>
        <input type="text" name="klipper">
    </label>
    <input class="btn" type="submit" name="submit" value="SUBMIT">
</form>

