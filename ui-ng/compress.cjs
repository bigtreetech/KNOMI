const FS = require("fs");
const path = require("path");

const BUNDLE_PAGE = FS.readFileSync(
    path.resolve(__dirname, "./dist/index.html.gz"),
);

function chunkArray(myArray, chunk_size) {
    let index = 0;
    const arrayLength = myArray.length;
    const tempArray = [];
    for (index = 0; index < arrayLength; index += chunk_size) {
        myChunk = myArray.slice(index, index + chunk_size);
        // Do something if you want with the group
        tempArray.push(myChunk);
    }
    return tempArray;
}

function addLineBreaks(buffer) {
    let data = "";
    const chunks = chunkArray(buffer, 30);
    chunks.forEach((chunk, index) => {
        data += chunk.join(",");
        if (index + 1 !== chunks.length) {
            data += ",\n";
        }
    });
    return data;
}

const FILE = `#pragma once

static const uint32_t KNOMI_HTML_SIZE = ${BUNDLE_PAGE.length};
static const char KNOMI_HTML[] = { 
${addLineBreaks(BUNDLE_PAGE)} 
};
`;

FS.writeFileSync(
    path.resolve(__dirname, "../src/generated/knomiWebpage.h"),
    FILE,
);
console.log(
    `[Generated] Generated webpage to knomiWebpage.h: ${(
        BUNDLE_PAGE.length / 1024
    ).toFixed(2)}KB`,
);
