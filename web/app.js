let port = null;
let writer = null;



const PLOT_WIN = 30000;
let plotBuf = [];
let plotId = null;
const SEND_INT = 50; // 20hz
const lastSent = {}


function sendSet(id, value){
    const now = Date.now();
    const state = lastSent[id] || (lastSent[id] = { time: 0, value:null, pendingTimer: null });
    if (state.value === value) return;

    if (now - state.time >= SEND_INT) {
        state.time = now;
        state.value = value;
        writeLine("!" +id +","+ value)
    } else {
        clearTimeout(state.pendingTimer);
        state.pendingTimer = setTimeout(()=> {
            state.time = Date.now();
            state.value = value;
        writeLine("!" +id +","+ value);
        }, SEND_INT - (now - state.time));
    }
}



async function writeLine(line){
    if (!writer) return;
    await writer.write(new TextEncoder().encode(line + "\n"));
}


document.getElementById("connectBtn").addEventListener("click", connect);

async function connect() {
    port = await navigation.serial.requestPort();
    await port.open({baudRate: 921600});
    writer = port.writable.getWriter();
    document.getElementById("status").textContent = "connected";

    readLoop();
    writeLine("?")
    requestAnimationFrame(drawPlot);

}
async function readLoop() {
  const reader = port.readable.getReader();
  const decoder = new TextDecoder();
  let pending = "";
  while(true){
    const {value, done} = await reader.read();
    if (done) break;
    pending += decoder.decode(value, {stream: true});
    const lines = pending.split("\n");
    pending = lines.pop();
    for (const line of lines){
        handleLine(line.replace(/\r$/, ""));
    }


  }
}



function handleLine(line) {
    if (!line) return;
    if ( line.startsWith("$S")){
        renderWidgets(JSON.parse(line.slice(2)));
    } else if (line.startsWith("$D")){
        const c = line.lastIndexOf(",");
        const id = line.slice(2, c);
        const val = parseFloat(line.slice(c+1));
        updateVal(id, val);
    } else if (line.startsWith("$R")){
        logMonitor("[pin] "+ line.slice(2))
    } else {
        logMonitor(line);
    }
}





function renderWidgets(schema){
    const cont = document.getElementById("widgets");
    cont.innerHTML ="";
    id = null;
    for (const w of schema.widgets){
        const div = document.createElement("div");
        div.className = "widget";
        const label = document.createElement("label");
        label.textContent = w.id + ": ";
        div.appendChild(label);
        if (w.type === "slider"){
            const input = document.createElement("input");
            input.type = "range";
            input.min = w.min;
            input.max = w.max;
            input.value = w.value;
            const readout = document.createElement("span");
            readout.textContent = w.value;
            input.addEventListener("input", ()=> {
                readout.textContent = input.value;
                sendSet(w.id, input.value);
            });
            div.appendChild(input);
            div.appendChild(readout);
        } else if ( w.type === "toggle") {
            const input = document.createElement("input");
            input.type = "checkbox";
            input.checked = !!w.value;
            input.addEventListener("change", ()=> {
                sendSet(w.id, input.checked ? 1: 0);
            });
            div.appendChild(input);
        } else if (w.type === "value") {
            const readout = document.createElement("span");
            readout.id = "value-" + w.id;
            readout.textContent = "-";
            div.appendChild(readout);
            if (plotId === null) plotId = w.id;
        }
        cont.appendChild(div)
    }
}


function updateVal(id, val){
    const readout = document.getElementById("value-" +id);
    if (readout) readout.textContent = val;
    if ( id === plotId) {
        plotBuf.push({t: Date.now(), v: val});
        const cutoff = Date.now() - PLOT_WIN;
        while (plotBuf.length && plotBuf[0].t < cutoff) plotBuf.shift();

    }
}


