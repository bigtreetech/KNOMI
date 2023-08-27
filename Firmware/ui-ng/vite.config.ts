import {defineConfig} from 'vite'
import {svelte} from '@sveltejs/vite-plugin-svelte'
import {viteSingleFile} from "vite-plugin-singlefile"
import viteCompression from 'vite-plugin-compression2';
import svg from 'vite-plugin-svgo';

// https://vitejs.dev/config/
export default defineConfig({
    root: 'src',
    build: {
        outDir: '../dist',
        emptyOutDir: true,
        rollupOptions: {
            output: {
                entryFileNames: `assets/[name].js`,
                chunkFileNames: `assets/[name].js`,
                assetFileNames: `assets/[name].[ext]`
            }
        }
    },
    plugins: [svg(), svelte(), viteSingleFile(), viteCompression({algorithm: "gzip", deleteOriginalAssets: false})],
})
