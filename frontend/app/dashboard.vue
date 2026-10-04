<script setup lang="ts">
import Camera from "./components/camera.vue";
import * as types from "./types/global_types.ts";
import { ref, onMounted, onUnmounted } from "vue";

const predictionData = ref<any>(null);
const isLoadingPrediction = ref(false);

async function predict(): Promise<any> {
  isLoadingPrediction.value = true;
  predictionData.value = null;

  try {
    const response = await $fetch<any>("/api/recognize");

    if (response && response.items && response.items.length > 0) {
      predictionData.value = response;
    } else {
      predictionData.value = { error: "No Lego matches found for this item." };
    }
  } catch (err) {
    console.error("Failed fetching prediction:", err);
    predictionData.value = {
      error: "Failed to fetch via internal proxy route",
    };
  } finally {
    isLoadingPrediction.value = false;
  }
}

const {
  data: sorter,
  pending: sorterPending,
  error: sorterError,
  refresh: refreshSorter,
} = await useFetch<types.SorterStatus>("/api/sorter/status");

const {
  data: detections,
  pending: detectionsPending,
  error: detectionsError,
  refresh: refreshDetections,
} = await useFetch<types.Detection[]>("/api/detections", {
  default: () => [],
});

async function startSorter() {
  await $fetch("/api/sorter/start", {
    method: "POST",
  });

  await refreshSorter();
}

async function stopSorter() {
  await $fetch("/api/sorter/stop", {
    method: "POST",
  });

  await refreshSorter();
}

let refreshTimer: ReturnType<typeof setInterval> | undefined;
let isRefreshing = false;

async function refreshDashboard() {
  if (isRefreshing) {
    return;
  }

  isRefreshing = true;

  try {
    await Promise.all([refreshSorter(), refreshDetections()]);
  } finally {
    isRefreshing = false;
  }
}

onMounted(() => {
  refreshTimer = setInterval(refreshDashboard, 2000);
});

onUnmounted(() => {
  if (refreshTimer) {
    clearInterval(refreshTimer);
  }
});

// The below bricks are purely for testing the software, before the actual system is up and running - where we can then use real data/bricks
const testBricks = [
  {
    partId: "3001",
    name: "Brick 2 x 4",
    colour: "Red",
    confidence: 0.96,
    binId: "3",
  },
  {
    partId: "3023",
    name: "Plate 1 x 2",
    colour: "Blue",
    confidence: 0.88,
    binId: "2",
  },
  {
    partId: "3062",
    name: "Round Brick 1 x 1",
    colour: "Yellow",
    confidence: 0.92,
    binId: "1",
  },
];

async function simulateDetection() {
  const brick = testBricks[Math.floor(Math.random() * testBricks.length)];

  await $fetch("/api/detections", {
    method: "POST",
    body: brick,
  });

  await Promise.all([refreshSorter(), refreshDetections()]);
}
</script>

<template>
  <div class="min-h-screen bg-gray-100 flex items-center justify-center p-3 sm:p-5 md:p-6 text-gray-900">
    <!-- Central application rectangle -->
    <main class="w-full max-w-7xl bg-white border border-gray-300 rounded-lg shadow-sm overflow-hidden">
      <!-- Header -->
      <header
        class="min-h-16 px-4 sm:px-6 py-4 flex flex-col gap-3 sm:flex-row sm:items-center sm:justify-between border-b border-gray-200">
        <div>
          <h1 class="text-lg font-semibold">
            LEGO Sorter
          </h1>

          <p class="text-xs text-gray-400">
            Vision sorting system
          </p>
        </div>

        <div class="flex items-center gap-2 text-xs text-gray-500">
          <span class="w-2 h-2 rounded-full bg-green-500"></span>
          Hardware connected
        </div>
      </header>

      <div class="grid grid-cols-1 md:grid-cols-3">

        <!-- CAMERA -->
        <section class="min-w-0 flex flex-col">
          <div class="h-12 px-4 sm:px-5 flex items-center border-b border-gray-200">
            <h2 class="text-xs font-semibold uppercase tracking-wider text-gray-500">
              Camera
            </h2>
          </div>

          <div class="p-4 sm:p-5">
            <Camera />
          </div>
        </section>

        <!-- PREDICTION -->
        <section
          class="min-w-0 flex flex-col border-t border-b md:border-t-0 md:border-b-0 md:border-l md:border-r border-gray-200">
          <div class="h-12 px-4 sm:px-5 flex items-center justify-between border-b border-gray-200">
            <h2 class="text-xs font-semibold uppercase tracking-wider text-gray-500">
              Identification
            </h2>

            <span v-if="predictionData?.listing_id" class="text-[10px] font-mono text-gray-400">
              {{ predictionData.listing_id.substring(4, 12) }}
            </span>
          </div>

          <div class="p-4 border-b border-gray-200">
            <button @click="predict()" :disabled="isLoadingPrediction"
              class="w-full rounded-md bg-blue-600 px-4 py-2.5 text-xs font-medium text-white hover:bg-blue-700 disabled:opacity-50 transition">
              {{ isLoadingPrediction ? "Analyzing..." : "Predict Brick" }}
            </button>
          </div>

          <div class="p-4 sm:p-5">
            <!-- Loading -->
            <div v-if="isLoadingPrediction" class="min-h-[220px] flex flex-col items-center justify-center text-center">
              <div class="h-7 w-7 rounded-full border-2 border-blue-600 border-t-transparent animate-spin mb-3" />

              <p class="text-xs text-gray-500">
                Running inference...
              </p>
            </div>

            <!-- Empty -->
            <div v-else-if="!predictionData"
              class="min-h-[220px] flex flex-col items-center justify-center text-center text-gray-400">
              <svg class="w-8 h-8 mb-3" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"
                  d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01" />
              </svg>

              <p class="text-xs">
                No prediction yet
              </p>
            </div>

            <!-- Error -->
            <div v-else-if="predictionData.error" class="rounded-md bg-red-50 border border-red-200 p-4 text-center">
              <p class="text-xs text-red-600">
                {{ predictionData.error }}
              </p>
            </div>

            <!-- Results -->
            <div v-else-if="predictionData?.items?.length" class="space-y-3">
              <article v-for="item in predictionData.items" :key="item.id"
                class="rounded-md border border-gray-200 p-3">
                <div class="flex gap-3">
                  <div class="w-16 h-16 shrink-0 rounded border border-gray-200 flex items-center justify-center">
                    <img :src="item.img_url" :alt="item.name" class="max-w-full max-h-full object-contain" />
                  </div>

                  <div class="min-w-0 flex-1">
                    <div class="flex justify-between gap-2">
                      <h3 class="text-sm font-semibold truncate">
                        {{ item.name }}
                      </h3>

                      <span class="text-xs font-semibold text-green-600 shrink-0">
                        {{ (item.score * 100).toFixed(1) }}%
                      </span>
                    </div>

                    <div class="mt-2 space-y-1 text-[11px] text-gray-500">
                      <p>
                        Part:
                        <span class="text-gray-800">
                          {{ item.id }}
                        </span>
                      </p>

                      <p>
                        Category:
                        <span class="text-gray-800">
                          {{ item.category }}
                        </span>
                      </p>

                      <p>
                        Type:
                        <span class="text-gray-800 capitalize">
                          {{ item.type }}
                        </span>
                      </p>
                    </div>
                  </div>
                </div>

                <div v-if="item.external_sites?.length" class="mt-3 pt-3 border-t border-gray-100 flex flex-wrap gap-3">
                  <a v-for="site in item.external_sites" :key="site.name" :href="site.url" target="_blank"
                    class="text-[10px] text-blue-600 hover:underline">
                    {{ site.name === "bricklink" ? "BrickLink" : site.name }}
                  </a>
                </div>
              </article>
            </div>
          </div>
        </section>

        <!-- SORTER -->
        <section class="min-w-0 flex flex-col">
          <div class="h-12 px-4 sm:px-5 flex items-center justify-between border-b border-gray-200">
            <h2 class="text-xs font-semibold uppercase tracking-wider text-gray-500">
              Sorter
            </h2>

            <span class="text-[10px] font-semibold uppercase" :class="{
              'text-green-600': sorter?.mode === 'running',
              'text-yellow-600':
                sorter?.mode === 'starting' ||
                sorter?.mode === 'stopping',
              'text-red-600': sorter?.mode === 'fault',
              'text-gray-400': sorter?.mode === 'idle',
            }">
              {{ sorter?.mode ?? "Loading" }}
            </span>
          </div>

          <div class="p-4 sm:p-5">
            <div v-if="sorter" class="space-y-5">
              <!-- Current state -->
              <div class="rounded-md border border-gray-200 p-5 text-center">
                <div class="mx-auto mb-3 w-3 h-3 rounded-full" :class="{
                  'bg-green-500 animate-pulse':
                    sorter.mode === 'running',
                  'bg-yellow-500':
                    sorter.mode === 'starting' ||
                    sorter.mode === 'stopping',
                  'bg-red-500':
                    sorter.mode === 'fault',
                  'bg-gray-400':
                    sorter.mode === 'idle',
                }" />

                <p class="text-sm font-semibold capitalize">
                  {{ sorter.mode }}
                </p>

                <p class="mt-1 text-xs text-gray-400">
                  {{ sorter.bricksProcessed }} bricks processed
                </p>
              </div>

              <!-- Stats -->
              <div class="grid grid-cols-2 gap-3">
                <div class="border border-gray-200 rounded-md p-3">
                  <p class="text-[10px] uppercase text-gray-400">
                    Processed
                  </p>

                  <p class="mt-1 text-xl font-semibold">
                    {{ sorter.bricksProcessed }}
                  </p>
                </div>

                <div class="border border-gray-200 rounded-md p-3">
                  <p class="text-[10px] uppercase text-gray-400">
                    Rate
                  </p>

                  <p class="mt-1 text-xl font-semibold">
                    {{ sorter.bricksPerMinute }}
                  </p>

                  <p class="text-[10px] text-gray-400">
                    bricks / min
                  </p>
                </div>
              </div>

              <!-- Controls -->
              <div class="grid grid-cols-2 gap-2">
                <button
                  class="rounded-md bg-green-600 px-3 py-2.5 text-xs font-semibold text-white hover:bg-green-700 disabled:opacity-40"
                  :disabled="sorter.mode !== 'idle'" @click="startSorter">
                  Start
                </button>

                <button
                  class="rounded-md bg-red-600 px-3 py-2.5 text-xs font-semibold text-white hover:bg-red-700 disabled:opacity-40"
                  :disabled="sorter.mode !== 'running'" @click="stopSorter">
                  Stop
                </button>
              </div>

              <!-- Session -->
              <div class="border-t border-gray-200 pt-4 space-y-2 text-[10px] text-gray-400">
                <div class="flex justify-between gap-3">
                  <span>Session</span>

                  <span class="font-mono truncate">
                    {{ sorter.sessionId ?? "None" }}
                  </span>
                </div>

                <div class="flex justify-between gap-3">
                  <span>Started</span>

                  <span class="text-right">
                    {{
                      sorter.startedAt
                        ? new Date(sorter.startedAt).toLocaleString()
                        : "Not running"
                    }}
                  </span>
                </div>
              </div>

              <!-- DEV -->
              <div class="border-t border-gray-200 pt-4">
                <p class="mb-2 text-[10px] uppercase text-gray-400">
                  Development
                </p>

                <button
                  class="w-full rounded-md border border-gray-300 px-3 py-2.5 text-xs text-gray-600 hover:bg-gray-50 disabled:opacity-40"
                  :disabled="sorter.mode !== 'running'" @click="simulateDetection">
                  Simulate Brick
                </button>
              </div>
            </div>

            <div v-else class="py-10 text-center text-xs text-gray-400">
              Loading sorter...
            </div>
          </div>
        </section>
      </div>
    </main>
  </div>
</template>