import {ESPLoader, Transport} from "https://unpkg.com/esptool-js@0.5.4/bundle.js";
const monitor = document.getElementById("monitor");

function log(text){
    monitor.textContent += text + "\n";
    monitor.scrollTop = monitor.scrollHeight;
}
const espTerm = {
    clean() {monitor.textContent = "";},
    writeLine(data) {log(data);},
    write(data) {monitor.textContent += data;},
};

document.getElementById("flashBtn").addEventListener("click", flash);

async function flash() {
    const inp = document.getElementById("binFile");
    if (!fileInput.files.length){
        log("pick a .bin file");
        return;
    }
    const offset = parseInt(document.getElementById("offset").value, 16);
    if (isNaN(offset)) {
        log("INVALID OFFSET");
        return;
    }
    const buf = await inp.files[0].arrayBuffer();
    const bytes = new Uint8Array(buf);
    let data = "";
    for (let i = 0; i < bytes.length; i++) data += String.fromCharCode(bytes[i]);
    let d;

    try {
        d = await navigator.serial.requestPort();

    } catch (e) {
        log("NO PORT SELECTED");
        return;
    }
    const transport = new Transport(d, true);
    try{
        const loader = new ESPLoader({
            transport,
            baudrate: 921600,
            terminal: espTerm,
        });
        const chip = await loader.main();
        log("DETECTED: "+ chip);
        await loader.writeFlash({
            fileArray: [{data, address: offset}],
            flashSize: "keep",
            eraseAll: false,
            compress: true,
            reportProgress(indx, written, total){
                log("progress: "+ Math.round((written/total)*100)+"%");
            },
        });

        await loader.after();
        log("DONE");

    } catch (e){
        log("ERROR: "+ e.message);
    } finally {
        await transport.disconnect();
    }
}