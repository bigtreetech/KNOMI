<script lang="ts">
    import {Route, router, active} from 'tinro';
    import voronLogo from './assets/voron.svg'

    var id = "unknown";

    var ssid = "";
    var pass = "";
    var ip = "";
    var hash = "";
    var branch = "";
    var gitTimestamp = "";
    var buildTimestamp = "";

    var isSaving = false;

    async function load() {
        let response = await fetch("/api/status");
        let json = await response.json();
        id = JSON.stringify(json);

        ssid = json.ssid;
        pass = json.pass;
        ip = json.ip;
        hash = json.hash;
        branch = json.branch;
        gitTimestamp = new Date(json.gitTimestamp).toLocaleString();
        buildTimestamp = new Date(json.buildTimestamp).toLocaleString();
    }

    async function saveSetup() {
        isSaving = true;
        console.log(ssid + "/" + pass + "/" + ip);
        const data = new URLSearchParams();
        data.append("ssid", ssid);
        data.append("pass", pass);
        data.append("klipper", ip);

        const res = await fetch('/api/configwifi', {
            method: "POST",
            body: data
        })
        if (res.status == 200) {
            isSaving = false;
            router.goto("/setupdone");
        } else if (res.status == 500) {
            const error = res.json().error;
            alert(error);
        }
        isSaving = false;
    }

    load();
</script>

<main>
    <nav>
        <ul>
            <li>
                <span class="logo">{@html voronLogo}</span>
                <a href="/" use:active>Setup</a>
            </li>
        </ul>
        <ul>
            <li>
            </li>
        </ul>
    </nav>

    <Route path="/">
        <div>
            <form on:submit|preventDefault={saveSetup}>
                <h3>Setup your KNOMI display</h3>
                <label class="input">
                    <span>WiFi SSID</span>
                    <input disabled={ isSaving } type="text" bind:value="{ssid}">
                </label>
                <label class="input">
                    <span>WiFi PASS</span>
                    <input disabled={ isSaving } type="text" bind:value="{pass}">
                </label>
                <label class="input">
                    <span>Klipper IP</span>
                    <input disabled={ isSaving } type="text" bind:value="{ip}">
                </label>
                <button disabled={ isSaving } type=submit>SAVE</button>
            </form>
        </div>
    </Route>
    <Route path="/setupdone">
        <span>
            Submission successful!
        </span>
        <span>
            You may now close this page.
        </span>
    </Route>

    <footer>
        <hr/>
        <p class="read-the-docs">
            <small>
                Firmware built from <span
                    title="Commit {hash} from {gitTimestamp}"><b>{branch}</b> at <b>{buildTimestamp}</b></span>. <br/>
                Check <a href="https://github.com/DiverOfDark/KNOMI" target="_blank">repository</a> for more details.
            </small>
        </p>
    </footer>
</main>
<style>
    .logo {
        width: 64px;
        height: 64px;
        display: inline-block;
        padding: 1em;
        padding-left: 0;
        box-sizing: content-box;
        will-change: filter;
        transition: filter 300ms;
    }

    nav a {
        padding: 1em;
    }

    nav a:hover {
        filter: drop-shadow(0 0 1px #ffffffff);
    }

    .logo:hover {
        filter: drop-shadow(0 0 1em #ffffffff);
    }

    .read-the-docs {
        color: #888;
    }
</style>