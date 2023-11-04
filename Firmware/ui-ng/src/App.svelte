<script lang="ts">
    import { active, Route, router } from "tinro";
    import voronLogo from "./assets/voron.svg";
    import prettyBytes from "pretty-bytes";
    import SparkMD5 from "spark-md5/spark-md5";
    import Theme from "./components/Theme.svelte";

    let fileinput: HTMLInputElement;
    let selectedFile: File;

    var ssid = "";
    var pass = "";
    var ip = "";
    var hostname = "";
    var hash = "";
    var branch = "";
    var gitTimestamp = "";
    var buildTimestamp = "";

    var isSaving = false;

    var otaSuccess = false;
    var otaError: String | boolean = false;
    var otaProgress = false;
    var otaPercentage = 0;
    var otaKind = "";

    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;

    async function load() {
        let response = await fetch("/api/status");
        let json = await response.json();

        ssid = json.ssid;
        pass = json.pass;
        ip = json.ip;
        hostname = json.hostname;
        hash = json.hash;
        branch = json.branch;
        gitTimestamp = new Date(json.gitTimestamp).toLocaleString();
        buildTimestamp = new Date(json.buildTimestamp).toLocaleString();

        initWebSocket();
    }

    function initWebSocket() {
        console.log("Trying to open a WebSocket connection…");
        websocket = new WebSocket(gateway);
        websocket.onopen = () => console.log("Connection opened");
        websocket.onclose = () => {
            console.log("Connection closed");
            setTimeout(initWebSocket, 2000);
        };
        websocket.onmessage = (event) => console.log(event.data);
    }

    async function saveSetup() {
        isSaving = true;
        console.log(ssid + "/" + pass + "/" + ip);
        const data = new URLSearchParams();
        data.append("ssid", ssid);
        data.append("pass", pass);
        data.append("klipper", ip);
        data.append("hostname", hostname);

        const res = await fetch("/api/configwifi", {
            method: "POST",
            body: data,
        });
        if (res.status == 200) {
            isSaving = false;
            router.goto("/setupdone");
        } else if (res.status == 500) {
            const error = res.json().error;
            alert(error);
        }
        isSaving = false;
    }

    const onFileSelected = (e: Event) => {
        selectedFile = e.target.files[0];
    };

    function retryOTA() {
        otaError = null;
        otaSuccess = false;
        otaUpdate();
    }

    function clear() {
        otaError = null;
        otaSuccess = false;
        otaProgress = false;
        otaPercentage = 0;
        selectedFile = null;
    }

    function fileMD5(file) {
        return new Promise((resolve, reject) => {
            const blobSlice =
                File.prototype.slice ||
                File.prototype.mozSlice ||
                File.prototype.webkitSlice;
            const chunkSize = 2097152; // Read in chunks of 2MB
            const chunks = Math.ceil(file.size / chunkSize);
            const spark = new SparkMD5.ArrayBuffer();
            const fileReader = new FileReader();
            let currentChunk = 0;

            let loadNext = () => {
                const start = currentChunk * chunkSize;
                const end =
                    start + chunkSize >= file.size
                        ? file.size
                        : start + chunkSize;

                fileReader.readAsArrayBuffer(blobSlice.call(file, start, end));
            };

            fileReader.onload = (e) => {
                spark.append(e.target!.result); // Append array buffer
                currentChunk += 1;

                if (currentChunk < chunks) {
                    loadNext();
                } else {
                    const md5 = spark.end();
                    resolve(md5);
                }
            };

            fileReader.onerror = (e) => {
                reject(e);
            };

            loadNext();
        });
    }

    async function otaUpdate() {
        otaProgress = true;
        otaPercentage = 0;
        const formData = new FormData();
        const request = new XMLHttpRequest();

        request.addEventListener("load", () => {
            // request.response will hold the response from the server
            if (request.status === 200) {
                otaSuccess = true;
            } else if (request.status !== 500) {
                otaError = `[HTTP ERROR] ${request.statusText}`;
            } else {
                otaError = request.responseText;
            }
            otaProgress = false;
            otaPercentage = 0;
        });

        // Upload progress
        request.upload.addEventListener("progress", (e) => {
            otaPercentage = Math.trunc((e.loaded / e.total) * 100);
        });

        request.withCredentials = true;

        fileMD5(selectedFile)
            .then((md5) => {
                formData.append("MD5", md5);
                formData.append("size", selectedFile.size);
                formData.append(otaKind, selectedFile, otaKind);
                request.open("post", "/update");
                request.send(formData);
            })
            .catch(() => {
                otaError =
                    "Unknown error while upload, check the console for details.";
                otaProgress = false;
                otaPercentage = 0;
            });
    }

    load();
</script>

<main>
    <nav>
        <ul>
            <li>
                <span class="logo"
                    ><!-- eslint-disable -->{@html voronLogo}<!-- eslint-enable --></span
                >
                <a href="/setup" use:active disabled={otaProgress || null}
                    >Setup</a
                >
                <a href="/update" use:active>Update</a>
                <a href="/theme" use:active disabled={otaProgress || null}
                    >Theme</a
                >
            </li>
        </ul>
        <ul>
            <li />
        </ul>
    </nav>

    <Route path="/" redirect="/setup" />
    <Route path="/setup">
        <div>
            <form on:submit|preventDefault={saveSetup}>
                <label class="input">
                    <span>WiFi SSID</span>
                    <input
                        disabled={isSaving || null}
                        type="text"
                        bind:value={ssid}
                    />
                </label>
                <label class="input">
                    <span>WiFi PASS</span>
                    <input
                        disabled={isSaving || null}
                        type="text"
                        bind:value={pass}
                    />
                </label>
                <label class="input">
                    <span>Klipper IP</span>
                    <input
                        disabled={isSaving || null}
                        type="text"
                        bind:value={ip}
                    />
                </label>
                <label class="input">
                    <span>KNOMI Hostname</span>
                    <input
                        disabled={isSaving || null}
                        type="text"
                        bind:value={hostname}
                    />
                </label>
                <button disabled={isSaving || null} type="submit">SAVE</button>
            </form>
        </div>
    </Route>
    <Route path="/setupdone">
        <span> Submission successful! </span>
        <span> You may now close this page. </span>
    </Route>
    <Route path="/update">
        {#if otaSuccess}
            <svg
                width="32px"
                height="32px"
                style="vertical-align: middle;"
                viewBox="0 0 24 24"
                version="1.1"
                xmlns="http://www.w3.org/2000/svg"
                xmlns:xlink="http://www.w3.org/1999/xlink"
            >
                <g
                    stroke="none"
                    stroke-width="1"
                    fill="none"
                    fill-rule="evenodd"
                >
                    <rect id="bound" x="0" y="0" width="24" height="24" />
                    <circle
                        id="Oval-5"
                        fill="#42BA96"
                        opacity="0.3"
                        cx="12"
                        cy="12"
                        r="10"
                    />
                    <path
                        d="M16.7689447,7.81768175 C17.1457787,7.41393107 17.7785676,7.39211077 18.1823183,7.76894473 C18.5860689,8.1457787 18.6078892,8.77856757 18.2310553,9.18231825 L11.2310553,16.6823183 C10.8654446,17.0740439 10.2560456,17.107974 9.84920863,16.7592566 L6.34920863,13.7592566 C5.92988278,13.3998345 5.88132125,12.7685345 6.2407434,12.3492086 C6.60016555,11.9298828 7.23146553,11.8813212 7.65079137,12.2407434 L10.4229928,14.616916 L16.7689447,7.81768175 Z"
                        id="Path-92"
                        fill="#42BA96"
                    />
                </g>
            </svg>
            <span style="vertical-align: middle;"> OTA Success </span>
            <br />
            <br />
            <button type="button" on:click={clear}>
                <svg
                    xmlns="http://www.w3.org/2000/svg"
                    class="pt-1"
                    width="16px"
                    height="16px"
                    viewBox="0 0 24 24"
                >
                    <g data-name="Layer 2">
                        <g data-name="arrow-back">
                            <rect
                                width="24"
                                height="24"
                                transform="rotate(90 12 12)"
                                opacity="0"
                            />
                            <path
                                fill="currentColor"
                                d="M19 11H7.14l3.63-4.36a1 1 0 1 0-1.54-1.28l-5 6a1.19 1.19 0 0 0-.09.15c0 .05 0 .08-.07.13A1 1 0 0 0 4 12a1 1 0 0 0 .07.36c0 .05 0 .08.07.13a1.19 1.19 0 0 0 .09.15l5 6A1 1 0 0 0 10 19a1 1 0 0 0 .64-.23 1 1 0 0 0 .13-1.41L7.14 13H19a1 1 0 0 0 0-2z"
                            />
                        </g>
                    </g>
                </svg>
                Back
            </button>
        {:else if otaError}
            <svg
                width="32px"
                height="32px"
                style="vertical-align: middle;"
                viewBox="0 0 24 24"
                version="1.1"
                xmlns="http://www.w3.org/2000/svg"
                xmlns:xlink="http://www.w3.org/1999/xlink"
            >
                <g
                    stroke="none"
                    stroke-width="1"
                    fill="none"
                    fill-rule="evenodd"
                >
                    <rect id="bound" x="0" y="0" width="24" height="24" />
                    <circle
                        id="Oval-5"
                        fill="#DF4759"
                        opacity="0.3"
                        cx="12"
                        cy="12"
                        r="10"
                    />
                    <rect
                        id="Rectangle-9"
                        fill="#DF4759"
                        x="11"
                        y="7"
                        width="2"
                        height="8"
                        rx="1"
                    />
                    <rect
                        id="Rectangle-9-Copy"
                        fill="#DF4759"
                        x="11"
                        y="16"
                        width="2"
                        height="2"
                        rx="1"
                    />
                </g>
            </svg>
            <span style="vertical-align: middle;"> {otaError} </span>
            <br />
            <br />
            <div class="mt-3">
                <button type="button" on:click={retryOTA}>
                    <svg
                        xmlns="http://www.w3.org/2000/svg"
                        class="pt-1"
                        width="16px"
                        height="16px"
                        viewBox="0 0 24 24"
                    >
                        <g data-name="Layer 2">
                            <g data-name="refresh">
                                <rect width="24" height="24" opacity="0" />
                                <path
                                    fill="currentColor"
                                    d="M20.3 13.43a1 1 0 0 0-1.25.65A7.14 7.14 0 0 1 12.18 19 7.1 7.1 0 0 1 5 12a7.1 7.1 0 0 1 7.18-7 7.26 7.26 0 0 1 4.65 1.67l-2.17-.36a1 1 0 0 0-1.15.83 1 1 0 0 0 .83 1.15l4.24.7h.17a1 1 0 0 0 .34-.06.33.33 0 0 0 .1-.06.78.78 0 0 0 .2-.11l.09-.11c0-.05.09-.09.13-.15s0-.1.05-.14a1.34 1.34 0 0 0 .07-.18l.75-4a1 1 0 0 0-2-.38l-.27 1.45A9.21 9.21 0 0 0 12.18 3 9.1 9.1 0 0 0 3 12a9.1 9.1 0 0 0 9.18 9A9.12 9.12 0 0 0 21 14.68a1 1 0 0 0-.7-1.25z"
                                />
                            </g>
                        </g>
                    </svg>
                    Retry
                </button>
            </div>
        {:else if otaProgress}
            <h6>Update is in progress. Please do not close this page.</h6>
            <progress value={otaPercentage} max="100" />{otaPercentage}%
        {:else}
            <h6>Select what you want to update:</h6>
            <form on:submit|preventDefault={otaUpdate}>
                <label class="input">
                    <input
                        bind:group={otaKind}
                        type="radio"
                        name="firmwaretype"
                        value="firmware"
                    />Firmware
                </label>
                <label class="input">
                    <input
                        bind:group={otaKind}
                        type="radio"
                        name="firmwaretype"
                        value="filesystem"
                    />Theme
                </label>
                <button
                    type="button"
                    on:click={() => {
                        fileinput.click();
                    }}
                    disabled={!otaKind}
                >
                    {#if selectedFile}
                        {selectedFile.name} {prettyBytes(selectedFile.size)}
                    {:else}
                        Choose Firmware
                    {/if}
                </button>
                <input
                    style="display:none"
                    type="file"
                    accept=".bin,.bin.gz"
                    on:change={(e) => onFileSelected(e)}
                    bind:this={fileinput}
                />
                <button type="submit" disabled={!otaKind || !selectedFile}
                    >Upload</button
                >
            </form>
        {/if}
    </Route>
    <Route path="/theme/*">
        <Theme {hash} />
    </Route>

    <footer>
        <hr />
        <p class="read-the-docs">
            <small>
                Sincerely, your lovely Knomi display.<br />
                Firmware built from
                <span data-tooltip="Commit {hash} from {gitTimestamp}"
                    ><b>{branch}</b> at <b>{buildTimestamp}</b></span
                >. <br />
                Check
                <a href="https://github.com/DiverOfDark/KNOMI" target="_blank"
                    >repository</a
                > for more details.
            </small>
        </p>
    </footer>
</main>

<style global>
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
        margin: 0;
    }

    :global(.active) {
        background: #ff030340;
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
