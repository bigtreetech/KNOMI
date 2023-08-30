import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import { viteSingleFile } from "vite-plugin-singlefile";
import viteCompression from "vite-plugin-compression2";
import svg from "vite-plugin-svgo";

// https://vitejs.dev/config/
export default defineConfig({
  root: "src",

  server: {
    proxy: {
      '/update': 'http://knomi',
      '/api': 'http://knomi',
      '/fs': 'http://knomi',
    }
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
        replacement: process.cwd() + 'node_modules/$1'
      },
    ],
  },

  clearScreen: false,
  plugins: [
    svg(),
    svelte(),
    viteSingleFile(),
    viteCompression({ algorithm: "gzip", deleteOriginalAssets: false }),
  ]
});
