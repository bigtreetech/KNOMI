<script lang="ts">
    import prettyBytes from "pretty-bytes";
    import { active } from "tinro";
    import { createEventDispatcher } from "svelte";

    const dispatch = createEventDispatcher();

    export let hash = "";
    export let filename = "";
    export let size = 0;

    export let usedSize = 0;
    export let totalSize = 0;

    let fileInput: HTMLInputElement;
    let uploadInProgress = false;
    let uploadProgress = 0;
    let selectedFileError: String | null = null;
    let reloadIter = Math.random();

    function getExtension(filename: string) {
        let pos = filename.lastIndexOf(".");
        return filename.slice(pos);
    }

    function uploadFile(file: File) {
        if (!file) return;

        const free_buffer = 8192;

        if (file.size > totalSize - usedSize - free_buffer + size) {
            selectedFileError =
                "Not enough free space for this file. Available is " +
                prettyBytes(totalSize - usedSize - free_buffer) +
                ", file is " +
                prettyBytes(file.size);
            return;
        }

        if (file.size)
            if (getExtension(file.name) === ".gif") {
                loadToImgAndCheckResolution(file);
            } else {
                selectedFileError =
                    "Not supported file format! Expected '" +
                    getExtension(filename) +
                    "', but got '" +
                    getExtension(file.name) +
                    "'";
            }
    }

    function loadToImgAndCheckResolution(file: File) {
        // check resolution - should be no more than CONST

        // todo extract file upload into ts function/helper
        const request = new XMLHttpRequest();
        const formData = new FormData();

        uploadInProgress = true;

        request.addEventListener("load", () => {
            // request.response will hold the response from the server
            if (request.status === 200) {
                selectedFileError = null;
                reloadIter = Math.random();
                dispatch("uploaded");
            } else if (request.status !== 500) {
                selectedFileError = `[HTTP ERROR] ${request.statusText}`;
            } else {
                selectedFileError = request.responseText;
            }
            uploadInProgress = false;
            uploadProgress = 0;
        });

        // Upload progress
        request.upload.addEventListener("progress", (e) => {
            uploadProgress = Math.trunc((e.loaded / e.total) * 100);
        });

        request.withCredentials = true;
        request.open("post", "/api/uploadFile");
        formData.append("filename", filename);
        formData.append("size", file.size.toString());
        formData.append("file", file, filename);
        request.send(formData);
    }
</script>

<article>
    <header>
        <div>
            <a role="button" href="/theme" class="outline" use:active>Back</a>
            <span>
                {filename} ({size == -1 ? "MISSING" : prettyBytes(size)})
            </span>
        </div>
    </header>
    <div class="grid">
        <div>
            <h5>Current</h5>
            <div class="imgContainer">
                {#if uploadInProgress}
                    <div class="imgOverlay">
                        <div aria-busy={uploadInProgress}>updating</div>
                    </div>
                {/if}
                {#if size == -1}
                    <h3>NOT FOUND</h3>
                {:else}
                    <img src="/fs/{filename}?{reloadIter}" alt={filename} />
                {/if}
            </div>
        </div>
        <div>
            <h5>Original</h5>
            <img
                src="https://github.com/DiverOfDark/KNOMI/raw/{hash}/Firmware/data/{filename}"
                alt="Original {filename}"
            />
        </div>
    </div>
    <footer>
        <div class="grid">
            <div>
                <form on:submit|preventDefault={() => fileInput.click()}>
                    <input
                        type="file"
                        style="display: none"
                        bind:this={fileInput}
                        on:change={(e) => uploadFile(e.target.files[0])}
                        accept={getExtension(filename)}
                    />
                    {#if selectedFileError}
                        <div class="error">{selectedFileError}</div>
                    {/if}
                    <button
                        type="submit"
                        aria-busy={uploadInProgress}
                        disabled={uploadInProgress}>Upload new image</button
                    >
                </form>
            </div>
        </div>
    </footer>
</article>

<style>
    .error {
        padding: 1em;
        background-color: var(--del-color);
        margin-bottom: 1em;
        border-radius: var(--border-radius);
    }

    .imgContainer {
        position: relative;
        float: left;
    }

    .imgOverlay {
        background-color: rgba(0, 0, 0, 0.8);
        position: absolute;
        height: 100%;
        width: 100%;
        display: flex;
        justify-content: center;
        align-items: center;
    }
</style>
