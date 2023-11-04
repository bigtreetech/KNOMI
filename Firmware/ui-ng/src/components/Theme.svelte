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
    var isSaving = false;
    var accentColor = "";
    var backgroundColor = "";
    export let hash = "master";

    async function load() {
        let themeConfig = await fetch("/api/status");
        let themeJson = await themeConfig.json();
        accentColor = themeJson.accentColor;
        backgroundColor = themeJson.backgroundColor;

        let response = await fetch("/api/listFiles");
        let json = await response.json();
        filesList = json;
        filesList!.files = filesList?.files.filter((f) => f.name)!;
        filesList!.files.sort((a, b) => a.name.localeCompare(b.name));
    }

    function getFile(filesList: ListFilesResponse, filename: string) {
        return filesList!.files.filter((t) => t.name == filename)[0];
    }

    async function saveThemeConfig() {
        isSaving = true;
        const data = new URLSearchParams();
        data.append("accentColor", accentColor);
        data.append("backgroundColor", backgroundColor);

        const res = await fetch("/api/themeConfig", {
            method: "POST",
            body: data,
        });
        if (res.status == 200) {
            isSaving = false;
            router.goto("/theme");
        } else if (res.status == 500) {
            const error = res.json().error;
            alert(error);
        }
        isSaving = false;
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
                usedSize={filesList.used}
                totalSize={filesList.total}
                on:uploaded={load}
            />
        </Route>
        <Route path="/">
            <form on:submit|preventDefault={saveThemeConfig}>
                <label class="input">
                    <span>UI Background Color</span>
                    <input
                        disabled={isSaving}
                        type="color"
                        bind:value={backgroundColor}
                    />
                </label>
                <label class="input">
                    <span>UI Accent Color</span>
                    <input
                        disabled={isSaving}
                        type="color"
                        bind:value={accentColor}
                    />
                </label>
                <button disabled={isSaving || null} type="submit">SAVE</button>
            </form>
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
                        <td
                            >{file.size == -1
                                ? "MISSING"
                                : prettyBytes(file.size)}</td
                        >
                    </tr>
                {/each}
            </table>
            <div></div>
        </Route>

        Used space {prettyBytes(filesList.used)} / {prettyBytes(
            filesList.total,
        )} <progress value={filesList.used} max={filesList.total} />
    {/if}
</div>
