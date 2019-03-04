const char* PROGMEM HTML = "<html><head><style type=\"text/css\">#colorpicker{--shadow:\"\";width:97vw;height:512px;image-rendering:pixelated;border-radius:6px;transition:.25s;box-shadow:var(--shadow);-moz-user-select:-moz-none;-khtml-user-select:none;-webkit-user-select:none}.slider{-webkit-appearance:slider-vertical;-moz-appearance:slider-vertical;padding:0 4em}fieldset{display:inline;border-radius:6px;padding:1em}input[type=button]{margin:0 .5em;height:5em}legend{text-align:center}body{display:flex;flex-direction:column;position:absolute;top:0;left:0;overflow-y:hidden}div{width:100%;display:flex;align-items:center;align-content:space-around;justify-content:center;justify-items:center;align-self:center}</style><script type=\"text/javascript\">const t=\"http://192.168.0.69?\";let e=!0;const o={f:\"Fill (solid)\",s:\"Sparkle\",g:\"Gradient\",G:\"Grainbient\",v:\"Visualizer (solid)\",V:\"Visualizer (rainbow)\",t:\"Theater Chase\",r:\"Gradient (rainbow)\",R:\"RGB Solid Fill Cycle\"};async function n(o){if(!e)return;let n=t;for(const[t,e]of Object.entries(o))n+=t+\"=\"+e+\"&\";n=n.slice(0,-1),console.log(n);const i=await fetch(n,{mode:\"no-cors\",credentials:\"include\"});await i.text();e=!0}const i=document.createElement(\"CANVAS\"),l=i.getContext(\"2d\");var a=function(t){let e=l.getImageData(t.offsetX,t.offsetY,1,1).data,o={r:e[0],g:e[1],b:e[2]};t.target.style.setProperty(\"--shadow\",`0 0 200px 100px rgb(${e[0]},${e[1]},${e[2]})`),n(o)};window.addEventListener(\"DOMContentLoaded\",()=>{const t=document.getElementById(\"colorpicker\");t.onload=(e=>{for(s of(i.width=t.width,i.height=t.height,l.drawImage(t,0,0,t.width,t.height),i.resize=function(){i.width=t.width,i.height=t.height,l.clearRect(0,0,i.width,i.height),l.drawImage(t,0,0,t.width,t.height)},i.resize(),t.ondragstart=function(t){t.preventDefault()},t.onmousedown=function(t){i.resize(),this.onmousemove=a,a(t)},t.onmouseup=function(t){this.onmousemove=null,this.style.setProperty(\"--shadow\",\"\")},t.ontouchstart=function(t){i.resize(),this.ontouchmove=a,a(t)},t.ontouchend=function(t){this.ontouchmove=null,this.style.setProperty(\"--shadow\",\"\")},t.ondblclick=function(t){this.onmousemove?(this.onmousemove=null,this.style.setProperty(\"--shadow\",\"\")):(i.resize(),this.onmousemove=a,a(t))},document.getElementsByClassName(\"slider\")))s.onmousewheel=function(t){this.value-=t.deltaY/10,this.oninput()},s.addEventListener(\"DOMMouseScroll\",function(t){this.value-=t.detail,this.oninput()},!1)});const e=document.getElementById(\"buttons\");for(m in o){let t=document.createElement(\"input\");t.type=\"button\",t.value=o[m],t.name=m,t.onclick=function(t){n({m:this.name})},e.appendChild(t)}});</script></head><body><fieldset><legend>RGB</legend><img draggable=\"false\" id=\"colorpicker\" src=\"data:image/png;base64,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\"></fieldset><div><fieldset id=\"buttons\" style=\"display:inline-block;width:fit-content\"><legend>Modes</legend></fieldset></div><div><fieldset><legend>Width</legend><input class=\"slider\" max=\"50\" min=\"2\" name=\"w\" oninput=\"sendPacket({[this.name]:this.value})\" orient=\"vertical\" type=\"range\" value=\"0\"></fieldset><fieldset><legend>Speed</legend><input class=\"slider\" max=\"128\" min=\"5\" name=\"f\" oninput=\"sendPacket({[this.name]:this.value})\" orient=\"vertical\" type=\"range\" value=\"0\"></fieldset><fieldset><legend>Population</legend><input class=\"slider\" max=\"180\" min=\"20\" name=\"p\" oninput=\"sendPacket({[this.name]:this.value})\" orient=\"vertical\" type=\"range\" value=\"20\"></fieldset></div></body></html>";
