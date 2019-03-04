const API_URL = "http://192.168.0.69?";
let cansend = true;

const LED_MODES = {
    "f": "Fill (solid)",
    "s": "Sparkle",
    "g": "Gradient",
    "G": "Grainbient",
    "v": "Visualizer (solid)",
    "V": "Visualizer (rainbow)",
    "t": "Theater Chase",
    "r": "Gradient (rainbow)",
    "R": "RGB Solid Fill Cycle"
}

async function sendPacket(data) {
    if (!cansend) return;
    let query = API_URL;
    for (const [key, value] of Object.entries(data)) query += key+"="+value+"&";
    query = query.slice(0, -1);
    console.log(query);
    const resp = await fetch(query, {
        mode: 'no-cors',
        credentials: "include"
    })
    const body = await resp.text();
    cansend = true;
}
const cv = document.createElement("CANVAS");
const ctx = cv.getContext('2d');

var getMousePixel = function (e) {
    let rgb = ctx.getImageData(e.offsetX, e.offsetY, 1, 1).data;
    let packet = {
        r: rgb[0],
        g: rgb[1],
        b: rgb[2]
    };
    e.target.style.setProperty("--shadow", `0 0 200px 100px rgb(${rgb[0]},${rgb[1]},${rgb[2]})`);
    sendPacket(packet);
};

window.addEventListener('DOMContentLoaded', () => {
    const hsv = document.getElementById("colorpicker");
    hsv.onload = e => {
        cv.width = hsv.width;
        cv.height = hsv.height;
        ctx.drawImage(hsv, 0, 0, hsv.width, hsv.height);

        cv.resize = function () {
            cv.width = hsv.width;
            cv.height = hsv.height;
            ctx.clearRect(0, 0, cv.width, cv.height)
            ctx.drawImage(hsv, 0, 0, hsv.width, hsv.height);
        }

        cv.resize();

        hsv.ondragstart = function (e) {
            e.preventDefault();
        };

        hsv.onmousedown = function (e) {
            cv.resize();
            this.onmousemove = getMousePixel;
            getMousePixel(e);
        }
        hsv.onmouseup = function (e) {
            this.onmousemove = null;
            this.style.setProperty("--shadow", "");
        }

        hsv.ontouchstart = function (e) {
            cv.resize();
            this.ontouchmove = getMousePixel;
            getMousePixel(e);
        }
        hsv.ontouchend = function (e) {
            this.ontouchmove = null;
            this.style.setProperty("--shadow", "");
        }

        hsv.ondblclick = function (e) {
            if (!this.onmousemove) {
                cv.resize();
                this.onmousemove = getMousePixel;
                getMousePixel(e);
            } else {
                this.onmousemove = null;
                this.style.setProperty("--shadow", "");
            }
        }

        for (s of document.getElementsByClassName("slider")) {
            s.onmousewheel = function (e) {
                this.value -= e.deltaY / 10
                this.oninput();
            };
            s.addEventListener('DOMMouseScroll', function (e) {
                this.value -= e.detail;
                this.oninput();
            }, false);
        }
    };

    const buttons = document.getElementById("buttons");
    for (m in LED_MODES) {
        let but = document.createElement("input");
        but.type = "button";
        but.value = LED_MODES[m];
        but.name = m;
        but.onclick = function (e) {
            sendPacket({
                "m": this.name
            })
        };
        buttons.appendChild(but);
    }
});