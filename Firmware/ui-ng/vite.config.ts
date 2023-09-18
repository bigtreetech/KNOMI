import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import { viteSingleFile } from "vite-plugin-singlefile";
import sveltePreprocess from "svelte-preprocess";
import viteCompression from "vite-plugin-compression2";
import svg from "vite-plugin-svgo";

// https://vitejs.dev/config/
export default defineConfig({
    root: "src",

    server: {
        proxy: {
            "/update": "http://knomi",
            "/api": "http://knomi",
            "/fs": "http://knomi",
            "/ws": {
                target: "ws://knomi",
                ws: true,
                secure: false,
                changeOrigin: true,
                rewrite: (path) => path.replace(/^\/ws/, ""),
                configure: (proxy, _options) => {
                    proxy.on("error", (err, _req, _res) => {
                        console.log("proxy error", err);
                    });
                    proxy.on("proxyReq", (proxyReq, req, _res) => {
                        console.log(
                            "Sending Request to the Target:",
                            req.method,
                            req.url,
                        );
                    });
                    proxy.on("proxyRes", (proxyRes, req, _res) => {
                        console.log(
                            "Received Response from the Target:",
                            proxyRes.statusCode,
                            req.url,
                        );
                    });
                },
            },
        },
    },

    build: {
        outDir: "../dist",
        emptyOutDir: true,
        rollupOptions: {
            output: {
                entryFileNames: `assets/[name].js`,
                chunkFileNames: `assets/[name].js`,
                assetFileNames: `assets/[name].[ext]`,
            },
        },
    },

    resolve: {
        alias: [
            {
                // this is required for the SCSS modules
                find: /^~(.*)$/,
                replacement: process.cwd() + "node_modules/$1",
            },
        ],
    },

    clearScreen: false,
    plugins: [
        svg(),
        svelte({
            preprocess: [sveltePreprocess({ typescript: true })],
        }),
        viteSingleFile(),
        viteCompression({ algorithm: "gzip", deleteOriginalAssets: false }),
    ],
});
