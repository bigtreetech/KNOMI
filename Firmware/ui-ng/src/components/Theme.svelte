<script lang="ts">
    import prettyBytes from "pretty-bytes";
    import { active, Route, router } from "tinro";

    interface ListFilesResponse {
        total: number;
        used: number;
        files: FileInfo[];
    }

    interface FileInfo {
        name: string;
        size: number;
    }

    let filesList: ListFilesResponse | null = null;
    let hash = "master";

    async function load() {
        let statusResponse = await fetch("/api/status");
        let statusJson = await statusResponse.json();
        hash = statusJson.hash;

        let response = await fetch("/api/listFiles");
        let json = await response.json();
        filesList = json;
        filesList!.files = filesList?.files.filter((f) => f.name)!;
    }

    function getFile(filename: string) {
        return filesList!.files.filter((t) => t.name == filename)[0];
    }

    load();
</script>

<div>
    {#if !filesList}
        <div aria-busy="true">Loading...</div>
    {:else}
        <Route path="/:filename" let:meta>
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
                            {getFile(meta.params.filename).name} ({prettyBytes(
                                getFile(meta.params.filename).size,
                            )})
                        </span>
                    </div>
                </header>
                <div class="grid">
                    <div>
                        <h5>Current:</h5>
                        <img
                            src="/fs/{getFile(meta.params.filename).name}"
                            alt={meta.params.filename}
                        />
                    </div>
                    <div>
                        <h5>Original:</h5>
                        <img
                            src="https://github.com/DiverOfDark/KNOMI/raw/{hash}/Firmware/data/{meta
                                .params.filename}"
                            alt="Original {meta.params.filename}"
                        />
                    </div>
                </div>
                <footer>
                    <div class="grid">
                        <div>
                            <button disabled>Upload new image</button>
                        </div>
                        <div>
                            <button disabled>Restore Original</button>
                        </div>
                    </div>
                </footer>
            </article>
        </Route>
        <Route path="/">
            <table role="grid">
                <thead>
                    <tr>
                        <th scope="col">Filename</th>
                        <th scope="col">Size</th>
                    </tr>
                </thead>
                {#each filesList.files as file}
                    <tr>
                        <td
                            ><a href="/theme/{file.name}" use:active
                                >{file.name}</a
                            ></td
                        >
                        <td>{prettyBytes(file.size)}</td>
                    </tr>
                {/each}
            </table>
            <div></div>
        </Route>

        Free space {prettyBytes(filesList.used)} / {prettyBytes(
            filesList.total,
        )} <progress value={filesList.used} max={filesList.total} />
    {/if}
</div>
