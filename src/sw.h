const char index_sw_js[] PROGMEM = R"rawliteral(
const CACHE_NAME = 'sensor-cache-v1';
const STATIC_ASSETS = [
  '/',
  '/index.html',
  '/manifest.json',
  '/sw.js'
];

// Установка сервис-воркера и кэширование статики
self.addEventListener('install', event => {
  console.log('[SW] Installing...');
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => {
      console.log('[SW] Caching static assets');
      return cache.addAll(STATIC_ASSETS);
    })
  );
  self.skipWaiting();
});

// Активация - удаляем старые кэши
self.addEventListener('activate', event => {
  console.log('[SW] Activating...');
  event.waitUntil(
    caches.keys().then(keys => {
      return Promise.all(
        keys.filter(key => key !== CACHE_NAME)
            .map(key => caches.delete(key))
      );
    })
  );
  self.clients.claim();
});

// Стратегия: Stale-While-Revalidate для API, Cache First для статики
self.addEventListener('fetch', event => {
  const url = new URL(event.request.url);
  
  // API запросы к датчикам - стратегия "сеть с падбэком на кэш"
  if (url.pathname === '/api/sensors' || url.href.includes('/api/sensors')) {
    event.respondWith(
      fetch(event.request)
        .then(response => {
          // Клонируем ответ и сохраняем в кэш
          const responseClone = response.clone();
          caches.open(CACHE_NAME).then(cache => {
            cache.put(event.request, responseClone);
          });
          return response;
        })
        .catch(() => {
          // Если сеть недоступна, пытаемся отдать из кэша
          return caches.match(event.request).then(cached => {
            if (cached) {
              return cached;
            }
            // Если нет кэша, возвращаем fallback-ответ
            return new Response(JSON.stringify({
              error: 'Нет соединения с сервером',
              timestamp: Date.now()
            }), {
              status: 503,
              headers: { 'Content-Type': 'application/json' }
            });
          });
        })
    );
    return;
  }
  
  // Для статических ресурсов (HTML, CSS, JS, иконки) - Cache First
  event.respondWith(
    caches.match(event.request)
      .then(cached => {
        if (cached) {
          return cached;
        }
        return fetch(event.request)
          .then(response => {
            // Сохраняем в кэш только успешные ответы
            if (response && response.status === 200) {
              const responseClone = response.clone();
              caches.open(CACHE_NAME).then(cache => {
                cache.put(event.request, responseClone);
              });
            }
            return response;
          })
          .catch(() => {
            // Для навигационных запросов возвращаем офлайн-страницу
            if (event.request.mode === 'navigate') {
              return caches.match('/');
            }
            return new Response('Офлайн режим', { status: 503 });
          });
      })
  );
});
)rawliteral";