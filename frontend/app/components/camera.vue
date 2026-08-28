<template>
    <!-- Left Side: Live Feed & Output Bins -->
    <div class="lg:col-span-2 space-y-6">

        <!-- Flat Camera Feed Container -->
        <div class="bg-white dark:bg-gray-800 rounded-lg border border-gray-200 dark:border-gray-700 shadow-sm">
            <div class="px-4 py-3 border-b border-gray-200 dark:border-gray-700 flex justify-between items-center">
                <h3
                    class="text-xs font-bold text-gray-700 dark:text-gray-300 uppercase tracking-wider flex items-center gap-2">
                    Cameras
                </h3>
                <span
                    class="text-xs font-mono px-2 py-0.5 rounded bg-gray-100 dark:bg-gray-900 text-gray-500 dark:text-gray-400">Disconnected</span>
            </div>

            <!-- Standard Video Window Frame -->
            <div
                class="aspect-video bg-gray-100 dark:bg-gray-950 flex flex-col items-center justify-center p-6 text-gray-400 dark:text-gray-600">
            </div>
        </div>
        <!-- Plain Console Log Output -->
        <div class="bg-white dark:bg-gray-800 rounded-lg border border-gray-200 dark:border-gray-700 p-5 shadow-sm">
            <h3 class="text-xs font-bold text-gray-700 dark:text-gray-300 uppercase tracking-wider mb-2">
                Console Output</h3>
            <div
                class="font-mono text-xs space-y-1.5 max-h-36 overflow-y-auto text-gray-600 dark:text-gray-400 bg-gray-50 dark:bg-gray-950 p-3 rounded border border-gray-100 dark:border-gray-800">
            </div>
        </div>
    </div>
</template>