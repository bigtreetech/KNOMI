<script lang="ts">
    import prettyBytes from "pretty-bytes";
    import { active, Route } from "tinro";
    import ThemeFile from "./ThemeFile.svelte";

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

    function getFile(filesList: ListFilesResponse, filename: string) {
        return filesList!.files.filter((t) => t.name == filename)[0];
    }

    load();
</script>

<div>
    {#if !filesList}
        <div aria-busy="true">Loading...</div>
    {:else}
        <Route path="/:filename" let:meta>
            <ThemeFile
                filename={getFile(filesList, meta.params.filename).name}
                {hash}
                size={getFile(filesList, meta.params.filename).size}
                on:uploaded={load}
            />
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
