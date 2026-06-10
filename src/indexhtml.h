
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
   <head>
      <meta charset="UTF-8" />
      <meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover" />
      <meta name="theme-color" content="#161718" />
      <!-- PWA Meta Tags -->
      <meta name="apple-mobile-web-app-capable" content="yes" />
      <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent" />
      <meta name="apple-mobile-web-app-title" content="SensorWatch" />
      <link rel="manifest" href="manifest.json" />
      <link rel="apple-touch-icon" href="icons/icon-192x192.png" />
      <title>Мониторинг датчиков</title>
     
      <style>
         * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: "Segoe UI", system-ui, -apple-system, BlinkMacSystemFont, 'Roboto', sans-serif;
         }
         body {
            background: #161718;
            min-height: 100vh;
            padding: 2rem 1.5rem;
            color: #fff;
         }
         .dashboard {
            max-width: 1400px;
            margin: 0 auto;
         }
         .header {
            display: flex;
            justify-content: flex-end;
            align-items: center;
            margin-bottom: 2rem;
            border-bottom: 1px solid #222324;
            padding-bottom: 1rem;
            flex-wrap: wrap;
            gap: 1rem;
         }
         .install-area {
            display: flex;
            gap: 0.8rem;
            align-items: center;
         }
         .install-btn {
            background: #2c2e30;
            border: none;
            color: white;
            padding: 0.4rem 1rem;
            border-radius: 40px;
            font-size: 0.8rem;
            font-weight: 500;
            cursor: pointer;
            transition: 0.2s;
            backdrop-filter: blur(4px);
            display: inline-flex;
            align-items: center;
            gap: 6px;
         }
         .install-btn:hover {
            background: #3a6ea5;
            transform: scale(0.98);
         }
         .status-area {
            background: #1d1e1f;
            padding: 0.5rem 1.2rem;
            border-radius: 8px;
            border: 1px solid #222324;
            display: flex;
            align-items: center;
            gap: 1rem;
            font-size: 0.9rem;
         }
         .led {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background-color: #44ff44;
            box-shadow: 0 0 4px #44ff44;
            transition: 0.1s;
         }
         .led.error {
            background-color: #ff4444;
            box-shadow: 0 0 4px #ff4444;
         }
         .sensors-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
            gap: 1.8rem;
            margin-bottom: 2rem;
         }
         .sensor-card {
            background: #1d1e1f;
            border-radius: 8px;
            padding: 1.4rem 1.2rem 1.8rem;
            border: 1px solid #222324;
            transition: transform 0.1s ease, box-shadow 0.2s;
         }
         .sensor-card:active {
            transform: scale(0.99);
         }
         .card-header {
            margin-bottom: 1rem;
            border-left: 3px solid #3a6ea5;
            padding-left: 0.8rem;
         }
         .sensor-name {
            font-size: 1.35rem;
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.5px;
         }
         .params-list {
            display: flex;
            flex-direction: column;
            gap: 0.8rem;
            margin: 1rem 0;
         }
         .param-item {
            display: flex;
            justify-content: space-between;
            align-items: baseline;
            padding: 0.5rem 0.8rem;
            background: #161718;
            border-radius: 8px;
         }
         .param-name {
            font-size: 0.85rem;
            color: #aaa;
         }
         .param-value {
            font-size: 1.4rem;
            font-weight: 700;
         }
         .param-unit {
            font-size: 0.8rem;
            color: #aaa;
            margin-left: 0.3rem;
         }
         .param-error {
            color: #ff8888;
            font-size: 0.9rem;
         }
         .extra-info {
            font-size: 0.75rem;
            color: #aaa;
            border-top: 1px solid #222324;
            margin-top: 0.8rem;
            padding-top: 0.7rem;
            display: flex;
            justify-content: space-between;
         }
         .footer {
            text-align: center;
            margin-top: 2rem;
            font-size: 0.7rem;
            color: #555;
         }
         @media (max-width: 560px) {
            body {
               padding: 1rem;
            }
            .param-value {
               font-size: 1.2rem;
            }
            .status-area {
               font-size: 0.75rem;
               gap: 0.5rem;
            }
         }
         /* offline splash hint */
         .offline-toast {
            position: fixed;
            bottom: 20px;
            left: 50%;
            transform: translateX(-50%);
            background: #2c2e30e6;
            backdrop-filter: blur(12px);
            padding: 8px 18px;
            border-radius: 60px;
            font-size: 0.75rem;
            color: #ffaa66;
            border: 1px solid #444;
            pointer-events: none;
            z-index: 1000;
            font-family: monospace;
         }
      </style>
   </head>
   <body>
      <div class="dashboard">
         <div class="header">
            <div class="install-area">
               <button id="installBtn" class="install-btn" style="display: none;">📲 Установить приложение</button>
            </div>
            <div class="status-area">
               <div class="led" id="statusLed"></div>
               <span id="statusText">подключение...</span>
               <span id="timestamp" style="font-family: monospace; font-size: 0.75rem">--:--:--</span>
            </div>
         </div>
         <div class="sensors-grid" id="sensorsGrid">
            <div style="grid-column: 1/-1; text-align: center; padding: 3rem">
               Загрузка данных...
            </div>
         </div>
         <div class="footer">
            ⚡ PWA ready · HTTP GET → http://192.168.0.110/api/sensors · обновление 1с
         </div>
      </div>

      <script>
         // ---------- PWA CORE & SERVICE WORKER REGISTRATION ----------
         const API_URL = "http://192.168.0.110/api/sensors",
            REFRESH_INTERVAL_MS = 1000;
         const sensorsGrid = document.getElementById("sensorsGrid"),
            statusLed = document.getElementById("statusLed"),
            statusTextSpan = document.getElementById("statusText"),
            timestampSpan = document.getElementById("timestamp");
         let currentSensorsData = {};
         let deferredPrompt = null;
         const installBtn = document.getElementById("installBtn");

         // PWA: Register service worker for offline caching & installability
         if ('serviceWorker' in navigator) {
            window.addEventListener('load', () => {
               navigator.serviceWorker.register('/sw.js').then(registration => {
                  console.log('[PWA] Service Worker registered with scope:', registration.scope);
                  // optional: check for updates
                  registration.update();
               }).catch(err => {
                  console.error('[PWA] Service Worker registration failed:', err);
               });
            });
         }

         // Handle beforeinstallprompt event to show custom install button
         window.addEventListener('beforeinstallprompt', (e) => {
            // Prevent Chrome's mini-infobar from appearing on mobile
            e.preventDefault();
            deferredPrompt = e;
            // Show the install button
            installBtn.style.display = 'inline-flex';
            console.log('[PWA] beforeinstallprompt fired');
         });

         installBtn.addEventListener('click', async () => {
            if (!deferredPrompt) return;
            // Show the native install prompt
            deferredPrompt.prompt();
            // Wait for the user's response
            const { outcome } = await deferredPrompt.userChoice;
            console.log(`[PWA] User response: ${outcome}`);
            deferredPrompt = null;
            installBtn.style.display = 'none';
         });

         // Optional: hide install button if app is already installed
         window.addEventListener('appinstalled', (evt) => {
            console.log('[PWA] App installed successfully');
            installBtn.style.display = 'none';
            deferredPrompt = null;
         });

         // detect standalone mode (PWA launched from home screen)
         const isStandalone = window.matchMedia('(display-mode: standalone)').matches || 
                              window.navigator.standalone === true;
         if (isStandalone) {
            console.log('[PWA] Running in standalone mode');
            // minor: we can adjust UI, but not necessary
         }

         // ---------- CORE DASHBOARD LOGIC ----------
         const updateStatusUI = (ok, errMsg = null) => {
            if (ok) {
               statusLed.className = "led";
               statusTextSpan.innerHTML = "онлайн · датчики активны";
            } else {
               statusLed.className = "led error";
               statusTextSpan.innerHTML = `ошибка: ${errMsg || "нет соединения"}`;
            }
            const n = new Date();
            timestampSpan.innerText = n.toLocaleTimeString("ru-RU", { hour12: false });
         };

         const escapeHtml = (str) => {
            if (str === undefined || str === null) return "";
            return String(str).replace(/[&<>]/g, (m) =>
               m === "&" ? "&amp;" : m === "<" ? "&lt;" : "&gt;"
            );
         };

         const formatParamValue = (param) => {
            if (param && param.error)
               return `<span class="param-error">${escapeHtml(param.error)}</span>`;
            if (param && (param.value === undefined || param.value === null)) return "—";
            if (param && typeof param.value === "number") return param.value.toFixed(2);
            if (param && typeof param.value === "string") return escapeHtml(param.value);
            if (param && typeof param === "number") return param.toFixed(2);
            if (param === null || param === undefined) return "—";
            if (typeof param === "object" && param.value !== undefined) {
               let v = param.value;
               if (typeof v === "number") return v.toFixed(2);
               return escapeHtml(String(v));
            }
            return escapeHtml(String(param));
         };

         const renderCards = (sensorsData) => {
            if (!sensorsData || Object.keys(sensorsData).length === 0) {
               sensorsGrid.innerHTML = `<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">Нет данных датчиков</div>`;
               return;
            }
            let html = "";
            const now = new Date(),
               smallTime = now.toLocaleTimeString([], { hour: "2-digit", minute: "2-digit", second: "2-digit" });
            for (let [sensorName, sensorData] of Object.entries(sensorsData)) {
               let paramsHtml = "";
               // sensorData может быть объектом с параметрами или плоским объектом с temperature/humidity
               if (sensorData && typeof sensorData === 'object') {
                  for (let [paramName, paramData] of Object.entries(sensorData)) {
                     let unit = "";
                     let displayValue = "";
                     if (paramData && typeof paramData === 'object' && paramData.unit) {
                        unit = `<span class="param-unit">${escapeHtml(paramData.unit)}</span>`;
                        displayValue = formatParamValue(paramData);
                     } else if (paramData && typeof paramData === 'object' && (paramData.value !== undefined || paramData.error)) {
                        unit = paramData.unit ? `<span class="param-unit">${escapeHtml(paramData.unit)}</span>` : "";
                        displayValue = formatParamValue(paramData);
                     } else {
                        // прямое значение
                        displayValue = formatParamValue({ value: paramData });
                        unit = "";
                     }
                     paramsHtml += `<div class="param-item"><span class="param-name">${escapeHtml(paramName)}</span><div><span class="param-value">${displayValue}</span>${unit}</div></div>`;
                  }
               } else {
                  paramsHtml = `<div class="param-item"><span class="param-name">значение</span><div><span class="param-value">—</span></div></div>`;
               }
               html += `<div class="sensor-card"><div class="card-header"><span class="sensor-name">${escapeHtml(sensorName)}</span></div><div class="params-list">${paramsHtml}</div><div class="extra-info"><span>${smallTime}</span><span>🔵 PWA</span></div></div>`;
            }
            sensorsGrid.innerHTML = html;
         };

         function extractSensorsFromResponse(data) {
            let sensors = {};
            if (data.sensors && typeof data.sensors === "object") {
               return data.sensors;
            }
            for (let [key, value] of Object.entries(data)) {
               if (value && typeof value === "object" && !Array.isArray(value) &&
                  (value.temperature !== undefined || value.humidity !== undefined || value.pressure !== undefined || value.value !== undefined)) {
                  sensors[key] = value;
               }
            }
            if (Object.keys(sensors).length === 0) {
               if (data.temperature !== undefined || data.humidity !== undefined || data.pressure !== undefined) {
                  sensors["датчик"] = data;
               }
            }
            return sensors;
         }

         // Кэширование последних данных для офлайн-рендера (service worker позаботится, но мы для UI)
         async function fetchSensorsData() {
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 4000);
            try {
               const response = await fetch(API_URL, {
                  method: "GET",
                  mode: "cors",
                  cache: "no-store",
                  signal: controller.signal,
               });
               clearTimeout(timeoutId);
               if (!response.ok) throw new Error(`HTTP ${response.status}`);
               const contentType = response.headers.get("content-type");
               let jsonData;
               if (contentType && contentType.includes("application/json"))
                  jsonData = await response.json();
               else {
                  let txt = await response.text();
                  try {
                     jsonData = JSON.parse(txt);
                  } catch (e) {
                     throw new Error("Ответ не является валидным JSON");
                  }
               }
               updateStatusUI(true);
               let sensorsData = extractSensorsFromResponse(jsonData);
               if (Object.keys(sensorsData).length === 0) {
                  sensorsGrid.innerHTML = `<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">Не удалось распознать датчики</div>`;
                  currentSensorsData = {};
               } else {
                  currentSensorsData = sensorsData;
                  renderCards(sensorsData);
                  // сохраняем в localStorage для офлайн-резервного отображения (опционально)
                  try {
                     localStorage.setItem('sensors_backup', JSON.stringify({ data: sensorsData, timestamp: Date.now() }));
                  } catch(e) {}
               }
            } catch (err) {
               clearTimeout(timeoutId);
               let errMsg = err.name === "AbortError" ? "таймаут" :
                            err.message.includes("Failed to fetch") ? "сервер недоступен" : err.message;
               updateStatusUI(false, errMsg);
               
               // Пытаемся показать закэшированные данные из localStorage, если есть
               let cached = null;
               try {
                  const raw = localStorage.getItem('sensors_backup');
                  if (raw) {
                     const backup = JSON.parse(raw);
                     if (backup.data && Object.keys(backup.data).length > 0) {
                        cached = backup.data;
                     }
                  }
               } catch(e) {}
               if (cached && Object.keys(cached).length > 0) {
                  if (Object.keys(currentSensorsData).length === 0) {
                     currentSensorsData = cached;
                     renderCards(cached);
                     const offlineNote = document.createElement('div');
                     // gentle offline hint
                  } else {
                     renderCards(currentSensorsData);
                  }
               } else if (Object.keys(currentSensorsData).length === 0) {
                  sensorsGrid.innerHTML = `<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">📡 Нет соединения с сервером<br><span style="font-size:0.8rem;">Проверьте доступность ${API_URL}</span></div>`;
               } else {
                  renderCards(currentSensorsData);
               }
            }
            const now = new Date();
            timestampSpan.innerText = now.toLocaleTimeString("ru-RU", { hour12: false });
         }

         let intervalId = null;
         function startPolling() {
            if (intervalId) clearInterval(intervalId);
            fetchSensorsData();
            intervalId = setInterval(fetchSensorsData, REFRESH_INTERVAL_MS);
         }

         window.addEventListener("load", () => {
            startPolling();
            // маленький офлайн-детектор (необязательно)
            window.addEventListener('online', () => {
               console.log('connection restored');
               fetchSensorsData();
            });
         });
      </script>
   </body>
</html>
)rawliteral";
