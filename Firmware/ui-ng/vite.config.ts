import {defineConfig} from 'vite'
import {svelte} from '@sveltejs/vite-plugin-svelte'
import {viteSingleFile} from "vite-plugin-singlefile"
import viteCompression from 'vite-plugin-compression2';
import svg from 'vite-plugin-svgo';

// https://vitejs.dev/config/
export default defineConfig({
    root: 'src',
    build: {
        outDir: '../dist'
    },
    plugins: [svg(), svelte(), viteSingleFile(), viteCompression({algorithm: "gzip", deleteOriginalAssets: false })],
})
