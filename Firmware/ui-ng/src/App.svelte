<script lang="ts">
    import voronLogo from './assets/voron.svg'

    import SetupDone from "./lib/SetupDone.svelte";
    import Setup from "./lib/Setup.svelte";
</script>

<main>
    <div>
      <span class="logo">
        {@html voronLogo}
      </span>
    </div>
    <h1>KNOMI</h1>

    <div class="card">
        <Setup/>
        <SetupDone/>
    </div>

    <p class="read-the-docs">
        Check our <a href="https://github.com/DiverOfDark/KNOMI" target="_blank">repository</a> for more details.
    </p>
</main>

<style>
    .logo {
        height: 6em;
        padding: 1.5em;
        will-change: filter;
        transition: filter 300ms;
    }

    .logo:hover {
        filter: drop-shadow(0 0 2em #ff3e00aa);
    }

    .read-the-docs {
        color: #888;
    }
</style>
