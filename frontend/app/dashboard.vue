<script setup lang="ts">
import Camera from "./components/camera.vue"
import * as types from "./types/global_types.ts"

const {
    data: sorter,
    pending: sorterPending,
    error: sorterError,
    refresh: refreshSorter
} = await useFetch<types.SorterStatus>('/api/sorter/status')

const {
    data: detections,
    pending: detectionsPending,
    error: detectionsError,
    refresh: refreshDetections
} = await useFetch<types.Detection[]>('/api/detections', {
    default: () => []
})

async function startSorter() {
    await $fetch('/api/sorter/start', {
        method: 'POST'
    })

    await refreshSorter()
}

async function stopSorter() {
    await $fetch('/api/sorter/stop', {
        method: 'POST'
    })

    await refreshSorter()
}

let refreshTimer: ReturnType<typeof setInterval> | undefined
let isRefreshing = false

async function refreshDashboard() {
    if (isRefreshing) {
        return
    }

    isRefreshing = true

    try {
        await Promise.all([
            refreshSorter(),
            refreshDetections()
        ])
    } finally {
        isRefreshing = false
    }
}

onMounted(() => {
    refreshTimer = setInterval(refreshDashboard, 2000)
})

onUnmounted(() => {
    if (refreshTimer) {
        clearInterval(refreshTimer)
    }
})

// The below bricks are purely for testing the software, before the actual system is up and running - where we can then use real data/bricks
const testBricks = [
    {
        partId: '3001',
        name: 'Brick 2 x 4',
        colour: 'Red',
        confidence: 0.96,
        binId: '3'
    },
    {
        partId: '3023',
        name: 'Plate 1 x 2',
        colour: 'Blue',
        confidence: 0.88,
        binId: '2'
    },
    {
        partId: '3062',
        name: 'Round Brick 1 x 1',
        colour: 'Yellow',
        confidence: 0.92,
        binId: '1'
    }
]

async function simulateDetection() {
    const brick =
        testBricks[Math.floor(Math.random() * testBricks.length)]

    await $fetch('/api/detections', {
        method: 'POST',
        body: brick
    })

    await Promise.all([
        refreshSorter(),
        refreshDetections()
    ])
}
</script>

<template>
    <div class="min-h-screen bg-gray-100 dark:bg-gray-900 text-gray-800 dark:text-gray-100 font-sans flex">

        <!-- Sidebar -->
        <aside
            class="w-64 bg-white dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700 p-5 flex-col justify-between hidden md:flex">
            <div>
                <!-- System Title -->
                <div class="flex items-center gap-2 mb-6 pb-4 border-b border-gray-100 dark:border-gray-700">
                    <span class="font-bold text-base tracking-tight text-gray-900 dark:text-white">Lego Sorter</span>
                </div>
            </div>

            <!-- Fixed Connection Status Badge -->
            <div
                class="p-3 bg-gray-50 dark:bg-gray-900 rounded-md border border-gray-200 dark:border-gray-700 flex items-center justify-between text-xs">
                <span class="text-gray-500 font-mono">Hardware:</span>
                <span
                    class="px-2 py-0.5 text-xs font-semibold rounded bg-green-100 text-green-800 dark:bg-green-900/50 dark:text-green-300">Connected</span>
            </div>
        </aside>

        <!-- Main Workspace Area -->
        <main class="flex-1 p-6 md:p-8 overflow-y-auto w-full max-w-[1400px] mx-auto">
            <!-- Top Action Bar -->
            <header
                class="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4 mb-6 pb-4 border-b border-gray-200 dark:border-gray-700">
                <div>
                    <h1 class="text-xl font-bold text-gray-900 dark:text-white">Sorter Overview</h1>
                    <p class="text-gray-500 dark:text-gray-400 text-xs mt-0.5">Control conveyor hardware and monitor
                        visual brick classification metrics.</p>
                </div>
            </header>

            <!-- Dashboard Layout Grid -->
            <div class="grid grid-cols-1 lg:grid-cols-3 gap-6">
                <Camera />
                <section
                    class="mt-6 rounded-lg border border-gray-200 bg-white shadow-sm dark:border-gray-700 dark:bg-gray-800">
                    <div
                        class="flex items-center justify-between border-b border-gray-200 px-4 py-3 dark:border-gray-700">
                        <h2 class="text-xs font-bold uppercase tracking-wider text-gray-700 dark:text-gray-300">
                            Recent Detections
                        </h2>

                        <span class="text-xs text-gray-500">
                            {{ detections.length }} detected
                        </span>
                    </div>

                    <div class="p-4">
                        <p v-if="detectionsPending && detections.length === 0" class="text-sm text-gray-500">
                            Loading detections...
                        </p>

                        <div v-else-if="detectionsError && detections.length === 0"
                            class="rounded bg-red-50 p-3 text-sm text-red-700 dark:bg-red-950/40 dark:text-red-300">
                            Failed to load detections.

                            <button class="ml-2 underline" @click="refreshDetections()">
                                Retry
                            </button>
                        </div>

                        <p v-else-if="detections.length === 0" class="text-sm text-gray-500">
                            No LEGO bricks detected yet.
                        </p>

                        <div v-else class="space-y-3">
                            <article v-for="detection in detections" :key="detection.id"
                                class="flex items-center gap-4 rounded-md border border-gray-200 p-3 dark:border-gray-700">
                                <img v-if="detection.imageUrl" :src="detection.imageUrl" :alt="detection.name"
                                    class="h-14 w-14 rounded bg-gray-100 object-contain">

                                <div class="min-w-0 flex-1">
                                    <div class="flex justify-between gap-3">
                                        <p class="truncate font-semibold">
                                            {{ detection.name }}
                                        </p>

                                        <span class="text-sm font-semibold text-green-600">
                                            {{ Math.round(detection.confidence * 100) }}%
                                        </span>
                                    </div>

                                    <p class="text-xs text-gray-500">
                                        Part {{ detection.partId }}
                                        · {{ detection.colour }}
                                        · Bin {{ detection.binId }}
                                    </p>

                                    <p class="mt-1 text-xs text-gray-400">
                                        {{ new Date(detection.detectedAt).toLocaleString() }}
                                    </p>
                                </div>
                            </article>
                        </div>
                    </div>
                </section>

                <!-- Sorter Status -->
                <section
                    class="bg-white dark:bg-gray-800 rounded-lg border border-gray-200 dark:border-gray-700 shadow-sm">
                    <div class="px-4 py-3 border-b border-gray-200 dark:border-gray-700">
                        <h2 class="text-xs font-bold text-gray-700 dark:text-gray-300 uppercase tracking-wider">
                            Sorter Status
                        </h2>
                    </div>

                    <div class="p-5">
                        <p v-if="sorterPending && !sorter" class="text-sm text-gray-500">
                            Loading sorter status...
                        </p>

                        <div v-else-if="sorterError && !sorter"
                            class="rounded bg-red-50 dark:bg-red-950/40 p-3 text-sm text-red-700 dark:text-red-300">
                            Failed to load sorter status.

                            <button class="ml-2 underline" @click="refreshSorter()">
                                Retry
                            </button>
                        </div>

                        <div v-else-if="sorter" class="space-y-5">
                            <!-- Current State -->
                            <div class="flex items-center justify-between">
                                <span class="text-sm text-gray-500 dark:text-gray-400">
                                    State
                                </span>

                                <span class="rounded px-2 py-1 text-xs font-semibold uppercase" :class="{
                                    'bg-green-100 text-green-800 dark:bg-green-900/50 dark:text-green-300':
                                        sorter.mode === 'running',
                                    'bg-red-100 text-red-800 dark:bg-red-900/50 dark:text-red-300':
                                        sorter.mode === 'fault',
                                    'bg-gray-100 text-gray-700 dark:bg-gray-700 dark:text-gray-300':
                                        sorter.mode === 'idle',
                                    'bg-yellow-100 text-yellow-800 dark:bg-yellow-900/50 dark:text-yellow-300':
                                        sorter.mode === 'starting' || sorter.mode === 'stopping'
                                }">
                                    {{ sorter.mode }}
                                </span>
                            </div>

                            <!-- Statistics -->
                            <div class="grid grid-cols-2 gap-3">
                                <div class="rounded-md bg-gray-50 dark:bg-gray-900 p-3">
                                    <p class="text-xs text-gray-500">
                                        Processed
                                    </p>

                                    <p class="mt-1 text-2xl font-bold">
                                        {{ sorter.bricksProcessed }}
                                    </p>
                                </div>

                                <div class="rounded-md bg-gray-50 dark:bg-gray-900 p-3">
                                    <p class="text-xs text-gray-500">
                                        Bricks/min
                                    </p>

                                    <p class="mt-1 text-2xl font-bold">
                                        {{ sorter.bricksPerMinute }}
                                    </p>
                                </div>
                            </div>

                            <!-- Session -->
                            <div class="space-y-2 text-xs text-gray-500 dark:text-gray-400">
                                <p>
                                    Session:
                                    <span class="font-mono">
                                        {{ sorter.sessionId ?? 'None' }}
                                    </span>
                                </p>

                                <p>
                                    Started:
                                    <span>
                                        {{
                                            sorter.startedAt
                                                ? new Date(sorter.startedAt).toLocaleString()
                                                : 'Not running'
                                        }}
                                    </span>
                                </p>
                            </div>

                            <!-- Error -->
                            <div v-if="sorter.lastError"
                                class="rounded bg-red-50 dark:bg-red-950/40 p-3 text-xs text-red-700 dark:text-red-300">
                                {{ sorter.lastError }}
                            </div>

                            <!-- Controls -->
                            <div class="grid grid-cols-2 gap-3">
                                <button
                                    class="rounded-md bg-green-600 px-4 py-2 text-sm font-semibold text-white hover:bg-green-700 disabled:cursor-not-allowed disabled:opacity-50"
                                    :disabled="sorter.mode !== 'idle'" @click="startSorter">
                                    Start
                                </button>

                                <button
                                    class="rounded-md bg-red-600 px-4 py-2 text-sm font-semibold text-white hover:bg-red-700 disabled:cursor-not-allowed disabled:opacity-50"
                                    :disabled="sorter.mode !== 'running'" @click="stopSorter">
                                    Stop
                                </button>

                                <h5>DEV Only Simulation:</h5>
                                <button
                                    class="col-span-2 rounded-md bg-blue-600 px-4 py-2 text-sm font-semibold text-white hover:bg-blue-700 disabled:opacity-50"
                                    :disabled="sorter?.mode !== 'running'" @click="simulateDetection">
                                    Simulate Brick
                                </button>
                            </div>
                        </div>
                    </div>
                </section>
            </div>
        </main>
    </div>
</template>