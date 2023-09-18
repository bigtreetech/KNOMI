<script lang="ts">
    import prettyBytes from "pretty-bytes";
    
    interface ListFilesResponse {
        total: number;
        used: number;
        files: FileInfo[]
    };

    interface FileInfo {
        name: string;
        size: number;
    };

    let filesList : ListFilesResponse | null = null;

    async function load() {
        let response = await fetch("/api/listFiles");
        let json = await response.json();
        filesList = json;
        filesList.files = filesList?.files.filter(f=>f.name);
    }

    let selectedFile : FileInfo | null = null;

    load();
</script>

<div>
    {#if !filesList}
        <div aria-busy="true">Loading...</div>
    {:else}
        {#if selectedFile}
            <article>
                <header>
                    <div>
                        <button on:click|preventDefault={(e) => selectedFile = null}>Back</button>
                        <span>
                            {selectedFile.name} ({prettyBytes(selectedFile.size)})
                        </span>
                    </div>
                </header>
                <img src="/fs/{selectedFile.name}" alt="{selectedFile.name}"/>
            </article>
        {:else}
        <div>
            {#each filesList.files as file}
            <div>
                <button on:click|preventDefault={(e) => selectedFile = file}>{file.name} ({prettyBytes(file.size)})</button>
            </div>
            {/each}
        </div>
        {/if}
        Free space {prettyBytes(filesList.used)} / {prettyBytes(filesList.total)} <progress value="{filesList.used}" max="{filesList.total}" />
    {/if}
</div>