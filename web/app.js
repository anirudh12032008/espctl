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
        updateValue(id, val);
    } else if (line.startsWith("$R")){
        logMonitor("[pin] "+ line.slice(2))
    } else {
        logMonitor(line);
    }
}