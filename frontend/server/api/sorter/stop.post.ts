import { useSorterState } from '../utils/sorter-state'

export default defineEventHandler(async () => {
    const state = useSorterState()

    if (state.mode === 'idle') {
        throw createError({
            statusCode: 409,
            statusMessage: 'Sorter is not running'
        })
    }

    // Later:
    // await hardwareClient.stopSorter() - once the hardware logic is built

    state.mode = 'idle'
    state.sessionId = null
    state.startedAt = null
    state.bricksPerMinute = 0

    return {
        success: true,
        state
    }
})