<script lang="ts">
    import {Route} from 'tinro';
    import voronLogo from './assets/voron.svg'

    import SetupDone from "./lib/SetupDone.svelte";
    import Setup from "./lib/Setup.svelte";

    var id = "unknown";

    var ssid = "";
    var pass = "";
    var ip = "";

    async function load() {
        let response = await fetch("/api/status");
        let json = await response.json();
        id = JSON.stringify(json);

        ssid = json.ssid;
        pass = json.pass;
        ip = json.ip;
    }

    load();
</script>

<main>
    <nav>
        <ul>
            <li>
                <span class="logo">{@html voronLogo}</span>
                <a href="/">Setup</a>
            </li>
        </ul>
        <ul>
            <li>
            </li>
        </ul>
    </nav>

    <Route path="/">
        <Setup/>
    </Route>
    <Route path="/setupdone">
        <SetupDone/>
    </Route>

    <footer>
        <hr/>
        <p class="read-the-docs">
            <small>
                Firmware: {id}. Check our <a href="https://github.com/DiverOfDark/KNOMI" target="_blank">repository</a> for more
                details.
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