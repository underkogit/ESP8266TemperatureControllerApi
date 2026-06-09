
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="theme-color" content="#161718">
    <link rel="manifest" href="manifest.json">
    <title>Мониторинг датчиков</title>
    <style>
        *{margin:0;padding:0;box-sizing:border-box;font-family:'Segoe UI',system-ui}
        body{background:#161718;min-height:100vh;padding:2rem 1.5rem;color:#fff}
        .dashboard{max-width:1400px;margin:0 auto}
        .header{display:flex;justify-content:flex-end;align-items:center;margin-bottom:2rem;border-bottom:1px solid #222324;padding-bottom:1rem}
        .status-area{background:#1d1e1f;padding:.5rem 1.2rem;border-radius:8px;border:1px solid #222324;display:flex;align-items:center;gap:1rem;font-size:.9rem}
        .led{width:12px;height:12px;border-radius:50%;background-color:#44ff44}
        .led.error{background-color:#ff4444}
        .sensors-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));gap:1.8rem;margin-bottom:2rem}
        .sensor-card{background:#1d1e1f;border-radius:8px;padding:1.4rem 1.2rem 1.8rem;border:1px solid #222324}
        .card-header{margin-bottom:1rem;border-left:3px solid #3a6ea5;padding-left:.8rem}
        .sensor-name{font-size:1.35rem;font-weight:600;text-transform:uppercase}
        .params-list{display:flex;flex-direction:column;gap:.8rem;margin:1rem 0}
        .param-item{display:flex;justify-content:space-between;align-items:baseline;padding:.5rem .8rem;background:#161718;border-radius:8px}
        .param-name{font-size:.85rem;color:#aaa}
        .param-value{font-size:1.4rem;font-weight:700}
        .param-unit{font-size:.8rem;color:#aaa;margin-left:.3rem}
        .param-error{color:#ff8888;font-size:.9rem}
        .extra-info{font-size:.75rem;color:#aaa;border-top:1px solid #222324;margin-top:.8rem;padding-top:.7rem;display:flex;justify-content:space-between}
        .footer{text-align:center;margin-top:2rem;font-size:.7rem;color:#555}
    </style>
</head>
<body>
<div class="dashboard">
    <div class="header">
        <div class="status-area">
            <div class="led" id="statusLed"></div>
            <span id="statusText">подключение...</span>
            <span id="timestamp" style="font-family:monospace;font-size:.75rem;">--:--:--</span>
        </div>
    </div>
    <div class="sensors-grid" id="sensorsGrid"><div style="grid-column:1/-1;text-align:center;padding:3rem;">Загрузка данных...</div></div>
    <div class="footer">HTTP GET → http://192.168.0.110/api/sensors · обновление 1с</div>
</div>

<script>
    const API_URL='http://192.168.0.110/api/sensors', REFRESH_INTERVAL_MS=1000;
    const sensorsGrid=document.getElementById('sensorsGrid'), statusLed=document.getElementById('statusLed'), statusTextSpan=document.getElementById('statusText'), timestampSpan=document.getElementById('timestamp');
    let currentSensorsData={};
    
    const updateStatusUI=(ok,errMsg=null)=>{if(ok){statusLed.className='led';statusTextSpan.innerHTML='онлайн · датчики активны'}else{statusLed.className='led error';statusTextSpan.innerHTML=`ошибка: ${errMsg||'нет соединения'}`}const n=new Date;timestampSpan.innerText=n.toLocaleTimeString('ru-RU',{hour12:false})};
    
    const escapeHtml=str=>{if(str===undefined||str===null)return '';return String(str).replace(/[&<>]/g,m=>m==='&'?'&amp;':m==='<'?'&lt;':'&gt;')};
    
    const formatParamValue=(param)=>{if(param.error)return `<span class="param-error">${escapeHtml(param.error)}</span>`;if(param.value===undefined||param.value===null)return '—';if(typeof param.value==='number')return param.value.toFixed(2);return escapeHtml(String(param.value))};
    
    const renderCards=(sensorsData)=>{if(!sensorsData || Object.keys(sensorsData).length===0){sensorsGrid.innerHTML=`<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">Нет данных датчиков</div>`;return}
    let html='';const now=new Date, smallTime=now.toLocaleTimeString([],{hour:'2-digit',minute:'2-digit',second:'2-digit'});
    for(let [sensorName, sensorData] of Object.entries(sensorsData)){let paramsHtml='';
        for(let [paramName, paramData] of Object.entries(sensorData)){let displayValue=formatParamValue(paramData),unit=paramData.unit?`<span class="param-unit">${escapeHtml(paramData.unit)}</span>`:'';
            paramsHtml+=`<div class="param-item"><span class="param-name">${escapeHtml(paramName)}</span><div><span class="param-value">${displayValue}</span>${unit}</div></div>`}
        html+=`<div class="sensor-card"><div class="card-header"><span class="sensor-name">${escapeHtml(sensorName)}</span></div><div class="params-list">${paramsHtml}</div><div class="extra-info"><span>${smallTime}</span><span>активен</span></div></div>`}
    sensorsGrid.innerHTML=html};
    
    function extractSensorsFromResponse(data){let sensors={};
        if(data.sensors && typeof data.sensors==='object'){return data.sensors}
        for(let [key, value] of Object.entries(data)){if(value && typeof value==='object' && !Array.isArray(value) && (value.temperature || value.humidity || value.pressure || value.value)){
                sensors[key]=value;}}
        if(Object.keys(sensors).length===0){if(data.temperature || data.humidity || data.pressure){sensors['датчик']=data}}
        return sensors}
    
    async function fetchSensorsData(){
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 4000);
        try{
            const response=await fetch(API_URL,{method:'GET',mode:'cors',cache:'no-store',signal:controller.signal});
            clearTimeout(timeoutId);
            if(!response.ok)throw new Error(`HTTP ${response.status}`);
            const contentType=response.headers.get('content-type');
            let jsonData;
            if(contentType&&contentType.includes('application/json'))jsonData=await response.json();
            else{let txt=await response.text();try{jsonData=JSON.parse(txt)}catch(e){throw new Error('Ответ не является валидным JSON')}}
            updateStatusUI(true);
            let sensorsData=extractSensorsFromResponse(jsonData);
            if(Object.keys(sensorsData).length===0){sensorsGrid.innerHTML=`<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">Не удалось распознать датчики</div>`;
                currentSensorsData={};}
            else{currentSensorsData=sensorsData;renderCards(sensorsData);}
        } catch(err){
            clearTimeout(timeoutId);
            let errMsg=err.name==='AbortError'?'таймаут':err.message.includes('Failed to fetch')?'сервер недоступен':err.message;
            updateStatusUI(false,errMsg);
            if(Object.keys(currentSensorsData).length===0)sensorsGrid.innerHTML=`<div style="grid-column:1/-1;text-align:center;padding:2rem;background:#1d1e1f;border-radius:8px;">Нет соединения с сервером</div>`;
            else renderCards(currentSensorsData);
        }
        const now=new Date;timestampSpan.innerText=now.toLocaleTimeString('ru-RU',{hour12:false});
    }
    
    let intervalId=null;
    function startPolling(){if(intervalId)clearInterval(intervalId);fetchSensorsData();intervalId=setInterval(fetchSensorsData,REFRESH_INTERVAL_MS)}
    
    window.addEventListener('load',()=>{startPolling()});
</script>
</body>
</html>
)rawliteral";
