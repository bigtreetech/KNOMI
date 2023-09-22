<script lang="ts">
    import prettyBytes from "pretty-bytes";
    import { active } from "tinro";

    export let hash = "";
    export let filename = "";
    export let size = 0;

    let fileInput: HTMLInputElement;
    let uploadInProgress = false;
    let selectedFileError = "";
    
    function getExtension(filename: string) {
        let pos = filename.lastIndexOf(".");
        return filename.slice(pos);
    }

    function uploadFile(file: File) {
        if (!file)
            return;

        if (getExtension(file.name) === ".gif") {
            checkGif(file);
        } else if (getExtension(file.name) === ".bmp") {
            checkBmp(file);
        } else {
            selectedFileError = "Not supported file format! Expected '" + getExtension(filename) + "', but got '" + getExtension(file.name) + "'";
        }
    }

    function checkBmp(file: File) {
        // recode using https://github.com/shaozilee/bmp-js/blob/master/index.js
        selectedFileError = "Bmp not supported yet";
        return;

        loadToImgAndCheckResolution(file);
    }

    function checkGif(file: File) {
        loadToImgAndCheckResolution(file);
    }

    function loadToImgAndCheckResolution(file : File) {
        // check resolution - should be no more than CONST
        // check free space
    }
</script>

<article>
    <header>
        <div>
            <a
                role="button"
                href="/theme"
                class="outline"
                use:active>Back</a
            >
            <span>
                {filename} ({prettyBytes(size)})
            </span>
        </div>
    </header>
    <div class="grid">
        <div>
            <h5>Current:</h5>
            <img
                src="/fs/{filename}"
                alt={filename}
            />
        </div>
        <div>
            <h5>Original:</h5>
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
                    <input type="file" 
                        style="display: none" 
                        bind:this={fileInput} 
                        on:change={(e) => uploadFile(e.target.files[0]) }
                        accept={getExtension(filename)}
                        />
                    {#if selectedFileError }
                    <div class="error">{selectedFileError}</div>
                    {/if}
                    <button type="submit" aria-busy="{uploadInProgress}">Upload new image</button>
                </form>
            </div>
            <div>
                <button disabled>Restore Original</button>
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
</style>