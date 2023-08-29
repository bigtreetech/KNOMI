<script lang="ts">
    import {Route} from 'tinro';
    import voronLogo from './assets/voron.svg'

    import SetupDone from "./lib/SetupDone.svelte";
    import Setup from "./lib/Setup.svelte";
</script>

<main>
    <header class="navbar">
        <section class="navbar-section">
            <span class="logo">{@html voronLogo}</span>
            <a href="/" class="navbar-brand mr-2">
                KNOMI
            </a>
            <a href="/setup">Setup</a>
        </section>
    </header>

    <Route path="/"><h1>This is the main page</h1></Route>
    <Route path="/setup"><Setup/></Route>
    <Route path="/setupdone"><SetupDone/></Route>

    <p class="read-the-docs">
        Check our <a href="https://github.com/DiverOfDark/KNOMI" target="_blank">repository</a> for more details.
    </p>
</main>
<style>
    .logo {
        width: 32px;
        height: 32px;
        display: inline-block;
        padding: 1em;
        box-sizing: content-box;
        will-change: filter;
        transition: filter 300ms;
    }

    header a {
        padding: 1em;
    }

    header a:hover {
        filter: drop-shadow(0 0 1em #ffffffff);
    }

    .logo:hover {
        filter: drop-shadow(0 0 1em #ffffffff);
    }

    .read-the-docs {
        color: #888;
    }
</style>